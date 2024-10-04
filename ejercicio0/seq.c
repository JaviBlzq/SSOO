#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
	FIRST_VALUE = 1,
	INITIAL_STEP = 1,
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
check_pointer(char *ptr, char *value)
{
	if (*ptr != '\0') {
		fprintf(stderr,
			"seq: argumento de coma flotante no valido «%s»",
			value);
		exit(EXIT_FAILURE);
	}
}

int
get_base(char *value)
{
	int base = 10;
	int has_0;
	int has_x;
	int correct_len;

	if (value == NULL) {
		fprintf(stderr, "Valor nulo");
	}
	has_0 = value[0] == '0';
	correct_len = strlen(value) >= 2;
	if (correct_len) {
		has_x = value[1] == 'x' || value[1] == 'X';
		if (has_0 && (correct_len && has_x)) {
			base = 16;
		}
	}

	return base;
}

int
string_to_integer(char *value)
{
	int base;
	char *ptr;
	int last_value;

	base = get_base(value);
	last_value = strtol(value, &ptr, base);
	check_pointer(ptr, value);

	return last_value;
}

void
parse_args(int argc, char *argv[], int *arr)
{
	int first_value = FIRST_VALUE;
	int step = INITIAL_STEP;
	int last_value;

	if (argc == 2) {
		last_value = string_to_integer(argv[1]);

	} else if (argc == 3) {
		first_value = string_to_integer(argv[1]);
		last_value = string_to_integer(argv[2]);

	} else if (argc == 4) {
		first_value = string_to_integer(argv[1]);
		step = string_to_integer(argv[2]);
		last_value = string_to_integer(argv[3]);

	}
	arr[0] = first_value;
	arr[1] = step;
	arr[2] = last_value;

}

void
print_numbers(int *values)
{
	int first_value = values[0];
	int step = values[1];
	int last_value = values[2];
	int i;

	if (correct_values(first_value, last_value)) {
		for (i = first_value; i <= last_value; i += step) {
			printf("%d\n", i);
		}
	}
}

int
main(int argc, char *argv[])
{
	int arr[3];

	check_args(argc);
	parse_args(argc, argv, arr);
	print_numbers(arr);
	exit(EXIT_SUCCESS);
}
