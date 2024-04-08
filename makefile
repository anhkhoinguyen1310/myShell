all: myShell

myShell: myShell.c wildcard.c
	gcc -o myShell myShell.c wildcard.c

clean:
	rm -f myShell
