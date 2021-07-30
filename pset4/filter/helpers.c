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

void printPixel(char *caption, int row, int col, RGBTRIPLE pixel)
{
    printf("%s (%i,%i): (%i,%i,%i)\n", caption, row, col, pixel.rgbtRed, pixel.rgbtGreen, pixel.rgbtBlue);
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
    _Bool debug = 0;
    // Can we use a small buffer (not the whole image)?
    // write the first block of blured rows to the buffer
    // write the first blurred row back onto the source image
    // write the next blurred row over the row just written out to the image
    // loop like that, one row in, one row out, until we get to the end
    // at the end, there will be rows left in the buffer that haven't been written
    // write out the remaining rows

    // needed variables
    // calculations ready flag
    // calculations done flag
    // image read row
    // image write row
    // buffer write row
    // buffer read row

    int blurRadius = 1;
    int blurSize = (blurRadius * 2) + 1;
    RGBTRIPLE blurBuffer[blurSize][width];
    int bufferWriteRow = 0;
    int bufferReadRow = 0;
    int imageReadRow = 0;
    int imageWriteRow = 0;

    while (imageWriteRow < height)
    {
        // if we haven't read all the rows, read a row of the image
        if (imageReadRow < height)
        {
            for (int col = 0; col < width; col++)
            {
                if (debug) {
                    printPixel("original", imageReadRow, col, image[imageReadRow][col]);
                }
                // initialize blur accumulators
                float redSum = 0, greenSum = 0, blueSum = 0;
                int boxPixelCount = 0;
                // blur using the box around this pixel
                for (int boxRow = imageReadRow - blurRadius; boxRow <= imageReadRow + blurRadius; boxRow++)
                {
                    for (int boxCol = col - blurRadius; boxCol <= col + blurRadius; boxCol++)
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
                blurBuffer[bufferWriteRow][col] = blurredPixel;
            }
            bufferWriteRow = (bufferWriteRow + 1) % blurSize;
            imageReadRow++;
        }

        // if the buffer is ready, write a row from it onto the image
        // also do this if the image is done being read (all calculations have ended)
        if (imageReadRow >= imageWriteRow + blurSize || imageReadRow == height)
        {
            // copy a row
            for (int col = 0; col < width; col++)
            {
                image[imageWriteRow][col] = blurBuffer[bufferReadRow][col];
                if (debug)
                {
                    printPixel("blurred", imageWriteRow, col, image[imageWriteRow][col]);
                }
            }
            imageWriteRow++;
            bufferReadRow = (bufferReadRow + 1) % blurSize;
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    return;
}
