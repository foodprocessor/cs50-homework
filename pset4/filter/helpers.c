#include "helpers.h"
#include <stdio.h>
#include <math.h>

_Bool debug = 0;

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
    // Can we use a small buffer (not the whole image)?
    // write the first block of blured rows to the buffer
    // write the first blurred row back onto the source image
    // write the next blurred row over the row just written out to the image
    // loop like that, one row in, one row out, until we get to the end
    // at the end, there will be rows left in the buffer that haven't been written
    // write out the remaining rows

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
    int kernelRadius = 1;
    int kernelSize = (kernelRadius * 2) + 1;
    RGBTRIPLE kernelBuffer[kernelSize][width];
    int bufferWriteRow = 0;
    int bufferReadRow = 0;
    int imageReadRow = 0;
    int imageWriteRow = 0;
    int gxKernel[3][3] =
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int gyKernel[3][3] =
    {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

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
                // initialize kernel accumulators
                float redGxSum = 0, greenGxSum = 0, blueGxSum = 0;
                float redGySum = 0, greenGySum = 0, blueGySum = 0;
                // compute kernel using the box around this pixel
                for (int boxRow = imageReadRow - kernelRadius, kernelRow = 0; boxRow <= imageReadRow + kernelRadius; boxRow++, kernelRow++)
                {
                    for (int boxCol = col - kernelRadius, kernelCol = 0; boxCol <= col + kernelRadius; boxCol++, kernelCol++)
                    {
                        if (validCoordinates(boxRow, boxCol, height, width))
                        {
                            // get the pixel (in the 3x3 grid)
                            RGBTRIPLE boxPixel = image[boxRow][boxCol];
                            // compute Gx color values
                            int weightGx = gxKernel[kernelRow][kernelCol];
                            redGxSum += boxPixel.rgbtRed * weightGx;
                            greenGxSum += boxPixel.rgbtGreen * weightGx;
                            blueGxSum += boxPixel.rgbtBlue * weightGx;
                            // compute Gy color values
                            int weightGy = gyKernel[kernelRow][kernelCol];
                            redGySum += boxPixel.rgbtRed * weightGy;
                            greenGySum += boxPixel.rgbtGreen * weightGy;
                            blueGySum += boxPixel.rgbtBlue * weightGy;
                        }
                    }
                }
                // combine the kernels
                float redEdge = sqrt(pow(redGxSum, 2) + pow(redGySum, 2));
                float greenEdge = sqrt(pow(greenGxSum, 2) + pow(greenGySum, 2));
                float blueEdge = sqrt(pow(blueGxSum, 2) + pow(blueGySum, 2));
                // cap at 255
                if (redEdge > 255)
                {
                    redEdge = 255;
                }
                if (greenEdge > 255)
                {
                    greenEdge = 255;
                }
                if (blueEdge > 255)
                {
                    blueEdge = 255;
                }
                // adding 0.5 before truncating to whole numbers rounds the values
                RGBTRIPLE edgePixel = {
                    .rgbtRed = redEdge + 0.5,
                    .rgbtGreen = greenEdge + 0.5,
                    .rgbtBlue = blueEdge + 0.5
                };
                // write the result into the pixel buffer
                kernelBuffer[bufferWriteRow][col] = edgePixel;
            }
            bufferWriteRow = (bufferWriteRow + 1) % kernelSize;
            imageReadRow++;
        }

        // if the buffer is ready, write a row from it onto the image
        // also do this if the image is done being read (all calculations have ended)
        if (imageReadRow >= imageWriteRow + kernelSize || imageReadRow == height)
        {
            // copy a row
            for (int col = 0; col < width; col++)
            {
                image[imageWriteRow][col] = kernelBuffer[bufferReadRow][col];
                if (debug)
                {
                    printPixel("edge", imageWriteRow, col, image[imageWriteRow][col]);
                }
            }
            imageWriteRow++;
            bufferReadRow = (bufferReadRow + 1) % kernelSize;
        }
    }
    return;
}
