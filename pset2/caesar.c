// classifying characters (isalpha, tolower, etc.)
#include <ctype.h>
// for the "string" type
#include <cs50.h>
// for printf
#include <stdio.h>
// for strlen
#include <string.h>
// for round
#include <math.h>
// for atoi
#include <stdlib.h>

// $ ./caesar 13
// plaintext:  HELLO
// ciphertext: URYYB

char ecryptLetter(char plainChar, int key);

int main(int argc, string argv[])
{
    // take in command line argument (cipher / offset)
    if (argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }
    int key = atoi(argv[1]);

    // Get text
    string text = get_string("plaintext:  ");

    // encrypt the string (just modify in place)
    unsigned int textLength = strlen(text);
    for (int i = 0; i < textLength; i++)
    {
        text[i] = ecryptLetter(text[i], key);
    }

    // print the result
    printf("ciphertext: %s\n", text);
}

char ecryptLetter(char plainChar, int key)
{
    // skip non-letters
    if (!isalpha(plainChar))
    {
        return plainChar;
    }

    // figure out which letter of the alphabet it is (a being the zeroth)
    int plainAlphabetPosition = tolower(plainChar) - 'a';

    // add the key, modulo 26
    int lettersInAlphabet = 26;
    int cipherAlphabetPosition = (plainAlphabetPosition + key) % lettersInAlphabet;

    // convert it back to a letter
    char cipherChar = cipherAlphabetPosition + 'a';

    // make it capital if it was a capital letter
    if (isupper(plainChar))
    {
        cipherChar += 'A' - 'a';
    }

    // now return the encrypted character!
    return cipherChar;
}