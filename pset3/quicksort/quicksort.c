#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

// prototype
void sort(int array[], int length);

int main(int argc, string argv[])
{
    // the number of numbers is on the command line
    if (argc != 2)
    {
        return 1;
    }
    int numValues = atoi(argv[1]);

    // get numbers
    int array[numValues];
    for (int i = 0; i < numValues; i++)
    {
        array[i] = get_int("");
    }

    // sort numbers
    sort(array, numValues);

    // print the result
    int numbersPerRow = 10;
    for (int i = 0; i < numValues/numbersPerRow; i++)
    {
        for (int j = 0; j < numbersPerRow; j++)
        {
            int index = (i * numbersPerRow) + j;
            if (index > numValues)
            {
                break;
            }
            else
            {
                printf("%i ",array[index]);
            }
        }
        printf("\n");
    }
}

void sort(int array[], int length)
{
    // base case
    if (length <= 1)
    {
        return;
    }

    // pick a random value for a guessed median
    int middle = array[length / 2];
    // loop from the outside in
    int bottomIndex = 0;
    int topIndex = length - 1;
    // this loop exit condition is designed to get the indices to just pass each other before exiting
    while (bottomIndex <= topIndex)
    {
        int bottom = array[bottomIndex];
        int top = array[topIndex];
        bool bottomNeedsToSwap = bottom >= middle;
        bool topNeedsToSwap = top <= middle;
        if (!bottomNeedsToSwap)
        {
            bottomIndex++;
        }
        if (!topNeedsToSwap)
        {
            topIndex--;
        }
        else if (bottomNeedsToSwap && topNeedsToSwap)
        {
            if (bottomIndex != topIndex)
            {
                // swap them!
                array[bottomIndex] = top;
                array[topIndex] = bottom;
                // move the pointers
                bottomIndex++;
                topIndex--;
            }
            else // topIndex == bottomIndex
            {
                // if the indices are the same,
                // incrementing both will break the recursion math below
                topIndex--;
            }
        }
    }

    // recurse on bottom and top sections
    // the bottom section is from index 0 with length topIndex + 1
    sort(array, topIndex + 1);
    // the top section is from bottomIndex with length length - bottomIndex
    sort(array + bottomIndex, length - bottomIndex);
}