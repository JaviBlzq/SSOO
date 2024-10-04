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
    has_x = value[1] == 'x' || value[1] == 'X';
    if (has_0 && (correct_len && has_x)) {
        base = 16;  
    }
    return base;
}

void
print_numbers(int argc, char *argv[])
{
	int first_value = FIRST_VALUE;
	int step = INITIAL_STEP;
	int last_value;
	int i;
    int base;
	char *ptr;

	if (argc == 2) {
        base = get_base(argv[1]);
		last_value = strtol(argv[1], &ptr, base);
		check_pointer(ptr, argv[1]);
	} else if (argc == 3) {
        base = get_base(argv[1]);
		first_value = strtol(argv[1], &ptr, base);
		check_pointer(ptr, argv[1]);

        base = get_base(argv[2]);
		last_value = strtol(argv[2], &ptr, base);
		check_pointer(ptr, argv[2]);
	} else if (argc == 4) {
        base = get_base(argv[1]);
		first_value = strtol(argv[1], &ptr, base);
		check_pointer(ptr, argv[1]);

        base = get_base(argv[2]);
		step = strtol(argv[2], &ptr, base);
		check_pointer(ptr, argv[2]);

        base = get_base(argv[3]);
		last_value = strtol(argv[3], &ptr, base);
		check_pointer(ptr, argv[3]);
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
