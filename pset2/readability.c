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

// gradeFloat = 0.0588 * L - 0.296 * S - 15.8
// L := the average number of letters per 100 words in the text
// S := the average number of sentences per 100 words in the text
// grade should be "rounded to the nearest whole number"

int lettersInWord(string word);
bool endOfSentence(string word);

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
        numWords++;
        numLetters += lettersInWord(word);
        numSentences += endOfSentence(word) ? 1 : 0;
        // From the strtok man page:
        // "In each subsequent call that should parse the same string, str must be NULL."
        word = strtok(NULL, delimeters);
    }

    // do some division
    float numHundredsOfWords = numWords / 100.0;
    float lettersPer100Words = numLetters / numHundredsOfWords;
    float sentencesPer100Words = numSentences / numHundredsOfWords;
    // calculate the grade
    int grade = round(0.0588 * lettersPer100Words - 0.296 * sentencesPer100Words - 15.8);

    // Print the grade
    printf("Grade %i\n", grade);
}

int lettersInWord(string word)
{
    int numLetters = 0;
    int numCharacters = strlen(word);
    for (int i=0; i<numCharacters; i++)
    {
        numLetters += isalpha(word[i]) ? 1 : 0;
    }
    return numLetters;
}

bool endOfSentence(string word)
{
    // only check the last character
    int lastIndex = strlen(word) - 1;
    // how do sentences end?
    const string sentenceTerminators = "!?.";

    // iterate over possible sentence-ending characters
    int numTerminators = strlen(sentenceTerminators);
    for (int i=0; i < numTerminators; i++)
    {
        if (word[lastIndex] == sentenceTerminators[i])
        {
            return true;
        }
    }

    // there was no match
    return false;
}