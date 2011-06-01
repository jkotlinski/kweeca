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

PHRASE_COUNT = $80
PHRASE_NOTE_OFFSET = $4000
PHRASE_INSTR_OFFSET = PHRASE_NOTE_OFFSET + 16 * PHRASE_COUNT
PHRASE_CMD_OFFSET = PHRASE_NOTE_OFFSET + 16 * PHRASE_COUNT * 2
PHRASE_VAL_OFFSET = PHRASE_NOTE_OFFSET + 16 * PHRASE_COUNT * 3

CHAIN_COUNT = $80
CHAIN_PHRASE_OFFSET = $6000
CHAIN_TSP_OFFSET = CHAIN_PHRASE_OFFSET + 16 * CHAIN_COUNT

