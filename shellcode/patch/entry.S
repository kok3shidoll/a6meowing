/*
 * pongoOS - https://checkra.in
 *
 * Copyright (C) 2019-2021 checkra1n team
 *
 * This file is part of pongoOS.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
.text

.pool
.set gUSBSerialNumber,                      0x10061F80
.set total_received,                        0x10061A7C
.set usb_create_string_descriptor,          0x3f007C55
.set usb_serial_number_string_descriptor,   0x100600D8
.set bcopy,                                 0x3f009ac0

.align 2
.globl start
.arm
.org    0x0
start:

.org    0x300
FLAGS:
.long 0x0

.arm
.org    0x400
start2:
bl  _check8_payload
blx _main_payload
pop {pc}

_check8_payload:
push {lr}

_ADD_PWND_STR:
ldr r0, =gUSBSerialNumber

_find_zero_loop:
add r0, r0, #1
ldrb r1, [r0]
cmp r1, #0
bne _find_zero_loop
mov r1, r0
adr r0, HAXX_STR
_write:
ldr r3, =bcopy
movs r2, #0x20
blx r3
ldr r0, =gUSBSerialNumber
ldr r1, =usb_create_string_descriptor
blx r1
ldr r1, =usb_serial_number_string_descriptor
strb r0, [r1]
mov r0, #0
ldr r1, =total_received
str r0, [r1]

pop {pc}

HAXX_STR:
.asciz " FLAG:0000 PWND:[meowing]"
