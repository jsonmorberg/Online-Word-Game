#include <stdio.h>
#include <poll.h>
#include <unistd.h>

int main() 
{ 
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI }; 
    char a[1000];
    if(poll(&mypoll, 1, 1000) )    { 
        scanf("%s", &a);  
        printf("Input =  %s\n", a);  
    } else { 
        printf("Timed Out");  
    }  
    return 0; 
}