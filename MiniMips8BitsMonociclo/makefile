compila:
		gcc minimips.c controle.c decodificador.c memoria.c miltiplexadores.c step.c -c -g
		gcc -g main.c minimips.o controle.o decodificador.o memoria.o miltiplexadores.o step.o -o exec -lm
		./exec
clear:
		rm minimips.o
		rm exec