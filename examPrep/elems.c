/*
Escriba un programa en C para linux que admita como argumentos nombres de
variables de entorno. El programa debe escribir por su salida, uno por línea,
cada elemento que contienen las variables de entorno especificadas. Se considera
que los elementos de una variable de entorno están separados por el carácter ':'
(como en el caso de la variable de entorno PATH). Si alguna de las variables
especificadas no existe, el programa debe terminar con fallo.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>


int
incorrect_args(int argc){
    return argc < 2;
}

void
print_vars(int argc, char *argv[]){
    int i;
    char* var;
    char *token;
    char *saveptr;
    for(i=0; i < argc; i++){
        
        var = getenv(argv[i]);
        if (var == NULL){
            errx(EXIT_FAILURE, "ERROR: var %s does not exist", argv[i]);
        }
        token = strtok_r(var, ":", &saveptr);
        while ( token != NULL) {
            printf("%s\n", token);
            token = strtok_r(saveptr, ":", &saveptr);
        }
    }
}

int
main(int argc, char* argv[]){
    if (incorrect_args(argc)){
        errx(EXIT_FAILURE, "usage: elems env [env]");
    }
    
    argc--;
    argv++;
    
    print_vars(argc, argv);
    exit(EXIT_SUCCESS);
}