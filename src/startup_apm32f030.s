.syntax unified
.cpu cortex-m0
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler

.extern _estack
.extern _etext
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word Default_Handler
    .word Default_Handler

    .rept 32
    .word Default_Handler
    .endr

.size g_pfnVectors, . - g_pfnVectors

.section .text.Reset_Handler, "ax", %progbits
.thumb_func
.type Reset_Handler, %function

Reset_Handler:
    ldr r0, =_etext
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:
    cmp r1, r2
    bcc copy_data_loop
    b zero_bss

copy_data_loop:
    ldr r3, [r0]
    str r3, [r1]
    adds r0, r0, #4
    adds r1, r1, #4
    b copy_data

zero_bss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

zero_bss_loop:
    cmp r0, r1
    bcc zero_bss_store
    b call_main

zero_bss_store:
    str r2, [r0]
    adds r0, r0, #4
    b zero_bss_loop

call_main:
    bl SystemInit
    bl main
    b .

.size Reset_Handler, . - Reset_Handler

.section .text.Default_Handler, "ax", %progbits
.thumb_func
.type Default_Handler, %function

Default_Handler:
    b .

.size Default_Handler, . - Default_Handler