

typedef struct node {

    // TODO: Define fields of the node

} trieNode;

// TODO: Implement the following functions in trie.c
trieNode *trieCreate(void);  // Returns a pointer to an empty trie
int trieInsert(trieNode *trie, char *word);  // Returns 1 on success, 0 on failure
int trieSearch(trieNode *trie, char *word);  // Returns 1 if word exists in trie, otherwise returns 0

/*
 * In addition to the three functions above, you may need/want to define and implement additional functions.
 * Define them below.
 */
