#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

enum {
	VALUE_EXIT_ERROR = 200,

};

void
check_args(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: <path> [args]");
		exit(EXIT_FAILURE);
	}

}

void
check_path(char *argv[])
{
	if (argv[0][0] != '/') {
		fprintf(stderr, "usage: <absolute_path> [args]");
		exit(EXIT_FAILURE);
	}
}

void
get_correct_args(int argc, char *argv[])
{
	char *executable = strrchr(argv[0], '/');

	if (executable == NULL) {
		fprintf(stderr,
			"Error: No se pudo encontrar '/' en el path proporcionado.\n");
		exit(EXIT_FAILURE);
	}
	executable++;
	argv[0] = executable;
}

int
exec_file(int argc, char *argv[])
{
	char *path;
	int status;
	pid_t pid;

	int tries = 0;

	path = argv[0];
	get_correct_args(argc, argv);
	while (1) {
		pid = fork();
		switch (pid) {
		case -1:
			fprintf(stderr, "fork failed!");
			exit(EXIT_FAILURE);
		case 0:
			execv(path, argv);
			fprintf(stderr, "keepup: cannot execute %s\n", path);
			exit(VALUE_EXIT_ERROR);
		default:
			if (wait(&status) == -1) {
				perror("wait");
			}

			if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
				return 0;

			} else if (WEXITSTATUS(status) == VALUE_EXIT_ERROR) {
				return 1;
			} else {
				tries++;
				fprintf(stderr,
					"keepup: the command %s exited ntimes: %d\n",
					path, tries);
			}
		}
	}
}

int
main(int argc, char *argv[])
{

	check_args(argc, argv);
	argv++;
	argc--;
	check_path(argv);
	if (exec_file(argc, argv) == 1) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
