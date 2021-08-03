#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs50.h"

// define a BYTE as uint8_t (taken from volume.c)
typedef uint8_t BYTE;

// prototypes
bool moveToNextJpegStart(FILE *inputFile);
bool isAtJpegStart(FILE *input);
void copyJpeg(FILE *input, FILE *output);
void copyBlock(FILE *input, FILE *output);

int main(int argc, char *argv[])
{
    // check the command line input
    // we expect a file path
    if (argc != 2)
    {
        printf("Usage: ./recover card.raw\n");
        return 1;
    }

    // open the memory card image
    FILE *inputFile = fopen(argv[1], "r");
    // check for failure
    if (inputFile == NULL)
    {
        printf("Error: Failed to open %s for reading.\n", argv[1]);
        return 1;
    }

    // initialize JPEG recovery variables
    int jpegNumber = 0;
    char outFilename[8];

    // move the file read position to the start of the first JPEG
    bool foundJpeg = moveToNextJpegStart(inputFile);
    // loop
    while (foundJpeg && !feof(inputFile))
    {
        // set the new jpeg filename
        sprintf(outFilename, "%03i.jpg", jpegNumber);
        // open the new file
        FILE *outputFile = fopen(outFilename, "w");
        // copy the contents over
        copyJpeg(inputFile, outputFile);
        // close the output file
        fclose(outputFile);
        // increment for the next one
        jpegNumber++;
    }

    // all done
    fclose(inputFile);
    return 0;
}

bool moveToNextJpegStart(FILE *inputFile)
{
    // walk the bytes, looking for the JPEG signature
    while (!feof(inputFile))
    {
        // record the current file read position
        long int startPosition = ftell(inputFile);
        // check if this is the start of a JPEG
        if (isAtJpegStart(inputFile))
        {
            // seek back to the start of the block
            fseek(inputFile, startPosition, SEEK_SET);
            return true;
        }
    }
    return false;
}

// check if this block starts a new JPEG
// this function will return the read pointer to where it was
bool isAtJpegStart(FILE *input)
{
    // read one block
    BYTE fatBlock[512];
    fread(&fatBlock, sizeof(BYTE), 512, input);
    // check the first 24 bits
    bool magic24 = fatBlock[0] == 0xff && fatBlock[1] == 0xd8 && fatBlock[2] == 0xff;
    // check the last four bits (half-byte) of the head
    // mask the first four bits and compare the first four bits to 1110 (0xe)
    bool jpegHeaderFound = magic24 && (fatBlock[3] & 0xf0) == 0xe0;
    // answer the caller
    return jpegHeaderFound;
}

// the input file is set to the start of the JPEG
// the output file is already open and ready to write
// this function does not close the output file
void copyJpeg(FILE *input, FILE *output)
{
    bool stopCopying = false;
    while (!stopCopying)
    {
        // copy one block
        copyBlock(input, output);
        // fseek willl clear feof, so it has to be checked before we seek
        if (feof(input))
        {
            break;
        }
        // check the next block for a new JPEG
        // record the current file read position
        long int startPosition = ftell(input);
        stopCopying = isAtJpegStart(input);
        // seek back to the start of the block
        fseek(input, startPosition, SEEK_SET);
    }
}

// copy one block
void copyBlock(FILE *input, FILE *output)
{
    BYTE fatBlock[512];
    // read a block
    int bytesRead = fread(&fatBlock, sizeof(BYTE), 512, input);
    // write that block to the output
    fwrite(&fatBlock, sizeof(BYTE), bytesRead, output);
}