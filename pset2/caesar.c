// classifying characters (for isdigit)
#include <ctype.h>
// for the "string" type
#include <cs50.h>
// for printf
#include <stdio.h>
// for strlen
#include <string.h>

// $ ./caesar 13
// plaintext:  HELLO
// ciphertext: URYYB

int parseKey(string keyString);
char ecryptLetter(char plainChar, int key);

int main(int argc, string argv[])
{
    bool validInput = true;
    // take in command line argument (cipher / offset)
    if (argc != 2)
    {
        validInput = false;
    }

    //get key
    int key = parseKey(argv[1]);

    // reject negative integers
    if (key < 0)
    {
        validInput = false;
    }

    // print usage
    if (!validInput)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

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

int parseKey(string keyString)
{
    int key = 0;
    // start at the beginning, eating one character at a time
    int keyLength = strlen(keyString);
    for (int i = 0; i < keyLength; i++)
    {
        // get the character
        char digit = keyString[i];
        // check that it's numeric
        if (!isdigit(digit))
        {
            return -1;
        }
        // add to key
        key = (key * 10) + digit;
    }

    return key;
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