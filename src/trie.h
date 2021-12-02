#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define char_size 26
struct Trie{
	int isWord;
	struct Trie* children[char_size];
};

struct Trie *createTrieNode();

void insert(struct Trie *head, char *word);

int search(struct Trie *head, char *word);
