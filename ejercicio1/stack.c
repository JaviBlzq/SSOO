#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

Stack *
newstack(void)
{
	Stack *stack = (Stack *) malloc(sizeof(Stack));

	if (!stack) {
		fprintf(stderr, "Error al crear la pila");
		exit(EXIT_FAILURE);
	}
	stack->tos = NULL;
	return stack;
}

void
push(char *str, Stack *s)
{
	Node *new_node = (Node *) malloc(sizeof(Node));

	if (!new_node) {
		fprintf(stderr, "Error al crear el nodo");
		exit(EXIT_FAILURE);
	}
	if (strlen(str) > MAX_SIZE) {
		fprintf(stderr, "String demasiado larga");
		exit(EXIT_FAILURE);
	}
	strncpy(new_node->str, str, MAX_SIZE - 1);
	new_node->str[MAX_SIZE] = '\0';
	new_node->next = s->tos;
	s->tos = new_node;
}

char *
pop(Stack *s)
{
	char *popped;
	Node *tos_node;

	if (s->tos == NULL) {
		return NULL;
	}
	tos_node = s->tos;
	popped = strdup(tos_node->str);
	if (popped == NULL) {
		fprintf(stderr, "No hay suficiente memoria");
		exit(EXIT_FAILURE);
	}
	s->tos = tos_node->next;
	free(tos_node);
	return popped;

}

void
destroystack(Stack *s)
{
	while (s->tos != NULL) {
		free(pop(s));
	}
	free(s);
}
