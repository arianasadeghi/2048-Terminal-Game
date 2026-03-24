joc_2048: 2048.c
	gcc - Wall -g -o joc_2048 2048.c -lncurses

# Regula pentru a rula programul
run: joc_2048
	./joc_2048


