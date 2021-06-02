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
int nullNextSpace(string text, int position);

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
    int thisWordPosition = 0;
    int nextWordPosition;

    while (thisWordPosition < textLength)
    {
        // Get the next word
        // Tricky code here: adding an int to a string moves the start of the string forward
        string word = text + thisWordPosition;
        // Tricky code here: nulling the space in the original text also affects the "word" variable
        thisWordPosition = nullNextSpace(text, thisWordPosition);
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

int nullNextSpace(string text, int textPosition)
{
    int i = textPosition;
    while(text[i] != ' ' && text[i] != '\0')
    {
        i++;
    }
    text[i] = '\0';
    return i + 1;
}