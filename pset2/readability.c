// classifying characters (isalpha, tolower, etc.)
#include <ctype.h>
// for the "string" type
#include <cs50.h>
// for printf
#include <stdio.h>
// for strlen
#include <string.h>

// gradeFloat = 0.0588 * L - 0.296 * S - 15.8
// L := the average number of letters per 100 words in the text
// S := the average number of sentences per 100 words in the text
// grade should be "rounded to the nearest whole number"

int lettersInWord(string word);

int main(void)
{
    // Get text
    string text = get_string("Text: ");

    // Initialize counters
    int numLetters = 0;
    int numWords = 0;
    int numSentences = 0;
    // strtok(string, delimeters) modifies the given string by breaking it into a series of strings along the given list of delimiter characters
    // strtok returns these substrings, one at a time (each call to strtok returns one substring), until there are no more, and it returns NULL.
    // strtok requires an initial call which differes from the repeated calls in the loop
    string delimeters = " ";
    string word = strtok(text, delimeters);
    while (word != NULL)
    {
        // Get some numbers

        // From the strtok man page:
        // "In each subsequent call that should parse the same string, str must be NULL."
        word = strtok(NULL, delimeters);
    }
    // do some division
    float lettersPer100Words;
    float sentencesPer100Words;
    // calculate the grade
    int grade = round(0.0588 * lettersPer100Words - 0.296 * sentencesPer100Words - 15.8);
    printf("Grade %i", grade);
}

int compute_score(string word)
{
    // iterate over the letters
    int score = 0;
    int wordLength = strlen(word);
    for (int i = 0; i < wordLength; i++)
    {
        // just work with lower case
        char letter = tolower(word[i]);
        // skip non-letters
        if (!isalpha(letter))
        {
            continue;
        }
        // if we subtract 'a', we get which letter of the alphabet we have, starting with 'a' at 0
        unsigned int letterIndex = letter - 'a';
        score += POINTS[letterIndex];
    }
    return score;
}