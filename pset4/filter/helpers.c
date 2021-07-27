#include "helpers.h"
#include <stdio.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // examine the pointers created by calloc
    printf("after calloc, image is set to %p\n", image;
    for (int row = 0; row < height; row++)
    {
        printf("&image[%i] is %p, and image[%i] is %p\n", row, &image[row], row, image[row]);
        for (int col = 0; col < width; col++)
        {
            RGBTRIPLE *pixel = &image[row][col];
            printf("&image[%i][%i] is %p, and image[%i][%i] is #%x%x%x\n", row, col, &image[row][col], row, col, pixel->rgbtRed, pixel->rgbtGreen, pixel->rgbtBlue);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    return;
}
