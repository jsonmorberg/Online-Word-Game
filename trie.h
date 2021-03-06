//https://www.geeksforgeeks.org/trie-insert-and-search/
//TODO change bool types to int 
#include <stdbool.h>
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (100)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

typedef struct node 
{ 
    struct node *children[ALPHABET_SIZE]; 
  
    // isEndOfWord is true if the node represents 
    // end of a word 
    bool isEndOfWord; 
}trieNode ; 
  

trieNode *trieCreate(void);  // Returns a pointer to an empty trie
int trieInsert(trieNode *root, const char *key);
int trieSearch(trieNode *root, const char *key);


