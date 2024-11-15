#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

enum {
	ValueError = 200,
	MallocError = 100,
	MaxSize = 2,
	MaxUserLength = 255,
};

int
correct_args(int argc)
{
	return argc >= 2;
}

int
wait_childs(pid_t *pids, int argc)
{
	int narg;
	int status;

	for (narg = 0; narg < argc; narg++) {
		if (waitpid(pids[narg], &status, 0) == -1) {
			perror("waitpid");
		} else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			return 1;
		}
	}
	return 0;
}

int
exec_id(int argc, char *argv[])
{
	int narg;
	int failed = 0;
	char *path = "/usr/bin/id";
	char **copy_argv;
	pid_t *pids = malloc(argc * sizeof(pid_t));

	if (pids == NULL) {
		fprintf(stderr, "malloc failed");
		return MallocError;
	}

	copy_argv = malloc(MaxSize * MaxUserLength * sizeof(char));

	if (copy_argv == NULL) {
		fprintf(stderr, "malloc failed");
		free(pids);
		return MallocError;
	}
	copy_argv[0] = "id";

	for (narg = 0; narg < argc; narg++) {
		copy_argv[1] = argv[narg];
		pids[narg] = fork();
		if (pids[narg] == -1) {
			err(EXIT_FAILURE, "fork:");
		} else if (pids[narg] == 0) {
			execv(path, copy_argv);
			free(copy_argv);
			free(pids);
			exit(ValueError);
		}
	}

	failed = wait_childs(pids, argc);

	free(copy_argv);
	free(pids);

	return failed;
}

int
main(int argc, char *argv[])
{
	int status;

	if (correct_args(argc) == 0) {
		fprintf(stderr, "usage: idall user [user]\n");
		exit(EXIT_FAILURE);
	}
	argc--;
	argv++;
	status = exec_id(argc, argv);
	if (status != 0) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);

}
