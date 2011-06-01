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

.export _player_init
.export _player_stop
.export _g_ticks
.export _g_playing
; .export _play_phrase
.export _PHRASE_INSTR_PTR_MSB
.export _PHRASE_INSTR_PTR_LSB
.export _PHRASE_NOTE_PTR_MSB
.export _PHRASE_NOTE_PTR_LSB
.export _PHRASE_CMD_PTR_MSB
.export _PHRASE_CMD_PTR_LSB
.export _PHRASE_ARG_PTR_MSB
.export _PHRASE_ARG_PTR_LSB
.export _PCC
.export _PCC7
.export _g_sid_src
.export _PLAYMODE
.export _CH_ACTIVE
.export _groove
.export tick

.importzp sp
.import incsp2
.import pusha
.import _advance_step
.import _CHAIN_TRANSPOSE
.import _apply_new_instr
.import _g_filter_mod

PLAYMODE_SONG = 0
PLAYMODE_CHAIN = 1
PLAYMODE_PHRASE = 2

CMD_DELAY = 0
CMD_FILTER_MOD = 1
CMD_GROOVE = 2

.macro ldb dst, val
	lda #val
	sta dst
.endmacro
.macro ldw dst, val
	lda #<(val)
	sta dst
	lda #>(val)
	sta dst + 1
.endmacro

.include "mem.inc"
.include "freqtable.inc"

_g_ticks:
	.byte 0
_g_playing:
	.byte 0

_PCC:
	.byte 0
_PCC7:
	.byte 0

_PLAYMODE:
	.byte 0
_CH_ACTIVE:
	.res 3
_PHRASE_INSTR_PTR_MSB:
	.res 3
_PHRASE_INSTR_PTR_LSB:
	.res 3
_PHRASE_NOTE_PTR_MSB:
	.res 3
_PHRASE_NOTE_PTR_LSB:
	.res 3
_PHRASE_CMD_PTR_MSB:
	.res 3
_PHRASE_CMD_PTR_LSB:
	.res 3
_PHRASE_ARG_PTR_MSB:
	.res 3
_PHRASE_ARG_PTR_LSB:
	.res 3
_ticks_to_next_step:
	.res 3
gate_off_timer:
	.res 3
active_delay:
	.res 3
_groove:
	.byte 6, 6 ; ch 1
	.byte 6, 6 ; ch 2
	.byte 6, 6 ; ch 3
groove_pos:
	.res 3 ; one per channel, contains 0 or 1

_g_sid_src:
	.res (3 * 7)
INSTR_AD = _g_sid_src
INSTR_SR = _g_sid_src + 1
INSTR_H = _g_sid_src + 2
INSTR_WF = _g_sid_src + 3
INSTR_PW = _g_sid_src + 4

; ----------------

; voice
SID_FREQ_W = $d400
SID_PWLO = $d402
SID_PWHI = $d402
SID_WF = $d404
SID_AD = $d405
SID_SR = $d406
; general
SID_FLT_W = $d415
SID_RC = $d417
SID_AMP = $d418

_player_init:
	; init global regs
	ldb SID_RC, $f1
	ldw SID_FLT_W, $6000

	lda	#0
	sta _CHAIN_TRANSPOSE
	sta _CHAIN_TRANSPOSE + 1
	sta _CHAIN_TRANSPOSE + 2

	; max volume
	lda	#$1f
	sta	$d418

	rts

_player_stop:
	lda #0
	sta _g_playing
	sta _CH_ACTIVE
	sta _CH_ACTIVE + 1
	sta _CH_ACTIVE + 2
	sta _ticks_to_next_step
	sta _ticks_to_next_step + 1
	sta _ticks_to_next_step + 2
	sta groove_pos
	sta groove_pos + 1
	sta groove_pos + 2
	sta SID_AD
	sta SID_AD + 7
	sta SID_AD + 14
	sta SID_SR
	sta SID_SR + 7
	sta SID_SR + 14
	rts

; out: a = note, x = PCC
; wastes x
get_note:
	ldx _PCC
	lda _PHRASE_NOTE_PTR_LSB, x
	sta	@ld + 1
	lda _PHRASE_NOTE_PTR_MSB, x
	sta	@ld + 2
@ld:
	lda $ffff ; overwritten
	rts

; out: a = cmd, x = PCC
; wastes x
get_cmd:
	ldx _PCC
	lda _PHRASE_CMD_PTR_LSB, x
	sta	@ld + 1
	lda _PHRASE_CMD_PTR_MSB, x
	sta	@ld + 2
@ld:
	lda $ffff ; overwritten
	rts

; out: a = arg, x = PCC
; wastes x
get_arg:
	ldx _PCC
	lda _PHRASE_ARG_PTR_LSB, x
	sta	@ld + 1
	lda _PHRASE_ARG_PTR_MSB, x
	sta	@ld + 2
@ld:
	lda $ffff ; overwritten
	rts

; out: a = note
; wastes y
get_instr:
	ldy _PCC
	lda _PHRASE_INSTR_PTR_LSB, y
	sta	@ld + 1
	lda _PHRASE_INSTR_PTR_MSB, y
	sta	@ld + 2
@ld:
	lda $ffff ; overwritten
	rts

; in: a = note
; wastes a, x, y
set_freq:
	tax
	ldy _PCC7
	lda	freq_table_msb, x
	sta SID_FREQ_W + 1, y
	lda	freq_table_lsb, x
	sta SID_FREQ_W, y
	rts

queued_freq_msb:
	.res 3
queued_freq_lsb:
	.res 3
queued_freq_timer:
	.res 3
queued_adsr_timer:
	.res 3

handle_hard_restarts:
	ldx _PCC
	lda queued_freq_timer, x
	beq	@exit

	dec queued_freq_timer, x
	bne @check_queued_adsr

	; Apply queued frequency.
	ldy _PCC7
	lda	queued_freq_msb, x
	sta SID_FREQ_W + 1, y
	lda	queued_freq_lsb, x
	sta SID_FREQ_W, y
	lda INSTR_H, y
	sta gate_off_timer, x

	; Open gate. 
	lda	INSTR_WF, y
	ora	#1
	sta SID_WF, y
	rts

@check_queued_adsr: ; x = PCC
	lda	queued_adsr_timer, x
	beq	@exit
	dec queued_adsr_timer, x
	bne @exit

	; Apply adsr. x = pcc
	ldy _PCC7
	lda	INSTR_AD, y
	sta	SID_AD, y
	lda INSTR_SR, y
	sta SID_SR, y
	lda INSTR_PW, y
	sta SID_PWLO, y
	sta SID_PWHI, y
	; Set test bit.
	lda	9 ; Ok?
	sta SID_WF, y
@exit:
	rts

; in: x = note
; wastes a, y
set_note_freq:
	ldy _PCC7
	ldx	s_new_note
	lda	freq_table_msb, x
	sta SID_FREQ_W + 1, y
	lda	freq_table_lsb, x
	sta SID_FREQ_W, y
	rts

; in: a = phrase
; wastes a, x, y
play_step:
	jsr get_note
	cmp #$ff
	beq @ret

	; add chain transpose
	sta	@note + 1
	lda	_CHAIN_TRANSPOSE, x
	clc
@note:
	adc	#0 ; overwritten

	sta s_new_note
	jsr	get_instr
	cmp	#$ff
	beq set_note_freq ; No instr on row = tie note = only update pitch.

	jsr	pusha
	jsr	_apply_new_instr

	; Play new note x!
	; Enqueue frequency.

	lda #4 ; Four ticks (including this) until we want the freq to be applied.
	ldy _PCC
	sta queued_freq_timer, y

	ldx	s_new_note
	lda	freq_table_msb, x
	sta queued_freq_msb, y
	lda	freq_table_lsb, x
	sta queued_freq_lsb, y

	; Three ticks (including this) until we want the ADSR to be applied.
	lda #3 
	sta queued_adsr_timer, y
	
	; Clear ADSR.
	ldy _PCC7
	lda #$f
	sta SID_AD, y
	lda	#0
	sta SID_SR, y

	; Clear gate bit.
	lda	INSTR_WF, y
	sta SID_WF, y
@ret:
	rts

s_new_note:
	.byte 0
	
; TICKS_PER_STEP = 6

handle_gate_offs:
	ldx	_PCC
	ldy	gate_off_timer, x
	beq	@exit
	dec gate_off_timer, x
	bne	@exit
	; Clear gate bit.
	ldy	_PCC7
    lda INSTR_WF, y
	sta	SID_WF, y
@exit:
	rts

handle_filter:
	jsr	get_arg
	sta	_g_filter_mod, x
	rts

handle_pre_fx:
	jsr	get_cmd
	cmp #CMD_FILTER_MOD
	beq	handle_filter
	rts

; output in a
get_groove_ticks:
	lax	_PCC
	asl a
	tay ; y = pcc * 2
	lda	groove_pos, x
	beq	@first_step
	iny ; second step
@first_step:
	eor	#1
	sta groove_pos, x
	lda _groove, y
	rts

get_new_groove:
	jsr	get_arg
	tay ; y = packed groove
	txa ; x = PCC from get_arg
	asl a
	tax ; x = PCC * 2
	tya
	lsr a
	lsr a
	lsr a
	lsr a
	sta _groove, x

	; second part
	tya
	and #$f
	inx
	sta _groove, x
	rts

tick_channel:
	ldx _PCC
	lda	_CH_ACTIVE, x
	beq	@ret

	dec _ticks_to_next_step, x
	bpl @ret

	; ======= tick stuff START. involves D command
	jsr get_cmd
	cmp	#CMD_GROOVE
	bne	@not_groove
	jsr get_new_groove
@not_groove:

	ldx	_PCC
	lda	active_delay, x
	beq @no_active_delay
	; Compensate for delay.
	jsr	get_groove_ticks
	sec
	sbc	active_delay, x
	tay
	dey ; Compensate for stealing this tick already.
	tya
	sta	_ticks_to_next_step, x
	lda #0
	sta	active_delay, x
	jmp	@play_step

@no_active_delay:
	jsr	get_groove_ticks
	sta _ticks_to_next_step, x

	; Handle sequencing commands (e.g. delay).
	jsr	get_cmd
	cmp	#CMD_DELAY
	bne	@play_step
	jsr	get_arg
	beq	@play_step
	; Delay playing this step.
	sta	active_delay, x
	sta	_ticks_to_next_step, x
	jmp	@ret
	; ======= tick stuff STOP

@play_step:
	jsr	handle_pre_fx
	jsr play_step
	jsr _advance_step

@ret:
	jsr handle_hard_restarts
	jmp handle_gate_offs

tick:
	inc _g_ticks ; Only increased here.

	lda _g_playing
	beq	@exit

	ldx #0
	stx _PCC
	stx _PCC7
	jsr	tick_channel
	inc _PCC
	ldx #7
	stx _PCC7
	jsr	tick_channel
	inc _PCC
	ldx #14
	stx _PCC7
	jsr tick_channel

@exit:
	rts
