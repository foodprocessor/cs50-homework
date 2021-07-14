// Modifies the volume of an audio file

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// define a BYTE as uint8_t
typedef uint8_t BYTE;

// Number of bytes in .wav header
const int HEADER_SIZE = 44;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 4)
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    // Open files and determine scaling factor
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    float factor = atof(argv[3]);

    // Copy header from input file to output file
    BYTE header[HEADER_SIZE];
    int bytesRead = fread(header, sizeof(BYTE), HEADER_SIZE, input);
    if (bytesRead != HEADER_SIZE)
    {
        // a short read indicates an error.
        printf("Error reading input header: Only read %i bytes.\n", bytesRead);
        // was the file unexpectedly short?
        if (feof(input))
        {
            printf("Unexpected end of file when reading header!\n");
        }
        else if (ferror(input))
        {
            printf("Error when reading header!\n");
        }
        // Close files
        fclose(input);
        fclose(output);
    }
    // Write header to output file
    int bytesWritten = fwrite(header, sizeof(BYTE), HEADER_SIZE, output);
    if (bytesWritten != HEADER_SIZE)
    {
        printf("Failed to write header to output file! fwrite only reported %i/%i bytes written.\n", bytesWritten, HEADER_SIZE);
        // Close files
        fclose(input);
        fclose(output);
    }

    // Read samples from input file and write updated data to output file
    int16_t sample;
    while (fread(&sample, sizeof(int16_t), 1, input))
    {
        sample = (int16_t)(sample * factor);
        fwrite(&sample, sizeof(int16_t), 1, output);
    }

    // Close files
    fclose(input);
    fclose(output);
}
