; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
