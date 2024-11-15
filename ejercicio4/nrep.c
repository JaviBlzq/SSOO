#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

enum {
    MaxNamesize = 255,
    Maxrepetitions = 1000,
    MaxFilesize = 10 * 1024,
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

void
write_copies(char *name, int rep, char* txt)
{
    int i;
    char new_name[MaxNamesize + 1];
    
    for (i=0; i<=rep; i++){
        new_name[0] = '\0';
        create_name(name, new_name, i);
        
        fprintf(stderr, "%s\n", new_name);
    }
    
}


void
do_copies(char* path, int rep)
{
    char* name = get_name(path);
    int fd;
    int size_readed;
    
    char txt[MaxReadSize];

    fd = open(path, O_RDONLY);
    if (fd == -1){
        err(EXIT_FAILURE, "file doesnt exist");
    }
    
    fprintf(stderr, "estamos aqui\n");
    size_readed = read(fd, txt, MaxReadSize);
    
    while (size_readed != 0){
        if (size_readed == -1){
            err(EXIT_FAILURE, "reader failed:");
        }
        write_copies(name, rep, txt);
        size_readed = read(fd, txt, MaxReadSize);
    }


    if (close(fd) == -1){
        err(EXIT_FAILURE, "close failed:");
    }

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