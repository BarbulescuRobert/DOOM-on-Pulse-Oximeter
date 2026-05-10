#!/usr/bin/env python3
"""
Convert a video to 1bpp RLE-compressed C frames for the 128x64 OLED.

Usage:
    python3 tools/video_to_frames.py <video> [options]

Dithering modes (--mode):
    threshold   Simple cutoff at 128. Compresses best, loses mid-tone detail.
    bayer       4x4 ordered dithering. Best balance of quality and compression.
    floyd       Floyd-Steinberg. Best quality, worst compression.

Outputs:
    src/doom_frames.c   (regenerated every run)

Requirements:
    ffmpeg, Pillow, numpy
"""

import argparse
import glob
import os
import subprocess
import sys
import tempfile
from pathlib import Path

import numpy as np

try:
    from PIL import Image, ImageEnhance
except ImportError:
    sys.exit("Pillow missing — run:  pip install Pillow")


W, H = 128, 64
BYTES_PER_FRAME = W * H // 8   # 1024 bytes raw per frame

# 4x4 Bayer matrix, normalized to [0, 1)
_BAYER_4 = np.array([
    [ 0,  8,  2, 10],
    [12,  4, 14,  6],
    [ 3, 11,  1,  9],
    [15,  7, 13,  5],
], dtype=np.float32) / 16.0


# ---------------------------------------------------------------------------
# frame extraction
# ---------------------------------------------------------------------------

def extract_frames(video: str, fps: int, start: float, duration, tmpdir: str):
    cmd = ["ffmpeg", "-y"]
    if start:
        cmd += ["-ss", str(start)]
    cmd += ["-i", video]
    if duration:
        cmd += ["-t", str(duration)]
    vf = f"fps={fps},scale={W}:{H}:flags=lanczos"
    cmd += ["-vf", vf, os.path.join(tmpdir, "frame_%05d.png")]
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        print(result.stderr.decode(), file=sys.stderr)
        sys.exit("ffmpeg failed")
    return sorted(glob.glob(os.path.join(tmpdir, "frame_*.png")))


# ---------------------------------------------------------------------------
# image → OLED 1bpp page-format bytes
# OLED layout: 8 pages × 128 columns, each byte = 8 vertical pixels (bit0=top)
# ---------------------------------------------------------------------------

def _pixels_to_oled(bw_array: np.ndarray) -> bytes:
    """
    Convert H×W boolean array to OLED bytes, stored COLUMN-MAJOR.

    Storage order: col 0 (pages 0-7), col 1 (pages 0-7), ..., col 127 (pages 0-7).
    Each byte covers 8 vertical pixels; bit 0 = top pixel of that page.

    Column-major order lets Bayer dithering compress with RLE: all 8 pages in
    a column share the same Bayer column-threshold, so mid-tone areas produce
    runs of 8 identical bytes instead of 8 unique bytes (200% → 50% ratio).

    The firmware decompressor writes fb[page*128 + col] to rearrange back into
    the OLED's page-major framebuffer layout.
    """
    # build page bytes — shape (8 pages, 128 cols)
    page_bytes = np.zeros((8, W), dtype=np.uint8)
    for page in range(8):
        rows = bw_array[page*8 : page*8+8, :].astype(np.uint8)  # (8, 128)
        for bit in range(8):
            page_bytes[page] |= (rows[bit] << bit)

    # emit column-major: for each col, all 8 page bytes in order
    buf = bytearray(BYTES_PER_FRAME)
    for col in range(W):
        buf[col*8 : col*8+8] = page_bytes[:, col].tobytes()
    return bytes(buf)


def image_to_oled_bytes(path: str, mode: str = "bayer", gamma: float = 1.0,
                        contrast: float = 1.0, black_point: float = 0.0) -> bytes:
    img = Image.open(path).convert("L")

    if gamma != 1.0:
        lut = [min(255, int((v / 255.0) ** gamma * 255)) for v in range(256)]
        img = img.point(lut)

    if contrast != 1.0:
        img = ImageEnhance.Contrast(img).enhance(contrast)

    if mode == "floyd":
        bw = img.convert("1", dither=Image.Dither.FLOYDSTEINBERG)
        arr = np.array(bw, dtype=bool)

    elif mode == "bayer":
        gray = np.array(img, dtype=np.float32) / 255.0
        tile = np.tile(_BAYER_4, (H // 4 + 1, W // 4 + 1))[:H, :W]
        # Shift Bayer thresholds up by black_point: pixels below black_point
        # never exceed any threshold → solid black, zero dithering noise.
        # Only the [black_point, 1.0] range is dithered.
        adjusted = black_point + tile * (1.0 - black_point)
        arr = gray > adjusted

    else:  # threshold
        arr = np.array(img, dtype=np.uint8) >= 128

    return _pixels_to_oled(arr)


# ---------------------------------------------------------------------------
# RLE: (count, byte) pairs, count 1-255
# ---------------------------------------------------------------------------

def rle_compress(data: bytes) -> bytes:
    out = bytearray()
    i = 0
    while i < len(data):
        val = data[i]
        run = 1
        while i + run < len(data) and data[i + run] == val and run < 255:
            run += 1
        out.append(run)
        out.append(val)
        i += run
    return bytes(out)


# ---------------------------------------------------------------------------
# C emitter
# ---------------------------------------------------------------------------

def emit_c(frames_data: list, fps: int, out_path: str) -> None:
    offsets = []
    lengths = []
    offset = 0
    for cdata in frames_data:
        offsets.append(offset)
        lengths.append(len(cdata))
        offset += len(cdata)

    n = len(frames_data)

    with open(out_path, "w") as f:
        f.write("/* AUTO-GENERATED — do not edit. Re-run tools/video_to_frames.py */\n")
        f.write('#include <stdint.h>\n')
        f.write('#include "oled.h"\n\n')

        f.write(f"/* {n} frames @ {fps} fps, RLE (count,byte) pairs */\n")
        f.write("static const uint8_t frame_data[] = {\n")
        for i, cdata in enumerate(frames_data):
            f.write(f"    /* [{i}] {len(cdata)}B */ ")
            f.write(", ".join(f"0x{b:02X}" for b in cdata))
            f.write(",\n")
        f.write("};\n\n")

        f.write(f"static const uint16_t frame_offsets[{n}] = {{\n    ")
        f.write(", ".join(str(o) for o in offsets))
        f.write("\n};\n\n")

        f.write(f"static const uint16_t frame_lengths[{n}] = {{\n    ")
        f.write(", ".join(str(l) for l in lengths))
        f.write("\n};\n\n")

        f.write(f"const uint32_t DOOM_FRAME_COUNT = {n}U;\n")
        f.write(f"const uint32_t DOOM_FPS        = {fps}U;\n\n")

        f.write("void DoomFrames_Blit(uint32_t idx)\n")
        f.write("{\n")
        f.write("    const uint8_t *src = frame_data + frame_offsets[idx];\n")
        f.write("    const uint8_t *end = src + frame_lengths[idx];\n")
        f.write("    uint8_t *fb = OLED_Framebuffer();\n")
        f.write("    /* Data is column-major: col 0 pages 0-7, col 1 pages 0-7, ... */\n")
        f.write("    /* Rearrange into page-major fb layout: fb[page*128 + col].     */\n")
        f.write("    uint8_t col = 0u, page = 0u;\n")
        f.write("    while (src < end) {\n")
        f.write("        uint8_t count = *src++;\n")
        f.write("        uint8_t value = *src++;\n")
        f.write("        do {\n")
        f.write("            fb[(uint16_t)page * 128u + col] = value;\n")
        f.write("            if (++page == 8u) { page = 0u; ++col; }\n")
        f.write("        } while (--count);\n")
        f.write("    }\n")
        f.write("}\n")


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("video",                                        help="Input video file")
    ap.add_argument("--fps",      type=int,   default=10,           help="Frames per second (default 10)")
    ap.add_argument("--budget",   type=int,   default=48,           help="Flash budget in KB (default 48)")
    ap.add_argument("--start",    type=float, default=0.0,          help="Start offset in seconds")
    ap.add_argument("--duration", type=float, default=None,         help="Duration to extract in seconds")
    ap.add_argument("--mode",     choices=["threshold","bayer","floyd"], default="bayer",
                                                                    help="1bpp conversion mode (default: bayer)")
    ap.add_argument("--gamma",    type=float, default=0.5,          help="Gamma (<1 = brighten, default 0.5)")
    ap.add_argument("--contrast",     type=float, default=1.0,  help="Contrast (>1 = crush darks to black, default 1.0)")
    ap.add_argument("--black-point",  type=float, default=0.0,  help="Bayer black point 0.0-1.0: pixels below this are solid black, no dithering (default 0.0)")
    ap.add_argument("--out",      default="src/doom_frames.c",      help="Output C file path")
    args = ap.parse_args()

    budget_bytes = args.budget * 1024

    with tempfile.TemporaryDirectory() as tmpdir:
        print(f"Extracting {args.fps}fps | mode={args.mode} | gamma={args.gamma}")
        paths = extract_frames(args.video, args.fps, args.start, args.duration, tmpdir)
        print(f"  {len(paths)} raw frames")

        compressed = []
        total = 0

        for i, path in enumerate(paths):
            raw = image_to_oled_bytes(path, mode=args.mode, gamma=args.gamma,
                                       contrast=args.contrast, black_point=args.black_point)
            c = rle_compress(raw)
            if total + len(c) > budget_bytes:
                print(f"  Budget hit at frame {i} ({total} bytes). Stopping.")
                break
            compressed.append(c)
            total += len(c)
            if i % 20 == 0:
                print(f"  frame {i:4d}: rle={len(c):4d}B  ({len(c)*100//len(raw)}%)")

        n = len(compressed)
        avg = total // n if n else 0
        secs = n / args.fps
        print(f"\n{n} frames | {total}/{budget_bytes} bytes ({total*100//budget_bytes}%) | "
              f"avg {avg}B/frame | {secs:.1f}s loop")

        Path(args.out).parent.mkdir(parents=True, exist_ok=True)
        emit_c(compressed, args.fps, args.out)
        print(f"Written: {args.out}")


if __name__ == "__main__":
    main()
