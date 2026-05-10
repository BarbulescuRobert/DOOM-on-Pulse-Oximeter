#!/usr/bin/env python3
import argparse
import csv
import os
import re
import sys
from collections import deque

def print_progress(current, total, width=40):
    if total <= 0:
        return
    ratio = min(max(current / total, 0.0), 1.0)
    filled = int(ratio * width)
    bar = "#" * filled + "-" * (width - filled)
    sys.stdout.write(f"\r[{bar}] {ratio * 100:6.2f}%")
    sys.stdout.flush()
    if current >= total:
        sys.stdout.write("\n")

def parse_hex_signature(text):
    tokens = re.findall(r"(?:0x)?[0-9A-Fa-f]{1,2}", text)
    if not tokens:
        raise ValueError("No hex bytes found in signature")
    return [int(t, 16) for t in tokens]

def norm_name(s):
    return re.sub(r"[^a-z0-9]", "", s.lower())

def find_column(fieldnames, requested):
    wanted = norm_name(requested)
    for name in fieldnames:
        if norm_name(name) == wanted:
            return name
    for name in fieldnames:
        if wanted in norm_name(name):
            return name
    raise SystemExit(f"Could not find column matching {requested!r}. Available: {fieldnames}")

def to_bit(value):
    value = value.strip()
    if value.lower() in ("1", "1.0", "true", "high"):
        return 1
    if value.lower() in ("0", "0.0", "false", "low"):
        return 0
    try:
        return 1 if float(value) >= 0.5 else 0
    except ValueError:
        return 0

def fmt_sig(sig):
    return " ".join(f"{b:02X}" for b in sig)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv_file")
    ap.add_argument("--signature", default="AE D5 80 A8 3F D3 00 40 A1 C8 DA 12 81 FF 8D 14 D9 F1 DB 40 A4 A6 2E AF")
    ap.add_argument("--time", default="Time")
    ap.add_argument("--mosi", default="MOSI")
    ap.add_argument("--sclk", default="SCLK")
    ap.add_argument("--dc", default="D/C")
    ap.add_argument("--edge", choices=["rising", "falling", "both"], default="falling")
    ap.add_argument("--dc-mode", choices=["all", "cmd", "data"], default="cmd")
    ap.add_argument("--invert-dc", action="store_true")
    ap.add_argument("--gap-us", type=float, default=1000.0)
    ap.add_argument("--max-matches", type=int, default=50)
    ap.add_argument("--dump-decoded", default=None)
    args = ap.parse_args()

    signature = parse_hex_signature(args.signature)
    sig_len = len(signature)
    gap_s = args.gap_us * 1e-6
    file_size = os.path.getsize(args.csv_file)

    with open(args.csv_file, newline="", errors="ignore") as f:
        reader = csv.DictReader(f)
        if not reader.fieldnames:
            raise SystemExit("CSV has no header row")

        time_col = find_column(reader.fieldnames, args.time)
        mosi_col = find_column(reader.fieldnames, args.mosi)
        sclk_col = find_column(reader.fieldnames, args.sclk)
        dc_col = find_column(reader.fieldnames, args.dc)

        print(f"Using columns: time={time_col}, MOSI={mosi_col}, SCLK={sclk_col}, D/C#={dc_col}")
        print(f"Searching for: {fmt_sig(signature)}")
        print(f"Sampling edge: {args.edge}, mode={args.dc_mode}, invert_dc={args.invert_dc}")

        dump_file = None
        dump_writer = None
        if args.dump_decoded:
            dump_file = open(args.dump_decoded, "w", newline="")
            dump_writer = csv.writer(dump_file)
            dump_writer.writerow(["byte_index", "time_s", "dc", "kind", "byte_hex", "byte_dec"])

        prev_sclk = None
        last_sample_time = None
        bit_count = 0
        current_byte = 0
        current_dc = 0
        current_start_time = 0.0

        byte_index = 0
        selected_index = 0
        total_decoded = 0
        total_selected = 0
        match_count = 0
        window = deque(maxlen=sig_len)
        row_count = 0

        def dc_to_kind(dc_bit):
            effective = dc_bit ^ (1 if args.invert_dc else 0)
            return "DATA" if effective else "CMD"

        def should_search(dc_bit):
            kind = dc_to_kind(dc_bit)
            return args.dc_mode == "all" or kind.upper() == args.dc_mode.upper()

        def finish_byte(t_s, dc_bit, byte_val):
            nonlocal byte_index, selected_index, total_decoded, total_selected, match_count
            kind = dc_to_kind(dc_bit)
            total_decoded += 1

            if dump_writer:
                dump_writer.writerow([byte_index, f"{t_s:.9f}", dc_bit, kind, f"0x{byte_val:02X}", byte_val])

            if should_search(dc_bit):
                total_selected += 1
                selected_index += 1
                window.append((selected_index, byte_index, t_s, dc_bit, kind, byte_val))

                if len(window) == sig_len and [x[5] for x in window] == signature:
                    match_count += 1
                    first = window[0]
                    last = window[-1]
                    if match_count <= args.max_matches:
                        print(
                            f"\nMatch #{match_count}: "
                            f"time {first[2]:.9f}s -> {last[2]:.9f}s, "
                            f"decoded byte index {first[1]} -> {last[1]}, "
                            f"selected index {first[0]} -> {last[0]}, "
                            f"kind={first[4]}"
                        )

            byte_index += 1

        for row in reader:
            row_count += 1
            if row_count % 50000 == 0:
                print_progress(f.tell(), file_size)

            try:
                t_s = float(row[time_col])
            except Exception:
                continue

            mosi = to_bit(row[mosi_col])
            sclk = to_bit(row[sclk_col])
            dc = to_bit(row[dc_col])

            if prev_sclk is None:
                prev_sclk = sclk
                continue

            rising = prev_sclk == 0 and sclk == 1
            falling = prev_sclk == 1 and sclk == 0
            prev_sclk = sclk

            sample = (
                (args.edge == "rising" and rising) or
                (args.edge == "falling" and falling) or
                (args.edge == "both" and (rising or falling))
            )

            if not sample:
                continue

            if last_sample_time is not None and (t_s - last_sample_time) > gap_s:
                bit_count = 0
                current_byte = 0

            last_sample_time = t_s

            if bit_count == 0:
                current_dc = dc
                current_start_time = t_s

            current_byte = ((current_byte << 1) | mosi) & 0xFF
            bit_count += 1

            if bit_count == 8:
                finish_byte(current_start_time, current_dc, current_byte)
                bit_count = 0
                current_byte = 0

        if dump_file:
            dump_file.close()

    print_progress(file_size, file_size)
    print(f"Decoded bytes:       {total_decoded}")
    print(f"Searched bytes:      {total_selected}")
    print(f"Total matches found: {match_count}")

    if match_count == 0:
        print("\nNo match found. Try:")
        print(f"  python3 {sys.argv[0]} {args.csv_file} --edge rising")
        print(f"  python3 {sys.argv[0]} {args.csv_file} --dc-mode all")
        print(f"  python3 {sys.argv[0]} {args.csv_file} --invert-dc")

if __name__ == "__main__":
    main()