; Copyright (c) 2011, Johan Kotlinski
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.

.import tick
.importzp sp
.export _startirq

zpspace = 26 ; This is dangerous, in case it changes with new cc65 versions...

_startirq:
    sei
    lda #>irq1
    sta $0315
    lda #<irq1
    sta $0314

	; Disable CIA interrupts.
    lda #$7f
    sta $dc0d

	; Clear MSbit of VIC raster register.
	and $d011
	sta $d011

	; Enable raster interrupt from VIC.
    lda #$01
    sta $d01a

	; Set raster pos for IRQ.
    lda #$a0
    sta $d012
	cli
	rts

handling_irq:
	.byte 0

irq1:
	lda handling_irq
	bne overload
	inc handling_irq

	inc	$d020

	; save zp
	ldx     #zpspace-1
L1: lda sp,x
	sta zpsave,x    ; Save the zero page locations we need
	dex
	bpl L1

	jsr tick

	; restore zp
	ldx #zpspace-1
L2: lda zpsave,x
	sta sp,x
	dex
	bpl L2
	
	dec $d020
	dec handling_irq
ret:
	asl $d019 ; Acknowledge interrupt by clearing VIC interrupt flag.
	jmp $ea31 ; Jump to standard kernel IRQ service.
overload:
	inc	$d020
	jmp ret

zpsave:
	.res zpspace
