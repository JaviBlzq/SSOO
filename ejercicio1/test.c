#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

void static
push_combinations(Stack *s)
{
	char first;
	char second;
	char str[3];

	for (first = 'a'; first <= 'z'; first++) {
		for (second = 'a'; second <= 'z'; second++) {
			str[0] = first;
			str[1] = second;
			str[2] = '\0';

			push(str, s);
		}
	}
}

void static
print_stack(Stack *s)
{
	char *popped;

	while (s->tos != NULL) {
		popped = pop(s);
		if (popped != NULL) {
			printf("%s\n", popped);
		}

		free(popped);
	}
}

int
main()
{
	Stack *s;

	s = newstack();
	push_combinations(s);
	print_stack(s);
	destroystack(s);

	exit(EXIT_SUCCESS);
}
