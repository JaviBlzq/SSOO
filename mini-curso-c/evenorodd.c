#include <stdlib.h>
#include <stdio.h>

/*Given a number, return if the number is even or odd*/

int
main(int argc, char *argv[])
{
    int num;
    if (argc != 2){
        fprintf(stderr, "Only one argument is allowed\n");
        exit(EXIT_FAILURE);
    } else {
        num = atoi(argv[1]);
        if (num % 2 == 0){
            printf("%d is even\n", num);
        } else {
            printf("%d is odd\n", num);
        }

    } 
    exit(EXIT_SUCCESS);
}