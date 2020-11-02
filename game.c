#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "trie.h"
#include <time.h>

trieNode *dictionary;

struct Player{
    char playerType;
    uint8_t boardSize;
    uint8_t seconds;

    uint8_t playerOneScore;
    uint8_t playerTwoScore;
    uint8_t round;
    char* board;
};


int validWord(trieNode * usedWords, char* word, char* board){

	char newBoard[sizeof(board)];
	size_t destination_size = sizeof(board);

	strncpy(newBoard, board, destination_size);
	newBoard[destination_size] = '\0';

    if(!trieSearch(dictionary, word)){
		return 0;
	}

	if(trieSearch(usedWords, word)){
		return 0;
	}

	for(int i = 0; i < strlen(word); i++){
		char wordChar = word[i];
		int flag = 0;

		for(int j = 0; j < strlen(newBoard); j++){
            if(wordChar == newBoard[j]){
                flag = 1;
                newBoard[j] = ' ';
                break;
            }
        }

        if(flag == 0){
            return 0;
        }
	}

    return 1;
}

//generate the board
char* generateBoard(int boardSize){

    srand(time(NULL));
    char abc[26]="abcdefghijklmnopqrstuvwxyz";
    char vowel[5]="aeiou";

    char *board;
    board = malloc(boardSize);

    for(int i = 0; i < boardSize; i++){
        board[i] = abc[rand() % (sizeof(abc) - 1)];
    }

    int vowelFlag = 0;
    for(int i = 0; i < boardSize-1; i++){
        if(board[i] == 'a' || board[i] == 'e'|| board[i] == 'i'|| board[i] == 'o'|| board[i] == 'u'){
            vowelFlag = 1;
        }
    }

    if(!vowelFlag){
        board[boardSize-1] = vowel[rand() % (sizeof(vowel) - 1)];
    }

    return board;
}

int main(int argc, char **argv) {

    dictionary = trieCreate();
    trieNode * usedWords = trieCreate();

    FILE* filePointer;
    int bufferLength = 1000;
    char buffer[bufferLength];

    filePointer = fopen("twl06.txt", "r");

    while(fgets(buffer, bufferLength, filePointer)) {
        trieInsert(dictionary, buffer);
    }

    fclose(filePointer);  

    char* board = generateBoard(5);
    printf("%s\n", board);
    for(;;){
    char charbuf[100];
    printf("Your turn, enter word: ");
    scanf("%s", charbuf);

        if(validWord(usedWords, charbuf, board)){
            printf("Valid Word!\n");
            trieInsert(usedWords, charbuf);
        }else{
            printf("Invalid Word!\n");
        }  
    }
}