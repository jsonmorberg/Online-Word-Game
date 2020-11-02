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


int validWord(char* word, char* board){
	char newBoard[sizeof(board)];
	size_t destination_size = sizeof(board);

	strncpy(newBoard, board, destination_size);
	newBoard[destination_size] = '\0';

    if(!trieSearch(dictionary, word)){
		return 1;
	}

	if(trieSearch(usedWords, word)){
		return 1;
	}

	for(int i = 0; i < strlen(word); i++){
		char wordChar = word[i];
		int flag = 0;

		for(int j = 0; j < strlen(newBoard); j++){
            if(wordChar == newBoard[j]){
                flag = 1;
                newBoard[j] = ' ';
                printf("%s\n", newBoard);
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
    uint16_t port;
    uint8_t boardSize;
    uint8_t seconds;
    char* dictionaryPath;


    if( argc != 5 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"TODO\n");
		exit(EXIT_FAILURE);
    }

    dictionary = trieCreate();

    FILE* filePointer;
    int bufferLength = 1000;
    char buffer[bufferLength];

    filePointer = fopen("twl06.txt", "r");

    while(fgets(buffer, bufferLength, filePointer)) {
        trieInsert(dictionary, buffer);
    }

    fclose(filePointer);    

    port = atoi(argv[1]);
    //PORT STUFF

    boardSize = atoi(argv[2]);
    seconds = atoi(argv[3]);
    dictionaryPath = argv[4];

    struct Player player1;
    struct Player player2;

    int playerOneScore = 0;
    int playerTwoScore = 0;

    player1.playerType = '1';
    player1.boardSize = boardSize;
    player1.seconds = seconds;

    player2.playerType = '2';
    player2.boardSize = boardSize;
    player2.seconds = seconds;

    int round = 1;
    //while(playerOneScore != 3 && playerTwoScore != 3){
    
       player1.playerOneScore = playerOneScore;
       player1.round = round;

       player2.playerTwoScore = playerTwoScore;
       player2.round = round;

       char* board = generateBoard(boardSize); 
       board[strlen(board)] = '\0'; 
       printf(board);

       char charbuf[1000];
       scanf("%s", charbuf);
       
       printf("%d\n", validWord(charbuf, board));



    //}


    
}