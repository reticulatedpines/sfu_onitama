onitama: onitama.c logging.c deck.h gamestate.h logging.h
	gcc -Wall -Wextra -O3 -pie -o onitama onitama.c logging.c

clean: onitama
	rm onitama
