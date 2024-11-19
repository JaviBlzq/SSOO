#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

enum {
    MaxNamesize = 255,
    Maxrepetitions = 1000,
    MaxReadSize = 255,
};

int
valid_argc(int argc)
{
    return argc == 3; 
}

char *
get_path(char *argv[])
{
    return argv[0];
}

int
get_number_repetitions(char *argv[])
{
    int rep;
    rep = atoi(argv[1]);
    if (rep <= 0){
        errx(EXIT_FAILURE, "usage: nrep file NUM. NUM must be greater than 0");
    } else if (rep > Maxrepetitions){
        errx(EXIT_FAILURE, "repetitions must be fewer than %d", Maxrepetitions);
    }
    return rep;
}

char*
get_name(char* path){
    char *name;
    name = strrchr(path, '/');
    if (name == NULL){
        return path;
    } else if( strcmp(name, "/") == 0){
        errx(EXIT_FAILURE, "provide a file");
    }
    name++;
    return name;
}

void
create_name(char* name, char* new_name, int i)
{
    snprintf(new_name, MaxNamesize, "%s.%d",name, i);
}

int
open_file(char* file_name){
    
    int fd;
    fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666); /*read-write*/
    if (fd == -1){
        err(EXIT_FAILURE, "error open file. ");
    }
    return fd;
    
}

void
write_file(int fd, char*txt, int rep, int size_readed){
    int j;
    int size_written;
    for (j=1; j <= rep; j++){
        
        size_written = write(fd, txt, size_readed);
        if (size_written < size_readed){
            err(EXIT_FAILURE, "can not write");
        }
    }
}

void
write_copies(int rep, char* txt, int size_readed, int* fds)
{
    int i;
    for (i=0; i<rep;i++){
        /*write*/
        write_file(fds[i], txt, i+1, size_readed); 
    }
}
    


void
open_fds(char *name, int* fds, int rep)
{
    int i;
    int fd;
    char new_name[MaxNamesize + 1];
    for (i=0; i<rep; i++){
        new_name[0] = '\0';
        create_name(name, new_name, i+1);
        
        fd = open_file(new_name);
        fds[i] = fd;
    }
}

void
close_all(int* fds, int rep){
    int i;
    for (i=0; i<rep; i++){
        close(fds[i]);
    }

}

void
do_copies(char* path, int rep)
{
    char* name = get_name(path);
    int *all_fds = malloc(sizeof(int) * rep);
    int fd;
    int size_readed;
    
    char txt[MaxReadSize+ 1];

    fd = open(path, O_RDONLY);
    if (fd == -1){
        err(EXIT_FAILURE, "file doesnt exist");
    }
    
    open_fds(name, all_fds, rep);
    
    while (1){
        memset(txt, 0, sizeof(txt));
        size_readed = read(fd, txt, MaxReadSize);
        if (size_readed == -1){
            err(EXIT_FAILURE, "reader failed:");
        } else if (size_readed == 0){
            break;
        }
        write_copies(rep, txt, size_readed, all_fds);
    }


    if (close(fd) == -1){
        err(EXIT_FAILURE, "close failed:");
    }
    close_all(all_fds, rep);
    free(all_fds);

}

int
main(int argc, char* argv[])
{
    char *path;
    int rep;
    
    

    if (!valid_argc(argc)){
        errx(EXIT_FAILURE, "usage: nrep FILE num");
    }
    argv++;
    argc--;
    path = get_path(argv);
    
    rep = get_number_repetitions(argv);
    do_copies(path, rep);

    exit(EXIT_SUCCESS);
}