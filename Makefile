all: 
	gcc -std=c99 CompletoComFimDoJogo.c -lintelfpgaup -o executavel
	sudo ./executavel
