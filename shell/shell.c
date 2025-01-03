#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

enum {
    Stdin = 0,
    Maxlinelen = 255,
    MaxOutputlinelen = 1024,
    Maxpath = 255,
    Success = 1,
    Failure = 0,
};

int
correct_args(int argc)
{
    return argc == 1;
}

char *
get_token(char* str_to_tokenize, char* delim, char **saveptr)
{    
    return strtok_r(str_to_tokenize, delim, saveptr);
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

void
change_dir(char *dir){
    if (chdir(dir) != 0){
        warn("cd error");
    }
}

int
do_cd(char* line){
    char* cmd;
    char* dir;
    char *saveptr = NULL;
    char *home;
    cmd = get_token(line, " ", &saveptr);
    if (cmd != NULL){
        if (strcmp(cmd, "cd") == 0){
            dir = get_token(NULL, " ", &saveptr);
            if (dir != NULL) {
                change_dir(dir);
                return Success;
            } else {
                if ((home = getenv("HOME") ) != NULL){
                    change_dir(home);
                    return Success;
                } else {
                    fprintf(stderr, "home not founded");
                    return Failure;
                }
            }
        }
    }
    return Failure;
}

int
create_variables(char *line)
{
    char *saveptr = NULL;
    char *name;
    char *value;
    int overwrite = 1;
    if (strchr(line, '=')){
        name = get_token(line, "=", &saveptr);
        value = get_token(NULL, "=", &saveptr);
        if (name != NULL && value != NULL){
            if (setenv(name, value, overwrite) == 0){
                return Success;
            } else {
                warn("setenv");
            }
        }
    }
    return Failure;

}

int 
do_builtins(char* line){
    char line_copy[Maxlinelen];
    strncpy(line_copy, line, sizeof(line_copy));
    line_copy[sizeof(line_copy) - 1] = '\0';
    if (do_cd(line_copy) == Success || create_variables(line_copy) == Success){
        return Success;
    }
    return Failure;
}

void
printline(void)
{
    char cwd[Maxpath];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s", cwd);
    } else {
        warn("getcwd() error"); 
    }
}

int
run_executable_cwd()
{
    return Failure;
}

void
run_command(char* line){
    fprintf(stderr, "Linea %s\n", line);
    if (run_executable_cwd(line) == Failure){
        
    }
    
    
}

void
sustitute_varenv(char*line, char* output_line, ssize_t output_size)
{
    char line_copy[Maxlinelen];
    char *token, *saveptr;
    char *env_value;
    size_t remaining_size = output_size - 1;

    strncpy(line_copy, line, sizeof(line_copy));
    line_copy[sizeof(line_copy) - 1] = '\0';

    output_line[0] = '\0';

    token = strtok_r(line_copy, " ", &saveptr);
    while (token != NULL) {
        if (token[0] == '$') {
            env_value = getenv(++token);
            if (env_value) {
                if (strlen(env_value) + strlen(output_line) + 1 > remaining_size) {
                    warn("Output buffer overflow");
                    return;
                }
                strncat(output_line, env_value, remaining_size);
                remaining_size -= strlen(env_value);
            } else {
                if (strlen(token) + strlen(output_line) + 1 > remaining_size) {
                    warn("Output buffer overflow");
                    return;
                }
                strncat(output_line, token, remaining_size);
                remaining_size -= strlen(token);
            }
        } else {
            if (strlen(token) + strlen(output_line) + 1 > remaining_size) {
                warn("Output buffer overflow");
                return;
            }
            strncat(output_line, token, remaining_size);
            remaining_size -= strlen(token);
        }

        token = strtok_r(NULL, " ", &saveptr);
        if (token && strlen(output_line) + 1 < remaining_size) {
            strncat(output_line, " ", remaining_size);
            remaining_size -= 1;
        }
    }
}

void
run_shell(void)
{
    char line[Maxlinelen];
    char* full_line = (char*)malloc(MaxOutputlinelen * sizeof(char));
    while (1){
        memset(line, 0, Maxlinelen);
        memset(full_line, 0, Maxlinelen);
        printline();
        printf("$ ");
        fgets(line, Maxlinelen, stdin);
        treat_line(line);
        if (do_builtins(line) != Success){
            sustitute_varenv(line, full_line, MaxOutputlinelen);
            run_command(full_line);
        }
        /*if (is_builtin(line)){
            do builtin
            printf("is builtin");
        } else {
            printf("hello");
        }*/
    }
    free(full_line);
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