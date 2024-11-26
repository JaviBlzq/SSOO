    #include <stdint.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <err.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <string.h>
    #include <unistd.h>

    enum {
        MaxBuffSize = 1024,
    };

    int
    correct_args(int argc){
        return argc == 1;
    }

    char*
    get_path(void)
    {
        char* path;
        path = getenv("PWD");
        if (path == NULL){
            errx(EXIT_FAILURE, "pwd env. variable not avalaible");
        }
        return path;
    }

    int
    is_regfile(char* d_name){
        struct stat statbuff;
        if (lstat(d_name, &statbuff) == -1) {
            err(EXIT_FAILURE, "problems with lstat");
        }
        return ((statbuff.st_mode & S_IFMT) == S_IFREG);
    }

    int
    is_txt(char* name){
        char* extension = ".txt";
        
        size_t name_len = strlen(name);
        size_t extension_len = strlen(extension);
        char* extension_of_name;
        
        if (name_len < extension_len) {
            return 0; // No puede terminar en .txt si es más corta
        }

        extension_of_name = name + name_len - extension_len;

        return strcmp(extension_of_name, extension) == 0;
    }

    FILE*
    open_file(char *name){
        FILE* f;
        f = fopen(name, "r");
        if (f == NULL) {
            err(EXIT_FAILURE, "file doesnt exist");
        }
        return f;
    }


    void
    print_lastlines(char *name){
        int count = 0;
        FILE* file = open_file(name);
        char buffer[MaxBuffSize + 1];
        memset(buffer, 0, sizeof(buffer));
        while (fgets(buffer, MaxBuffSize, file) != NULL) {
            count++;
        }
        if (count > 0){
            printf("%s:%d:%s\n", name, count, buffer);
        } else if (count == 0){
            printf("%s:%d\n", name, count + 1);
        }
        fclose(file);
    }

    void
    do_lastlines(char *path)
    {
        DIR* pwd;
        struct dirent *dir;
        pwd = opendir(path);
        if (pwd == NULL){
            err(EXIT_FAILURE, "issues opening dir ");
        }
        
        errno = 0;
        while ((dir = readdir(pwd)) != NULL){
            if (is_txt(dir->d_name) && is_regfile(dir->d_name)){
                    print_lastlines(dir->d_name);
            }


        }
        if ((dir == NULL) && (errno != 0)){
            err(EXIT_FAILURE, "problems reading dir");
        }
        closedir(pwd); /*NO COMPROBAMOS EL ERROR*/
    }

    int
    main(int argc, char* argv[])
    {
        char* path;
        if (!correct_args(argc)){
            errx(EXIT_FAILURE, "usage: lastlines ");
        }
        path = get_path();
        do_lastlines(path);
        exit(EXIT_SUCCESS);
    }