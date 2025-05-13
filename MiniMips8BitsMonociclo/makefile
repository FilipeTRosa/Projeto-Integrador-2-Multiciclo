compila:
		gcc minimips.c controle.c decodificador.c memoria.c multiplexadores.c step.c -c -g
		gcc -g main.c minimips.o controle.o decodificador.o memoria.o multiplexadores.o step.o -o exec -lm
		./exec
clear:
		rm minimips.o
		rm exec