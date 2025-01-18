#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum {
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
get_token(char *str_to_tokenize, const char *delim, char **saveptr)
{
	char *token;

	token = strtok_r(str_to_tokenize, delim, saveptr);

	while (token && *token == '\0') {
		token = strtok_r(NULL, delim, saveptr);
	}

	return token;
}

void
treat_line(char *line)
{
	if (line == NULL) {
		errx(EXIT_FAILURE, "line not valid");
	}

	size_t len = strlen(line);

	if (len > 0 && line[len - 1] == '\n') {
		line[len - 1] = '\0';
	}
}

void
change_dir(char *dir)
{
	if (chdir(dir) != 0) {
		warn("cd error");
	}
}

int
do_cd(char *line)
{
	char *cmd;
	char *dir;
	char *saveptr = NULL;
	char *home;

	cmd = get_token(line, " \t", &saveptr);
	if (cmd != NULL) {
		if (strcmp(cmd, "cd") == 0) {
			dir = get_token(NULL, " \t", &saveptr);
			if (dir != NULL) {
				change_dir(dir);
				return Success;
			} else {
				if ((home = getenv("HOME")) != NULL) {
					change_dir(home);
					return Success;
				} else {
					fprintf(stderr, "home not found\n");
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

	if (strchr(line, '=')) {
		name = get_token(line, "=", &saveptr);
		value = get_token(NULL, "=", &saveptr);
		if (name != NULL && value != NULL) {
			if (setenv(name, value, overwrite) == 0) {
				return Success;
			} else {
				warn("setenv");
			}
		}
	}
	return Failure;
}

int
do_builtins(char *line)
{
	char line_copy[Maxlinelen];

	strncpy(line_copy, line, sizeof(line_copy));
	line_copy[sizeof(line_copy) - 1] = '\0';

	if (do_cd(line_copy) == Success
	    || create_variables(line_copy) == Success) {
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
get_args(const char *line, char argbuf[Maxargs][Maxlinelen],
	 char *args[Maxargs + 1])
{
	char line_copy[Maxlinelen];
	char *token;
	char *saveptr;
	int i = 0;

	strncpy(line_copy, line, sizeof(line_copy) - 1);
	line_copy[sizeof(line_copy) - 1] = '\0';

	token = get_token(line_copy, " \t", &saveptr);
	while (token != NULL && i < Maxargs) {
		strncpy(argbuf[i], token, Maxlinelen - 1);
		argbuf[i][Maxlinelen - 1] = '\0';
		args[i] = argbuf[i];
		i++;
		token = get_token(NULL, " \t", &saveptr);
	}
	args[i] = NULL;
}

void
create_path(const char *filename, char *path)
{
	strcpy(path, "./");
	strcat(path, filename);
}

int
run_executable_cwd(char *line, int is_waitable)
{
	char argbuf[Maxargs][Maxlinelen];
	char *args[Maxargs + 1];
	char path[Maxlinelen + 3];
	int status;
	int pid;

	get_args(line, argbuf, args);

	pid = fork();
	switch (pid) {
	case -1:
		warn("fork");
		return Failure;
	case 0:
		memset(path, 0, sizeof(path));
		create_path(args[0], path);
		execv(path, args);
		exit(Childfailure);
	default:
		if (is_waitable) {
			wait(&status);
		}
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
			return Success;
		} else if (WEXITSTATUS(status) == Childfailure) {
			return Failure;
		}
	}
	return Success;
}

int
check_waitable(char *line)
{
	int len_line = strlen(line);

	if (len_line > 0 && line[len_line - 1] == '&') {
		line[len_line - 1] = '\0';
		return 0;
	}
	return 1;
}

int
do_child(char *args[Maxargs + 1])
{
	char *path_env = getenv("PATH");

	if (path_env == NULL) {
		fprintf(stderr, "error: PATH is not defined\n");
		return Failure;
	}

	char path_copy[MaxOutputlinelen];

	strncpy(path_copy, path_env, sizeof(path_copy) - 1);
	path_copy[sizeof(path_copy) - 1] = '\0';

	char *dir;
	char *saveptr = NULL;

	dir = get_token(path_copy, ":", &saveptr);

	while (dir != NULL) {
		char full_path[Maxlinelen * 2];

		snprintf(full_path, sizeof(full_path), "%s/%s", dir, args[0]);

		execv(full_path, args);
		dir = get_token(NULL, ":", &saveptr);
	}
	return Failure;
}

int
run_exe_path(char *line, int is_waitable)
{
	char argbuf[Maxargs][Maxlinelen];
	char *args[Maxargs + 1];
	int status;
	int pid;

	get_args(line, argbuf, args);

	pid = fork();
	switch (pid) {
	case -1:
		warn("fork");
		return Failure;
	case 0:
		if (do_child(args) == Failure) {
			exit(Childfailure);
		}
		exit(Childfailure);	
	default:
		if (is_waitable) {
			waitpid(pid, &status, 0);
		}
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
			return Success;
		} else if (WEXITSTATUS(status) == Childfailure) {
			
			return Failure;
		}
	}
	return Success;
}

int
parse_redirection(char *line, char *command, char *input_file,
		  char *output_file)
{
	char *token;
	char *saveptr;
	int input_redirect = 0;
	int output_redirect = 0;
	size_t len;

	command[0] = '\0';
	input_file[0] = '\0';
	output_file[0] = '\0';

	token = get_token(line, " \t", &saveptr);
	while (token != NULL) {
		if (strcmp(token, ">") == 0) {
			output_redirect = 1;
			token = get_token(NULL, " \t", &saveptr);
			if (token != NULL) {
				strncpy(output_file, token, Maxpath - 1);
				output_file[Maxpath - 1] = '\0';
			} else {
				fprintf(stderr,
					"error: missing output file after '>'\n");
				return Failure;
			}
		} else if (strcmp(token, "<") == 0) {
			input_redirect = 1;
			token = get_token(NULL, " \t", &saveptr);
			if (token != NULL) {
				strncpy(input_file, token, Maxpath - 1);
				input_file[Maxpath - 1] = '\0';
			} else {
				fprintf(stderr,
					"error: missing input file after '<'\n");
				return Failure;
			}
		} else {
			if (output_redirect || input_redirect) {
				fprintf(stderr,
					"error: invalid syntax after redirection operator\n");
				return Failure;
			}
			strncat(command, token,
				Maxlinelen - 1 - strlen(command));
			strncat(command, " ", Maxlinelen - 1 - strlen(command));
		}
		token = get_token(NULL, " \t", &saveptr);
	}

	len = strlen(command);
	if (len > 0 && command[len - 1] == ' ') {
		command[len - 1] = '\0';
	}

	return Success;
}

int
safe_dup2(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) == -1) {
		warn("dup2");
		return Failure;
	}
	return Success;
}

int
setup_redirection(const char *input_file, const char *output_file,
		  int is_waitable)
{
	int fd;

	if (input_file[0] != '\0') {
		fd = open(input_file, O_RDONLY);
		if (fd < 0) {
			warn("open input file");
			return Failure;
		}
		if (safe_dup2(fd, STDIN_FILENO) == Failure) {
			close(fd);
			return Failure;
		}
		close(fd);
	} else {
		if (!is_waitable) {
			fd = open("/dev/null", O_RDONLY);
			if (fd < 0) {
				warn("open /dev/null");
				return Failure;
			}
			if (safe_dup2(fd, STDIN_FILENO) == Failure) {
				close(fd);
				return Failure;
			}
			close(fd);
		}
	}

	if (output_file[0] != '\0') {
		int flags = O_WRONLY | O_CREAT | O_TRUNC;

		fd = open(output_file, flags, 0644);
		if (fd < 0) {
			err(EXIT_FAILURE, "open output file");
			return Failure;
		}
		if (safe_dup2(fd, STDOUT_FILENO) == Failure) {
			close(fd);
			return Failure;
		}
		close(fd);
	}

	return Success;
}

int
run_command(char *line)
{
	int status;
	int is_waitable = check_waitable(line);

	char command[Maxlinelen] = "";
	char input_file[Maxpath] = "";
	char output_file[Maxpath] = "";

	if (parse_redirection(line, command, input_file, output_file) ==
	    Failure) {
		return Failure;
	}

	int pid = fork();

	if (pid == -1) {
		warn("fork");
		return Failure;
	}

	if (pid == 0) {
		if (setup_redirection(input_file, output_file, is_waitable) ==
		    Failure) {
			exit(Childfailure);
		}

		if (run_executable_cwd(command, is_waitable) == Failure) {
			if (run_exe_path(command, is_waitable) == Failure) {
				exit(Childfailure);
			}
		}
		exit(Success);
	}

	waitpid(pid, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != Success) {
		fprintf(stderr, "error: command failed\n");
	}
	return Success;
}

int
sustitute_varenv(char *line, char *output_line, ssize_t output_size)
{
	char line_copy[Maxlinelen];
	char *token, *saveptr;
	char *env_value;
	size_t remaining_size = output_size - 1;

	strncpy(line_copy, line, sizeof(line_copy));
	line_copy[sizeof(line_copy) - 1] = '\0';

	output_line[0] = '\0';

	token = get_token(line_copy, " \t", &saveptr);
	while (token != NULL) {
		if (token[0] == '$') {
			env_value = getenv(++token);
			if (env_value != NULL) {
				if (strlen(env_value) + strlen(output_line) +
				    1 > remaining_size) {
					warn("Output buffer overflow");
					return Failure;
				}
				strncat(output_line, env_value, remaining_size);
				remaining_size -= strlen(env_value);
			} else {
				fprintf(stderr,
					"error: var %s does not exist.\n",
					token);
				output_line[0] = '\0';
				return Failure;
			}
		} else {
			if (strlen(token) + strlen(output_line) + 1 >
			    remaining_size) {
				warn("Output buffer overflow");
				return Failure;
			}
			strncat(output_line, token, remaining_size);
			remaining_size -= strlen(token);
		}

		token = get_token(NULL, " \t", &saveptr);
		if (token && strlen(output_line) + 1 < remaining_size) {
			strncat(output_line, " ", remaining_size);
			remaining_size -= 1;
		}
	}
	return Success;
}

void
run_shell(void)
{
	char line[Maxlinelen];

	char full_line[MaxOutputlinelen];
	int is_interactive = isatty(STDIN_FILENO);

	while (1) {
		if (is_interactive) {
			printline();
		}

		if (fgets(line, Maxlinelen, stdin) == NULL) {
			if (feof(stdin)) {
				break;
			} else {
				warn("Error reading from stdin");
			}
		}

		treat_line(line);

		memset(full_line, 0, sizeof(full_line));

		if (do_builtins(line) != Success) {
			if (sustitute_varenv(line, full_line, MaxOutputlinelen)
			    == Success) {
				run_command(full_line);
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	if (!correct_args(argc)) {
		errx(EXIT_FAILURE, "usage: shell");
	}
	run_shell();
	exit(EXIT_SUCCESS);
}
