#
# makefile for TINY
# Borland C Version
# K. Louden 2/3/98
#
# modified at Oct. 29, 2022 in SJTU
#

CC = gcc

CFLAGS =

oS = main.o util.o scanner.o parser.o

main.o: ../main.c ../globals.h ../util.h ../scan.h ../parse.h
	$(CC) $(CFLAGS) -c ../main.c

scanner.o: ../scanner.c ../scan.h ../util.h ../globals.h
	$(CC) $(CFLAGS) -c ../scanner.c

util.o: ../util.c ../util.h ../globals.h
	$(CC) $(CFLAGS) -c ../util.c

parser.o: ../parser.c ../parse.h ../scan.h ../globals.h ../util.h
	$(CC) $(CFLAGS) -c ../parser.c

clean:
	-rm main.o
	-rm util.o
	-rm scanner.o
	-rm parser.o

rmelf:
	-rm bin/parser

parser: $(oS)
	$(CC) $(CFLAGS) -o bin/parser $(oS)

testp:
	-bin/parser test/sample.tny

