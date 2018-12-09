all:
	gcc tiny_shell.c -o tshell

fork:	tiny_shell.c
	gcc -D FORK tiny_shell.c -o tshell

vfork:	tiny_shell.c
	gcc -D VFORK tiny_shell.c -o tshell

clone:	tiny_shell.c
	gcc -D CLONE tiny_shell.c -o tshell

pipe:	tiny_shell.c
	gcc -D PIPE tiny_shell.c -o tshell

clean:
	rm tshell
	
