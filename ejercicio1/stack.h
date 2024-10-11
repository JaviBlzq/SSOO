enum {
    MAX_SIZE = 31 + 1, /*31 del límite más '\0'*/
};

typedef struct Node Node;
struct Node {
    char str[MAX_SIZE];
    Node *next;
};

typedef struct Stack Stack;
struct Stack
{
    Node *tos; /*Top Of Stack*/
};

Stack* newstack(void);
void push(char *str, Stack *s);
char* pop(Stack *s);
void destroystack(Stack *s);