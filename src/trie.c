#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "trie.h"

#define char_size 26

struct Trie *createTrieNode(){
	struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
	node->isWord = 0;

	for(int i=0; i<char_size; i++){
		node->children[i] = NULL;
	}
	return node;
}

void insert(struct Trie *head, char *word){
	struct Trie* curr = head;
	for(int i=0; i<strlen(word); i++){
		char ch = word[i];
		if(curr->children[ch - 'a'] == NULL){
			curr->children[ch - 'a'] = createTrieNode();
		}
		curr = curr->children[ch - 'a'];
	}
	curr->isWord = 1;
}

int search(struct Trie *head, char *word){
	struct Trie* curr = head;
	for(int i=0; i<strlen(word); i++){
		char ch = word[i];
		if(curr->children[ch - 'a'] == NULL){
			return 0;
		}
		curr = curr->children[ch-'a'];
	}
	return curr->isWord;
}

