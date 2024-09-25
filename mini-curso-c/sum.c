#include <stdlib.h>
#include <stdio.h>


/*Given 2 numbers, return the sum of the two numbers*/

int 
sum_two_nums(int num1, int num2)
{
    return num1 + num2;
}

int
main(int argc, char **argv)
{
    int num1;
    int num2;
    int sum;

    if (argc != 3)  /*The argv always contains the program name*/
    {
        fprintf(stderr, "Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        num1 = atoi(argv[1]);
        num2 = atoi(argv[2]);
        sum = sum_two_nums(num1, num2);
        printf("The sum of %d and %d is %d\n", num1, num2, sum);
    }
}        