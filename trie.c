//SOURCE https://www.geeksforgeeks.org/trie-insert-and-search/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "trie.h"
 
// Returns new trie node (initialized to NULLs) 
trieNode *trieCreate(void) 
{ 
    trieNode *pNode = NULL; 
  
    pNode = (trieNode *)malloc(sizeof(trieNode)); 
  
    if (pNode) 
    { 
        int i; 
  
        pNode->isEndOfWord = false; 
  
        for (i = 0; i < ALPHABET_SIZE; i++) 
            pNode->children[i] = NULL; 
    } 
  
    return pNode; 
}

void trieInsert(trieNode *root, const char *key)
{
    int level;
    int length = strlen(key);
    int index;

    trieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index]){
            pCrawl->children[index] = trieCreate();
        }
        pCrawl = pCrawl->children[index];
    }

    // mark last node as leaf
    pCrawl->isEndOfWord = true;
}

// Returns true if key presents in trie, else false
int trieSearch(trieNode *root, const char *key)
{
    int level;
    int length = strlen(key);
    int index;
    trieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);

        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }

    return (pCrawl != NULL && pCrawl->isEndOfWord);
} 

