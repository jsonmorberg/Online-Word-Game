#include <stdio.h>
#include <poll.h>
#include <unistd.h>

int main() 
{ 
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI }; 
    int a;
    if( poll(&mypoll, 1, 5000) )    { 
        scanf("%d", &a);  
        printf("Input =  %d\n", a);  
    } else { 
        printf("Timed Out");  
    }  
    return 0; 
}