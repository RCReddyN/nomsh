shell: shell.o trie.o
	gcc shell.o trie.o -o shell
shell.o: src/shell.c
	gcc src/shell.c -c
trie.o: src/trie.c
	gcc src/trie.c -c
clean:
	rm -f *.o shell
