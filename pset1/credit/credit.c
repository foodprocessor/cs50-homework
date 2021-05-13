#include <stdio.h>
#include <cs50.h>
#include <string.h>

int main(void)
{
    bool validNumber = false;
    // Prompt for a credit card number
    long int creditCardNumber;
    do
    {
        // get the number
        creditCardNumber = get_long("Number: ");

        // check for which card type this is
        // AmEx (15 digits starting with 34 or 37)
        bool isAmEx34 = creditCardNumber >= 34e13 && creditCardNumber < 35e13;
        bool isAmEx37 = creditCardNumber >= 37e13 && creditCardNumber < 38e13;
        bool isAmEx = isAmEx34 || isAmEx37;
        // Visa (13 or 16 digits starting with 4)
        bool isVisa13 = creditCardNumber >= 4e12 && creditCardNumber < 5e12;
        bool isVisa16 = creditCardNumber >= 4e15 && creditCardNumber < 5e15;
        bool isVisa = isVisa13 || isVisa16;
        // MasterCard (16 digits starting with 51-55)
        bool isMasterCard = creditCardNumber >= 51e14 && creditCardNumber < 56e14;

        // if the basic pattern doesn't match, start over
        if (!(isAmEx || isVisa || isMasterCard))
        {
            printf("INVALID\n");
            break;
        }

        // Checksum instructions (verbatim from the assignment):
        // 1. Multiply every other digit by 2, starting with the number’s second-to-last digit, and then add those products’ digits together.
        // 2. Add the sum to the sum of the digits that weren’t multiplied by 2.
        // 3. If the total’s last digit is 0 (or, put more formally, if the total modulo 10 is congruent to 0), the number is valid!

        // consume the number, two digits at a time, starting at the least significant end (the right side)
        long int remainingNumber = creditCardNumber;
        int evens = 0;
        int odds = 0;
        while (remainingNumber > 0)
        {
            // get the last two digits
            int thisChunk = remainingNumber % 100;
            // remove them from the remaining number
            remainingNumber /= 100;

            // split the current chunk into tens and ones
            int tens = thisChunk / 10;
            int ones = thisChunk % 10;

            // multiply the tens by two and add them to the sum of evens
            evens += tens * 2;
            // add the ones to the odds
            odds += ones;
        }

        // add the sums together
        int checksum = evens + odds;

        // check if it's 0 mod 10
        validNumber = checksum % 10 == 0;
        if (!validNumber)
        {
            printf("INVALID\n");
            break;
        }

        // if we made it this far, the number is VALID!
        // Now let's print out the card type
        if (isAmEx)
        {
            printf("AMEX\n");
        }
        else if (isVisa)
        {
            printf("VISA\n");
        }
        else if (isMasterCard)
        {
            printf("MASTERCARD\n");
        }
        else
        {
            // execution should never reach this branch!!!
            printf("This should never happen!!!!!!!!!\n");
        }
    }
    while (!validNumber);
}