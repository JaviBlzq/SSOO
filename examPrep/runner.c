/*
Le pasas dos comandos y te dice cual ha acabado primero
*/
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

enum {
    Maxlength = 255,
    Execerror = 200,
};

int
incorrect_args(int argc)
{
    return (argc < 2) || (argc > 3);
}

int
check_errors(int status)
{
    if (status == -1){
        err(EXIT_FAILURE, "wait:");
    } else if (WIFEXITED(status)){
        switch (WEXITSTATUS(status)){
            case 0:
                return 0;
            case Execerror:
                errx(EXIT_FAILURE, "exec failure");
            default:
                errx(EXIT_FAILURE, "something went really bad");

        }
    } else {
        errx(EXIT_FAILURE, "childs went wrong");
    }
}

void
print_winner(int winner, int* pids, char* argv[]){

    if (pids[0] == winner){  
        printf("The winner is the process with pid %d, command %s\n", winner, argv[0]);
    } else {
        printf("The winner is the process with pid %d, command %s\n", winner, argv[1]);
    }
}

int
wait_childs()
{
    int pid_winner;
    int status;
    pid_winner = wait(&status);
    check_errors(status);
    wait(&status);
    check_errors(status);
    return pid_winner;
}
char*
create_path(char* cmd, char *dst){
    char* path = "/usr/bin/";
    
    strcpy(dst, path);
    strcat(dst, cmd);
    return dst;
}

int
exec_cmd(int argc, char* argv[], int *pids)
{
    int i;
    char* cmd;
    int pid;
    char* dst = malloc(Maxlength * 2);
    if (dst == NULL){
        err(EXIT_FAILURE, "malloc failed");
    }

    for (i = 0; i < argc; i++){
        cmd = create_path(argv[i], dst);
        pid = fork();
        switch (pid){
            case -1:
                err(EXIT_FAILURE, "fork failed");
            case 0:
                pids[i] = pid;
                execl(cmd, argv[i], (char *)0);
                errx(Execerror, "not able to find command");
            default:
                pid = wait_childs();
            
        }
    }
    free(cmd);
    return pid;
}


int
main(int argc, char* argv[])
{
    int pid_winner;
    int* pids  = malloc(argc*Maxlength);
    if (pids == NULL){
        err(EXIT_FAILURE, "malloc failed");
    }
    if (incorrect_args(argc)) {
        errx(EXIT_FAILURE, "usage: runner cmd cmd");
    }
    argc--;
    argv++;
    pid_winner = exec_cmd(argc, argv, pids);
    print_winner(pid_winner, pids, argv);
    free(pids);
    exit(EXIT_SUCCESS);
}