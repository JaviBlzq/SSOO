#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
    Stdin = 0,
    Maxlinelen = 255,
    MaxOutputlinelen = 1024,
    Maxpath = 255,
    Success = 1,
    Failure = 0,
    Maxargs = 127,
    Childfailure = 200,
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
        printf("%s$ ", cwd);
    } else {
        warn("getcwd() error"); 
    }
}

void 
get_args(char* line, char** args) 
{
    char line_copy[Maxlinelen];
    char *token;
    char *saveptr;
    int i = 0;

    
    strncpy(line_copy, line, sizeof(line_copy) - 1);
    line_copy[sizeof(line_copy) - 1] = '\0'; 

    token = strtok_r(line_copy, " ", &saveptr);
    while (token != NULL && i < Maxargs) {
        args[i] = malloc(strlen(token) + 1); 
        if (args[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strcpy(args[i], token);         
        token = strtok_r(NULL, " ", &saveptr);
        i++;
    }
    args[i] = NULL;
}

void free_args(char** args) 
{
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
}

void 
create_path(const char* filename, char* path) {
    
    strcpy(path, "./");
    strcat(path, filename);
}

int
run_executable_cwd(char* line, int is_waitable)
{
    char** args = malloc((Maxargs + 1) * sizeof(char*));;
    char path[Maxlinelen + 3];
    int status;

    get_args(line, args);

    int pid = fork();
    switch (pid)
    {
        case -1:
            warn("fork");
            return Failure;
        case 0:
            memset(path, 0, Maxlinelen);
            create_path(args[0], path);
            execv(path, args);
            exit(Childfailure);
        default:
            if (is_waitable){
                wait(&status);
            }
            if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
				return Success;

			} else if (WEXITSTATUS(status) == Childfailure) {
				return Failure;
            }

    }
    
    free_args(args);
    free(args);
    return Success;
}

int
check_waitable(char* line)
{
    int len_line = strlen(line);
    if (len_line > 0){
        char last_char = line[len_line - 1];
        return (last_char != '&');
    }
    return Success;
}

int
run_exe_path()
{
    
}

void
run_command(char* line){
    int is_waitable = check_waitable(line);
    if (run_executable_cwd(line, is_waitable) == Failure){
        fprintf(stderr, "PROBANDO QUE NO ESTA EN .\n");
        run_exe_path(line);
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
            if (env_value != NULL) {
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
        
        fgets(line, Maxlinelen, stdin);
        treat_line(line);
        if (do_builtins(line) != Success){
            sustitute_varenv(line, full_line, MaxOutputlinelen);
            run_command(full_line);
        }
        
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