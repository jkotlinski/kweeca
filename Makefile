CC65BASE = /usr/local/lib/cc65

CRT0   	= $(CC65BASE)/lib/c64.o
CLIB	= $(CC65BASE)/lib/c64.lib
CC	= cc65 -O --create-dep -Cl -tc64 -T -I $(CC65BASE)/include/ 
AS	= ca65 --cpu 6502x # -l
LD	= ld65 -C kweeca.cfg -m kweeca.map -Ln kweeca.lbl 
C1541  	= ~/bin/c1541
DEPDIR = .dep


all:   	kweeca

# --------------------------------------------------------------------------
# Generic rules

%.o : %.c

%.a : %.c
	@echo $<
	@$(CC) -o $(basename $<).a $(basename $<).c
	@mkdir -p $(DEPDIR)
	@mv $(basename $<).u $(DEPDIR)/

%.o : %.a
	@$(AS) $(basename $<).a

# Don't delete intermediate .a files.
.PRECIOUS : %.a 

%.o : %.s
	@echo $<
	@$(AS) $(basename $<).s

OBJS = kweeca.o phrase.o player.o cursor.o print.o global.o irq.o alloc.o file.o mem.o chain.o song.o screens.o status.o player_c.o kbd.o sprites.o instr.o math.o

-include $(OBJS:%.o=$(DEPDIR)/%.u)


# --------------------------------------------------------------------------
# Rules how to make each one of the binaries

EXELIST=kweeca

kweeca: 		$(OBJS) $(CLIB)
	@$(LD) -o $@.prg $^

run: kweeca
	x64 kweeca.prg

# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
	rm -f *~ *.map *.o *.lbl *.prg *.lst *.a *.u .dep/*
	
