advcalc:	main.o
		gcc main.o -o advcalc

main.o:		main.c
		gcc -c main.c 
