#include <stdlib.h>
#include <stdio.h>

enum {
	FIRST_VALUE = 1,
	STEP = 1,
};

void
check_args(int argc)
{
	if (argc < 2) {
		fprintf(stderr, "seq: Falta un operando");
		exit(EXIT_FAILURE);
	} else if (argc > 4) {
		fprintf(stderr, "seq: Operando extra");
		exit(EXIT_FAILURE);
	}
}

int
correct_values(int first_value, int last_value)
{
	return first_value <= last_value;
}

void
print_numbers(int argc, char *argv[])
{
	int first_value = FIRST_VALUE;
	int step = STEP;
	int last_value;
	int i;

	if (argc == 2) {
		last_value = strtol(argv[1], NULL, 0);

	} else if (argc == 3) {
		first_value = strtol(argv[1], NULL, 0);
		last_value = strtol(argv[2], NULL, 0);
	} else if (argc == 4) {
		first_value = strtol(argv[1], NULL, 0);
		step = strtol(argv[2], NULL, 0);
		last_value = strtol(argv[3], NULL, 0);
	}
	if (correct_values(first_value, last_value)) {
		for (i = first_value; i <= last_value; i += step) {
			printf("%d\n", i);
		}
	}
}

int
main(int argc, char *argv[])
{
	check_args(argc);
	print_numbers(argc, argv);
	exit(EXIT_SUCCESS);
}
