
#include "stack.h"
#include <stddef.h>
#include <stdlib.h>

void push(Stack *stack, WORD val) {

    if (stack->size + 1 > stack->max) {
        stack->stack = realloc(stack->stack, stack->max + 10);
        stack->max += 10;
    }
    stack->stack[stack->size + 1] = val;
    stack->size += 1;
}
void init(Stack *stack) {
    stack->max = 10;
    stack->size = 0;
    stack->stack = malloc(stack->max);
}

WORD pop(Stack *stack) {
    if (stack->size - 1 < 0) {
        return -1;
    } else {

        stack->size -= 1;
        return stack->stack[stack->size + 1];
    }
}
