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
int numNonSpaces(string text, int position);

int main(void)
{
    // Get text
    string text = get_string("Text: ");

    // Initialize counters
    int numLetters = 0;
    int numWords = 0;
    int numSentences = 0;
    // Variables for word parsing
    int textLength = strlen(text);
    int textPosition = 0;

    while (textPosition < textLength)
    {
        // Get the next word
        int wordLength = numNonSpaces(text, textPosition);
        char word[wordLength + 1];
        // this is a trick: if you add an integer to an array, it moves the start forward
        strncpy(word, text + textPosition, wordLength);
        word[wordLength] = '\0';
        textPosition += wordLength + 1; // skip the space
        // Get some numbers
        numWords++;
        numLetters += lettersInWord(word);
        numSentences += endOfSentence(word) ? 1 : 0;
    }

    // do some division
    float numHundredsOfWords = numWords / 100.0;
    float lettersPer100Words = numLetters / numHundredsOfWords;
    float sentencesPer100Words = numSentences / numHundredsOfWords;
    // calculate the grade
    int grade = round(0.0588 * lettersPer100Words - 0.296 * sentencesPer100Words - 15.8);

    // Print the grade
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }
}

int lettersInWord(string word)
{
    int numLetters = 0;
    int numCharacters = strlen(word);
    for (int i = 0; i < numCharacters; i++)
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
    for (int i = 0; i < numTerminators; i++)
    {
        if (word[lastIndex] == sentenceTerminators[i])
        {
            return true;
        }
    }

    // there was no match
    return false;
}

int numNonSpaces(string text, int textPosition)
{
    int wordLength = 0;
    int i = textPosition;
    while(text[i] != ' ' && text[i] != '\0')
    {
        wordLength++;
        i++;
    }
    return wordLength;
}