#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    if (argc != 2){
        fprintf(stderr, "Only one argument is allowed\n");
        exit(EXIT_FAILURE);
    } else {
        int num = atoi(argv[1]);
        if (num % 2 == 0){
            printf("%d is even\n", num);
        } else {
            printf("%d is odd\n", num);
        }

    } 
    exit(EXIT_SUCCESS);
}