#include "helpers.h"
#include <stdio.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // examine the pointers created by calloc
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // get the pixel
            RGBTRIPLE *pixel = &image[row][col];
            // get the original colors
            BYTE red = pixel->rgbtRed;
            BYTE green = pixel->rgbtGreen;
            BYTE blue = pixel->rgbtBlue;
            // calculate the intensity (brightness)
            float averageIntensity = (red + green + blue) / 3.0;
            // set the intensity to all three color values
            // adding 0.5 to a number before truncating it is the same as rounding
            pixel->rgbtRed = averageIntensity + 0.5;
            pixel->rgbtGreen = averageIntensity + 0.5;
            pixel->rgbtBlue = averageIntensity + 0.5;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE temp;
    for (int row = 0; row < height; row++)
    {
        // we only need to go half way
        for (int col = 0; col < width/2; col++)
        {
            temp = image[row][col];
            int reflectedColumn = width - 1 - col;
            image[row][col] = image[row][reflectedColumn];
            image[row][reflectedColumn] = temp;
        }
    }
    return;
}

void printImagePixel(int row, int col, RGBTRIPLE pixel)
{
    printf("(%i,%i): #%x%x%x\n", row, col, pixel.rgbtRed, pixel.rgbtGreen, pixel.rgbtBlue);
}

_Bool validCoordinates(int row, int col, int height, int width)
{
    _Bool rowValid = row >= 0 && row < height;
    _Bool colValid = col >= 0 && col < width;
    return rowValid && colValid;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Can we use a small buffer (not the whole image)?
    // let's walk through the first steps

    // write the first three blured rows to the buffer
    // write the first blurred row back onto the source image
    // write the fourth blurred row to the buffer

    int blurSize = 3;
    RGBTRIPLE blurBuffer[blurSize][width];
    int blurBufferWriteRow = 0;
    int blurBufferReadRow;
    int filledBlurBufferRows = 0;

    // iterate over the image's pixels
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // initialize blur accumulators
            float redSum = 0, greenSum = 0, blueSum = 0;
            int boxPixelCount = 0;
            // blur using the box around this pixel
            for (int boxRow = row - 1; boxRow <= row + 1; boxRow++)
            {
                for (int boxCol = col - 1; boxCol <= col + 1; boxCol++)
                {
                    if (validCoordinates(boxRow, boxCol, height, width))
                    {
                        RGBTRIPLE boxPixel = image[boxRow][boxCol];
                        redSum += boxPixel.rgbtRed;
                        greenSum += boxPixel.rgbtGreen;
                        blueSum += boxPixel.rgbtBlue;
                        boxPixelCount++;
                    }
                }
            }
            // get the average (adding 0.5 rounds the result before truncation)
            RGBTRIPLE blurredPixel = {
                .rgbtRed = (redSum / boxPixelCount) + 0.5,
                .rgbtGreen = (greenSum / boxPixelCount) + 0.5,
                .rgbtBlue = (blueSum / boxPixelCount) + 0.5
            };
            // write the result into the blurred pixel buffer
            blurBuffer[blurBufferWriteRow][col] = blurredPixel;
        }
        // move to write the next blur buffer row
        blurBufferWriteRow = (blurBufferWriteRow + 1) % 3;
        filledBlurBufferRows++;
        // if there are three rows in the blur buffer, write one out to the original image
        if (filledBlurBufferRows == 3)
        {
            // why is this plus one? We're using a full circular buffer...
            // so the row to read is right after the row we just wrote!
            blurBufferReadRow = (blurBufferWriteRow + 1) % 3;
            // copy the row
            for (int col = 0; col < width; col++)
            {
                image[row-2][col] = blurBuffer[blurBufferReadRow][col];
                // printImagePixel(row-2, col, image[row-2][col]);
            }
            // update the number of remaining rows in the buffer
            filledBlurBufferRows--;
        }
    }
    // after this loop exits, the blurBuffer will still have two more rows that need to be written to the image
    int imageWriteRow = height - (blurSize - 1);
    while (imageWriteRow < height)
    {
        blurBufferReadRow = (blurBufferReadRow + 1) % blurSize;
        // copy the row
        for (int col = 0; col < width; col++)
        {
            image[imageWriteRow][col] = blurBuffer[blurBufferReadRow][col];
            // printImagePixel(imageWriteRow, col, image[imageWriteRow][col]);
        }
        // move on to the next row
        imageWriteRow++;
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    return;
}
