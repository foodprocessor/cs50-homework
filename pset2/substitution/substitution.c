// classifying characters (for isdigit)
#include <ctype.h>
// for the "string" type
#include <cs50.h>
// for printf
#include <stdio.h>
// for strlen
#include <string.h>

// I have copied the assignment text here for reference
// https://cs50.harvard.edu/x/2021/psets/2/substitution/

// Substitution

// Implement a program that implements a substitution cipher, per the below.

// $ ./substitution JTREKYAVOGDXPSNCUIZLFBMWHQ
// plaintext:  HELLO
// ciphertext: VKXXN

// Background

// In a substitution cipher, we “encrypt” (i.e., conceal in a reversible way) a message by replacing every letter with another letter. To do so, we use a key: in this case, a mapping of each of the letters of the alphabet to the letter it should correspond to when we encrypt it. To “decrypt” the message, the receiver of the message would need to know the key, so that they can reverse the process: translating the encrypt text (generally called ciphertext) back into the original message (generally called plaintext).
// A key, for example, might be the string NQXPOMAFTRHLZGECYJIUWSKDVB. This 26-character key means that A (the first letter of the alphabet) should be converted into N (the first character of the key), B (the second letter of the alphabet) should be converted into Q (the second character of the key), and so forth.
// A message like HELLO, then, would be encrypted as FOLLE, replacing each of the letters according to the mapping determined by the key.
// Let’s write a program called substitution that enables you to encrypt messages using a substitution cipher. At the time the user executes the program, they should decide, by providing a command-line argument, on what the key should be in the secret message they’ll provide at runtime.
// Here are a few examples of how the program might work. For example, if the user inputs a key of YTNSHKVEFXRBAUQZCLWDMIPGJO and a plaintext of HELLO:

// $ ./substitution YTNSHKVEFXRBAUQZCLWDMIPGJO
// plaintext:  HELLO
// ciphertext: EHBBQ

// Here’s how the program might work if the user provides a key of VCHPRZGJNTLSKFBDQWAXEUYMOI and a plaintext of hello, world:
// $ ./substitution VCHPRZGJNTLSKFBDQWAXEUYMOI
// plaintext:  hello, world
// ciphertext: jrssb, ybwsp

// Notice that neither the comma nor the space were substituted by the cipher. Only substitute alphabetical characters! Notice, too, that the case of the original message has been preserved. Lowercase letters remain lowercase, and uppercase letters remain uppercase.
// Whether the characters in the key itself are uppercase or lowercase doesn’t matter. A key of VCHPRZGJNTLSKFBDQWAXEUYMOI is functionally identical to a key of vchprzgjntlskfbdqwaxeuymoi (as is, for that matter, VcHpRzGjNtLsKfBdQwAxEuYmOi).

// And what if a user doesn’t provide a valid key?
// $ ./substitution ABC
// Key must contain 26 characters.
// Or really doesn’t cooperate?
// $ ./substitution
// Usage: ./substitution key
// Or even…
// $ ./substitution 1 2 3
// Usage: ./substitution key

// Specification
// Design and implement a program, substitution, that encrypts messages using a substitution cipher.
//     Implement your program in a file called substitution.c in a directory called substitution.
//     Your program must accept a single command-line argument, the key to use for the substitution. The key itself should be case-insensitive, so whether any character in the key is uppercase or lowercase should not affect the behavior of your program.
//     If your program is executed without any command-line arguments or with more than one command-line argument, your program should print an error message of your choice (with printf) and return from main a value of 1 (which tends to signify an error) immediately.
//     If the key is invalid (as by not containing 26 characters, containing any character that is not an alphabetic character, or not containing each letter exactly once), your program should print an error message of your choice (with printf) and return from main a value of 1 immediately.
//     Your program must output plaintext: (without a newline) and then prompt the user for a string of plaintext (using get_string).
//     Your program must output ciphertext: (without a newline) followed by the plaintext’s corresponding ciphertext, with each alphabetical character in the plaintext substituted for the corresponding character in the ciphertext; non-alphabetical characters should be outputted unchanged.
//     Your program must preserve case: capitalized letters must remain capitalized letters; lowercase letters must remain lowercase letters.
//     After outputting ciphertext, you should print a newline. Your program should then exit by returning 0 from main.

bool validKey(string keyString);
char ecryptLetter(char plainChar, string key);

int main(int argc, string argv[])
{
    bool validInput = true;
    string key;
    // take in command line argument (cipher / offset)
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        validInput = false;
    }
    else
    {
        // assign key to the input
        key = argv[1];
        // validate key
        if (!validKey(key))
        {
            validInput = false;
        }
    }
    // quit on invalid input
    if (!validInput)
    {
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

    // exit
    return 0;
}

bool validKey(string keyString)
{
    // initialize key (uniqueness checker)
    int lettersInAlphabet = 26;
    char key[lettersInAlphabet];
    for (int i = 0; i < lettersInAlphabet; i++)
    {
        key[i] = 0;
    }

    // start at the beginning, eating one character at a time
    int keyLength = strlen(keyString);
    // key must have 26 characters
    if (keyLength != 26)
    {
        printf("Key must contain 26 characters.\n");
        return false;
    }
    for (int i = 0; i < keyLength; i++)
    {
        // reject non-alpha
        if (!isalpha(keyString[i]))
        {
            printf("Encountered non-alpha character in key: %c\n", keyString[i]);
            return false;
        }
        int positionInAlphabet = tolower(keyString[i]) - 'a';
        // reject duplicates
        if (key[positionInAlphabet] != 0)
        {
            printf("Encountered letter %c more than once in key!\n", keyString[i]);
            return false;
        }
        // accept
        key[positionInAlphabet] = keyString[i];
    }

    return true;
}

char ecryptLetter(char plainChar, string key)
{
    // skip non-letters
    if (!isalpha(plainChar))
    {
        return plainChar;
    }

    // figure out which letter of the alphabet it is (a being the zeroth)
    int plainAlphabetPosition = tolower(plainChar) - 'a';

    // lookup the character in the key
    char cipherChar = key[plainAlphabetPosition];

    // make it capital if it was a capital letter
    if (isupper(plainChar))
    {
        cipherChar = toupper(cipherChar);
    }
    else
    {
        cipherChar = tolower(cipherChar);
    }

    // now return the encrypted character!
    return cipherChar;
}