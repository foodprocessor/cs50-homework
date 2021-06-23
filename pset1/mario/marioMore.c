#include <cs50.h>
#include <stdio.h>

void printChars(char character, int times);

int main(void)
{
    // Prompt for a size between 1 and 8
    int size;
    do
    {
        size = get_int("Height: ");
    }
    while (size < 1 || size > 8);

    // Make towers
    for (int i = 1; i <= size; i++)
    {
        printChars(' ', size - i);
        printChars('#', i);
        printf("  ");
        printChars('#', i);
        printf("\n");
    }
}

// Print a character many times
void printChars(char character, int times)
{
    for (int i = 0; i < times; i++)
    {
        printf("%c", character);
    }
}