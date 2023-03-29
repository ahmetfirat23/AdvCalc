advcalc.exe:	main.o
			gcc main.o -o advcalc.exe

advcalc.o:		main.c
			gcc -c main.c 