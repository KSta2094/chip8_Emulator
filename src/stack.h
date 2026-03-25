#include <stddef.h>

typedef unsigned char BYTE;
typedef unsigned short int WORD;

typedef struct {
    WORD *stack;
    size_t size;
    size_t max;
} Stack;

void init(Stack *stack);
void push(Stack *stack, WORD val);
WORD pop(Stack *stack);
