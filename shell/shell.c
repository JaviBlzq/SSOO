#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

enum {
    Stdin = 0,
    Maxlinelen = 256,
};

int
correct_args(int argc)
{
    return argc == 1;
}

void
treat_line(char* line)
{
    if (line == NULL){
        errx(EXIT_FAILURE, "line not valid");
    }
    line[strlen(line)-1]='\0';
    /*update line with value of env value*/
}

char *
get_token(char* str_to_tokenize, char* delim)
{
    char *saveptr;
    char *token;
    token = strtok_r(str_to_tokenize, delim, &saveptr);
    
    return token;
}

int 
is_builtin(char* line){
    char* cmd;
    cmd = get_token(line, " ");
    if (cmd != NULL){
        cmd++;
        fprintf(stderr, "%s\n", cmd);
    }
    return 1;
}

void
run_shell(void)
{
    char line[Maxlinelen];
    while (1){
        memset(line, 0, Maxlinelen);
        printf("$ ");
        fgets(line, Maxlinelen, stdin);
        treat_line(line);
        if (is_builtin(line)){
            /*do builtin*/
            printf("is builtin");
        } else {
            printf("hello");
        }
    }
}

int
main(int argc, char* argv[])
{
    if (!correct_args(argc)){
        errx(EXIT_FAILURE, "usage: shell");
    }
    run_shell();
    exit(EXIT_SUCCESS);
}