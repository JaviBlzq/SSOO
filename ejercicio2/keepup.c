#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

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

void
exec_file(int argc, char *argv[])
{
	char *path;
	int status;
	pid_t pid;

	int tries = 0;
	int exists = 0;

	path = argv[0];

	get_correct_args(argc, argv);

	while (!exists) {
		pid = fork();
		switch (pid) {
		case -1:
			fprintf(stderr, "fork failed!");
			exit(EXIT_FAILURE);
		case 0:
			execv(path, argv);
			fprintf(stderr, "keepup: cannot execute %s\n", path);

		default:
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid");
			}

			if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
				exists = 1;
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
	exec_file(argc, argv);
	exit(EXIT_SUCCESS);
}
