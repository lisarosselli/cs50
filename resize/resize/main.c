/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: resize f infile outfile\n");
        return 1;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // remember sizing attempt
    float factor = strtof(argv[1], '\0');
    
    if (factor >= 1.0 && factor <= 100.0)
    {
        // good
    } else
    {
        printf("Cannot resize to %.4f\n", factor);
        return 4;
    }
    
    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    
    // break here!
    int newWidth = floor(bi.biWidth * factor);
    int newHeight = floor(bi.biHeight * factor);
    newHeight += 1;
    int roundedFactor = round(factor);
    
    printf("newWidth=%i\n", newWidth);
    printf("newHeight=%i\n", newHeight);
    printf("roundedFactor=%i\n", roundedFactor);
    
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // update BITMAPFILEHEADER for outfile
    int newBfSize = (newWidth * abs(newHeight));
    newBfSize *= 3; // 24 bits/pixel = 3 bytes/pixel
    newBfSize += bf.bfOffBits; // add in the size of the meta data
    bf.bfSize = newBfSize;
    printf("bf.bfSize=%i\n", bf.bfSize);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // remember the old dimensions
    int oldWidth = bi.biWidth;
    int oldHeight = bi.biHeight;
    
    // update BITMAPINFOHEADER for outfile
    bi.biWidth = newWidth;
    bi.biHeight = newHeight;
    
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    
    // determine padding for scanlines
    int inpadding =  (4 - (oldWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int outpadding = 4 - (bi.biWidth % 4);
    
    
    // iterate over infile's scanlines
    //for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    for (int i = 0, biHeight = abs(oldHeight); i < biHeight; i++)
    {
        
        for (int n = 0; n < roundedFactor; n ++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < oldWidth; j++)
            {

                printf("i=%i, n=%i, j=%i\n", i, n, j);
                // temporary storage
                RGBTRIPLE triple;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                
                
                // "replicate" pixels some 'factor' times
                for (int f = roundedFactor - 1; f > 0; f--)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                
            }
            
            // at the end of my scanline...
            int rewind = (3 * oldWidth);
            if (n == (roundedFactor - 1))
            {
                // do not rewind
                // but add in any padding that may exist
                fseek(inptr, inpadding, SEEK_CUR);
            } else if (n < (roundedFactor - 1))
            {
                // rewind scanline
                fseek(inptr, -rewind, SEEK_CUR);
            }
            
            
            // then add it back (to demonstrate how)
            for (int k = 0; k < outpadding; k++)
            {
                fputc(0x00, outptr);
            }

            
        }

    }
    
    // close infile
    fclose(inptr);
    
    // close outfile
    fclose(outptr);
    
    // that's all folks
    return 0;
}
