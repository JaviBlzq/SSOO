#include <stdlib.h>
#include <stdio.h>

enum {
    FIRST_VALUE = 1,
    STEP = 1,
};

void
check_args(int argc){
    if(argc < 2){
        fprintf(stderr, "seq: Falta un operando");
        exit(EXIT_FAILURE);
    } else if (argc > 4) {
        fprintf(stderr, "seq: Operando extra");
        exit(EXIT_FAILURE);
    }
}

void
print_numbers(int argc, char* argv[]){
    int first_value = FIRST_VALUE;
    int step = STEP;
    int last_value;
    if (argc == 2){
        last_value = strtol(argv[1], NULL, 0);
    } else if (argc == 3){
        first_value = strtol(argv[1], NULL, 0);
        last_value = strtol(argv[2], NULL, 0);
    } else if (argc == 4){
        first_value = strtol(argv[1], NULL, 0);
        step = strtol(argv[1], NULL, 0);
        last_value = strtol(argv[1], NULL, 0);
    }
}


int
main(int argc, char *argv[]){
    check_args(argc);
    int end_value = strtol(argv[1], NULL, 0); /*Detecta automáticamente la base*/
    printf("%d", end_value);
    exit(EXIT_SUCCESS);
}