#include "stack.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// GLOBAl
BYTE GameRAM[0xFFF];
BYTE Registers[16];
WORD RegisterI;
WORD ProgramCounter = 0x200;
BYTE Screen[64][32] = {0};

// implament a stack
Stack stack;
short timer = 60;

WORD fetchOpcode() {
    WORD opcode = (GameRAM[ProgramCounter] << 8) | GameRAM[ProgramCounter + 1];
    ProgramCounter += 2;
    return opcode;
}

void flushScreen() {
    printf("\n");
    for (int y = 0; y < 32; y++) {
        printf("\n");
        for (int x = 0; x < 64; x++) {
            if (Screen[x][y] == 1) {
                printf("█");
            } else {
                printf(" ");
            }
        }
    }
}

void draw(WORD opcode) {
    short regx = (opcode & 0x0f00) >> 8;
    short regy = (opcode & 0x00F0) >> 4;
    short height = opcode & 0x000f;

    int screenx = Registers[regx];
    int screeny = Registers[regy];

    Registers[0xf] = 0;
    for (int y = 0; y < height; y++) {

        BYTE data = GameRAM[RegisterI + y];
        int xlinev = 7;
        for (int x = 0; x < 8; x++, xlinev--) {
            int mask = 1 << xlinev;
            if (data & mask) {
                int posx = screenx + x;
                int posy = screeny + y;
                if (Screen[posx][posy] == 1) {
                    Registers[0xf] = 1;
                }
                Screen[posx][posy] ^= 1;
            }
        }
    }
}

void decodeOpcode(WORD opcode) {
    switch (opcode & 0xF000) {

    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000:
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    Screen[x][y] = 0;
                }
            }
            break;
        case 0x000E:
            ProgramCounter = pop(&stack);
            break;
        }
        break;

    case 0x1000:
        ProgramCounter = opcode & 0x0FFF;
        break;

    case 0x2000:
        push(&stack, ProgramCounter + 2);
        ProgramCounter = opcode & 0x0FFF;
        break;
    case 0x3000:
        if (Registers[opcode & 0x0F00] == (opcode & 0x00FF)) {
            ProgramCounter += 2;
        };
        break;
    case 0x4000:
        if (Registers[opcode & 0x0F00] != (opcode & 0x00FF)) {
            ProgramCounter += 2;
        };
        break;
    case 0x5000:
        if (Registers[opcode & 0x0F00] == Registers[opcode & 0x00F0]) {
            ProgramCounter += 2;
        }
        break;

    case 0x6000:
        Registers[opcode & 0x0F00] = opcode & 0x00FF;
        break;
    case 0x7000:
        Registers[opcode & 0x0F00] += opcode & 0x00FF;
        break;
    case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0000:
            Registers[opcode & 0x0F00] = Registers[opcode & 0x00F0];
            break;
        case 0x0002:
            Registers[opcode & 0x0f00] &= Registers[opcode & 0x00f0];
            break;
        case 0x0004:
            Registers[opcode & 0x0F00] += Registers[opcode & 0x00F0];
            break;
        case 0x0005:
            Registers[opcode & 0x0F00] -= Registers[opcode & 0x00F0];
            break;

        case 0x000e:

            break;
        default:
            printf("opcode : %#x   : Program Counter %i", opcode,
                   ProgramCounter);
            exit(-1);
            break;
        }
        break;

    case 0xA000:
        RegisterI = opcode & 0x0FFF;
        break;

    case 0xc000:
        Registers[opcode & 0x0F00] = (rand() % (256)) & (opcode & 0x00FF);
        break;
    case 0xd000:

        draw(opcode);
        break;
    case 0xe000:
        ProgramCounter += 2;
        // implament keyboard input
        break;

    case 0xf000:
        switch (opcode & 0x000F) {

        case 0x0005:
            timer = Registers[opcode & 0x0F00];
            break;
        case 0x0007:
            Registers[opcode & 0x0F00] = timer;
            break;
        case 0x000e:
            RegisterI += Registers[opcode & 0x0F00];
            break;

        default:
            break;
            printf("opcode : %#x   : Program Counter %i", opcode,
                   ProgramCounter);
            exit(-1);
        }

        break;
    default:
        printf("opcode : %#x   : Program Counter %i", opcode, ProgramCounter);
        exit(-1);
    }
}

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int main(int argc, char **argv) {

    // Initialization
    if (argc != 2) {
        printf("ERROR :: Pass in File name\n");
        exit(-1);
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("ERROR :: Failed opening file\n %d", errno);
        exit(-1);
    }

    fread(&GameRAM[0x200], 0xFFF, sizeof(BYTE), file);
    long long last = timeInMilliseconds();

    init(&stack);
    while (true) {
        if ((timeInMilliseconds() - last > 6)) {
            last = timeInMilliseconds();
            WORD opcode = fetchOpcode();
            decodeOpcode(opcode);

            flushScreen();
            if (timer != 0) {
                timer -= 1;
            } else {
                timer = 60;
            }
        }
    }
    fclose(file);
    return 1;
}
