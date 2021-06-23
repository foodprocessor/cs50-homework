#include <stdio.h> // printf
#include <cs50.h> // get_float
#include <math.h> // round

int main(void)
{
    bool validAmount = false;
    // Prompt for a quantity of change
    float dollars = 0.0;
    do
    {
        dollars = get_float("Change owed: ");
    }
    while (dollars < 0);

    // convert to cents
    int cents = round(dollars * 100);

    // figure out how many coins are needed
    const int numCoinTypes = 4;
    const int coins[] = {25, 10, 5, 1};

    // initialize the answer
    int numChangeCoins = 0;
    // loop over the coin types
    for (int coinType = 0; coinType < numCoinTypes; coinType++)
    {
        int coinCents = coins[coinType];
        while (cents >= coinCents)
        {
            cents -= coinCents;
            numChangeCoins++;
        }
    }

    // print the result
    printf("%i\n", numChangeCoins);
}