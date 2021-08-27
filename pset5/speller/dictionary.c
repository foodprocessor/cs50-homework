// Implements a dictionary's functionality

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "crc16ccitt.c"
#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 0x10000;

// Hash table
node *table[N];

// cheat by keeping the number of words on load
int numWords = 0;

void printHashTable();

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // hash the word
    unsigned int wordHash = hash(word);
    // walk the linked list for this hash
    for (node *wordNode = table[wordHash]; wordNode != NULL; wordNode = wordNode->next)
    {
        char *dictWord = wordNode->word;
        int i = 0;
        while(true)
        {
            // the spec says the dictionary will be lowercase
            // so we save a step by not running tolower on the dictionary word
            char textWordLetter = tolower(word[i]);
            char dictWordLetter = dictWord[i];
            if (textWordLetter != dictWordLetter)
            {
                // not a match
                // break to look at the next entry in the linked list
                break;
            }
            else if (textWordLetter == '\0')
            {
                // both words have ended, without finding a difference
                return true;
            }
            // look at the next letter
            i++;
        }
    }
    // no match
    return false;
}

// Hashes word to a number
// inspired by / taken from https://www.php.net/manual/en/function.crc32.php
unsigned int hash(const char *word)
{
    int wordLength = 0;
    while (true)
    {
        if (word[wordLength] == '\0')
        {
            break;
        }
        wordLength++;
    }
    unsigned char *lowerWord = malloc(wordLength + 1);
    for (int i = 0; i < wordLength; i++)
    {
        lowerWord[i] = tolower(word[i]);
    }
    lowerWord[wordLength] = '\0';
    return CRCCCITT(lowerWord, wordLength, 65521, 127);
    // int i = 0, sum = 65521; // random prime starting value (not zero!)
    // while (true)
    // {
    //     if (word[i] == '\0')
    //     {
    //         return sum;
    //     }
    //     sum = (sum * 127 + tolower(word[i]));
    //     i++;
    // }
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // initialize the table
    for (int bucket = 0; bucket < N; bucket++)
    {
        table[bucket] = NULL;
    }
    // opent the file
    FILE *fileHandle = fopen(dictionary, "r");
    if (fileHandle == NULL)
    {
        printf("Failed to open file: %s\n", dictionary);
        return false;
    }
    // iterate over incoming dictionary words
    numWords = 0;
    int matches;
    do
    {
        // create a new node
        node *newWordNode = malloc(sizeof(node));
        newWordNode->next = NULL;
        // Brian says to use fscanf. Good idea!
        matches = fscanf(fileHandle, "%s\n", newWordNode->word);
        if (matches <= 0)
        {
            // no words were read in
            printf("Reached end of dictionary. Total words: %d\n", numWords);
            free(newWordNode);
            break;
        }
        // now let's insert the word into the hash table
        // first, hash the word using the has function
        unsigned int wordHash = hash(newWordNode->word);
        // to insert into the linked list, first copy the head pointer to the new node's next field
        newWordNode->next = table[wordHash];
        // now replace the head with the new node
        table[wordHash] = newWordNode;
        // increment the word count
        numWords++;
    } while(!feof(fileHandle));
    // tell the caller of our success
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // int numWords = 0;
    // for (int bucket = 0; bucket < N; bucket++)
    // {
    //     for (node *wordNode = table[bucket]; wordNode != NULL; wordNode = wordNode->next)
    //     {
    //         numWords++;
    //     }
    // }
    return numWords;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    printHashTable(); // debug
    // node *temp, *nodeToFree;
    for (int bucket = 0; bucket < N; bucket++)
    {
        // free from the head
        node *nodeToFree = table[bucket];
        // don't free NULL pointers!
        while (nodeToFree != NULL)
        {
            // move the list head forward
            table[bucket] = table[bucket]->next;
            // free the old head of the list
            free(nodeToFree);
            // get ready for the next iteration
            nodeToFree = table[bucket];
        }
    }
    return true;
}

void printHashTable()
{
    int bucketsPerRow = 100;
    int bucketsInThisRow = 0;
    int wordsPerBucket[N];
    for (int bucket = 0; bucket < N; bucket++)
    {
        int numWordsInBucket = 0;
        for (node *wordNode = table[bucket]; wordNode != NULL; wordNode = wordNode->next)
        {
            numWordsInBucket++;
        }
        printf("%d ", numWordsInBucket);
        bucketsInThisRow++;
        if (bucketsInThisRow > bucketsPerRow)
        {
            printf("\n");
            bucketsInThisRow = 0;
        }
    }
    printf("\n");
}