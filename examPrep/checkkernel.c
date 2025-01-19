#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <string.h>

int
correct_args(int argc)
{
    return argc == 2;
}

void
get_arg(char* argv[])
{
    fprintf(stderr, "%s\n", argv[0]);
}

int
get_kernel(char *argv[])
{
    int pid;
    int pipefd[2];
    int nr = 1024;
    int read_bytes;
    char version[1024];
    char buf[1024];
    memset(version, 0, 1024);
    if (pipe(pipefd) == -1){
        err(EXIT_FAILURE, "pipe failed");
    }
    pid = fork();
    switch (pid)
    {
    case -1:
        err(EXIT_FAILURE, "fork failed");
    
    case 0:
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execl("/bin/uname","uname", "-r", NULL);
    default:
        close(pipefd[1]);
        while ((read_bytes = read(pipefd[0], buf, nr)) != 0){
            if (read_bytes == -1){
                err(EXIT_FAILURE, "read failed");
            }
            buf[read_bytes] = '\0';
            strncat(version, buf, read_bytes);
        }
    }
    version[strcspn(version, "\n")] = '\0';
    if (strcmp(version, argv[0]) == 0){
        close(pipefd[0]);
        return EXIT_SUCCESS;
    }
    fprintf(stderr, "wrong kernel\n");
    return EXIT_FAILURE;
}

int
main(int argc, char* argv[])
{
    if (!correct_args(argc)){
        exit(EXIT_FAILURE);
    }
    argv++;
    exit(get_kernel(argv));
    

}