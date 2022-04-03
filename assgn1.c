#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct tagBITMAPFILEHEADER
{
    WORD bfType;      //specifies the file type
    DWORD bfSize;     //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct tagBITMAPINFOHEADER
{
    DWORD biSize;         //specifies the number of bytes required by the struct
    LONG biWidth;         //specifies width in pixels
    LONG biHeight;        //species height in pixels
    WORD biPlanes;        //specifies the number of color planes, must be 1
    WORD biBitCount;      //specifies the number of bit per pixel
    DWORD biCompression;  //spcifies the type of compression
    DWORD biSizeImage;    //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed;      //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important
};

unsigned char *readBMP(struct tagBITMAPFILEHEADER *, struct tagBITMAPINFOHEADER *, FILE *);
void writeBMP(struct tagBITMAPFILEHEADER imgf, struct tagBITMAPINFOHEADER imgi, unsigned char *, FILE *fw);
unsigned char getColor(int x, int y, struct tagBITMAPINFOHEADER imgi, unsigned char *bmp, int c, int *);
int main(int argc, char *argv[])

{
    struct tagBITMAPFILEHEADER *img1F = (struct tagBITMAPFILEHEADER *)malloc(sizeof(struct tagBITMAPFILEHEADER));
    struct tagBITMAPFILEHEADER *img2F = (struct tagBITMAPFILEHEADER *)malloc(sizeof(struct tagBITMAPFILEHEADER));
    struct tagBITMAPINFOHEADER *img1I = (struct tagBITMAPINFOHEADER *)malloc(sizeof(struct tagBITMAPINFOHEADER));
    struct tagBITMAPINFOHEADER *img2I = (struct tagBITMAPINFOHEADER *)malloc(sizeof(struct tagBITMAPINFOHEADER));
    unsigned char *bmpimg1;
    unsigned char *bmpimg2;
    unsigned char *outimage;
    bool printMan = false;
    double ratio;
    if (argc == 5)
    {
        sscanf(argv[3], "%lf", &ratio);
    }
    if (argc != 5)
    {
        printf("There should be 4 command line arguements\n");
        printMan = true;
    }
    else if ((strstr(argv[1], ".bmp") == NULL))
    {
        printf("\nfile name 1 is not of type .bmp\n");
        printMan = true;
    }
    else if ((strstr(argv[2], ".bmp") == NULL))
    {
        printf("\nfile name 2 is not of type .bmp\n");
        printMan = true;
    }
    else if ((strstr(argv[4], ".bmp") == NULL))
    {
        printf("\nOutput file name is not of type .bmp\n");
        printMan = true;
    }
    else if (ratio > 1 || ratio < 0)
    {
        printf("\nRatio should be no greater than 1 and no less than 0\n");
        printMan = true;
    }
    if (printMan)
    {
        printf("\nEnter commandline arguements with the following format:\n");
        printf("\n[imagefile1] [imagefile2] [ratio] [outputfile]\n");
        printf("\nExample:\nfilename1.bmp filename2.bmp .3 outputname.bmp\n");
    }
    else
    {
        FILE *f = fopen(argv[1], "rb");
        FILE *fw = fopen("output.bmp", "wb");
        if (f == NULL)
        {
            printf("failed to open file: %s",argv[1]);
            return 0;
        }
        bmpimg1 = readBMP(img1F, img1I, f);
        fclose(f);

        f = fopen(argv[2], "rb");
        if (f == NULL)
        {
            printf("failed to open file: %s",argv[2]);
            return 0;
        }
        bmpimg2 = readBMP(img2F, img2I, f);
        fclose(f);
        outimage = (unsigned char *)malloc(img1I->biSizeImage);
        if (img1I->biSizeImage == img2I->biSizeImage)
        {

            for (int i = 0; i < img1I->biSizeImage; i++)
            {
                outimage[i] = (bmpimg1[i] * ratio) + (bmpimg2[i] * (1 - ratio));
            }
        }
        else
        {
            if (img1I->biSizeImage < img2I->biSizeImage)
            {
                struct tagBITMAPFILEHEADER tempF = *img1F;
                struct tagBITMAPINFOHEADER tempI = *img1I;
                unsigned char *tempBMP = bmpimg1;
                *img1F = *img2F;
                *img1I = *img2I;
                bmpimg1 = bmpimg2;
                *img2F = tempF;
                *img2I = tempI;
                bmpimg2 = tempBMP;
            }
            outimage = (unsigned char *)malloc(img1I->biSizeImage);
            int offset1 = 0;
            int offset2 = 0;
            for (int j = 0; j < img1I->biHeight; j++)
            {
                for (int i = 0; i < img1I->biWidth; i++)
                {
                    double x = (i * img2I->biWidth) / img1I->biWidth;
                    double y = (j * img2I->biHeight) / img1I->biHeight;
                    int x1 = x;
                    int y1 = y;
                    int x2 = ceil(x);
                    int y2 = ceil(y);
                    double dx = x - x1;
                    double dy = y - y1;

                    for (int k = 0; k < 3; k++)
                    {
                        unsigned char left_upper = getColor(x1, y2, *img2I, bmpimg2, k, &offset2);
                        unsigned char right_upper = getColor(x2, y2, *img2I, bmpimg2, k, &offset2);
                        unsigned char left_lower = getColor(x1, y1, *img2I, bmpimg2, k, &offset2);
                        unsigned char right_lower = getColor(x2, y1, *img2I, bmpimg2, k, &offset2);
                        unsigned char left = left_upper * (1 - dy) + left_lower * dy;
                        unsigned char right = right_upper * (1 - dy) + right_lower * dy;
                        unsigned char result = left * (1 - dx) + right * dx;
                        int index = ((j * img1I->biWidth + i) * 3);
                        int factor = img1I->biWidth % 4;
                        if (factor != 0)
                        {

                            if ((index + 1) > factor)
                            {
                                if ((index + 1) % (img1I->biWidth * 3) > 0)
                                {

                                    if ((index + 1) % (img1I->biWidth * 3) <= factor)
                                    {

                                        offset1 = j * (factor);
                                    }
                                }
                            }
                        }
                        index = index + offset1 + k;
                        outimage[index] = (bmpimg1[index] * ratio) + (result * (1 - ratio));
                    }
                }
                //printf("\n");
            }
        }
        writeBMP(*img1F, *img1I, outimage, fw);
    }
}

unsigned char *readBMP(struct tagBITMAPFILEHEADER *imgf, struct tagBITMAPINFOHEADER *imgi, FILE *f)
{

    fread(&imgf->bfType, sizeof(imgf->bfType), 1, f);
    fread(&imgf->bfSize, sizeof(imgf->bfSize), 1, f);
    fread(&imgf->bfReserved1, sizeof(imgf->bfReserved1), 1, f);
    fread(&imgf->bfReserved2, sizeof(imgf->bfReserved2), 1, f);
    fread(&imgf->bfOffBits, sizeof(imgf->bfOffBits), 1, f);

    fread(&imgi->biSize, sizeof(imgi->biSize), 1, f);
    fread(&imgi->biWidth, sizeof(imgi->biWidth), 1, f);
    fread(&imgi->biHeight, sizeof(imgi->biHeight), 1, f);
    fread(&imgi->biPlanes, sizeof(imgi->biPlanes), 1, f);
    fread(&imgi->biBitCount, sizeof(imgi->biBitCount), 1, f);
    fread(&imgi->biCompression, sizeof(imgi->biCompression), 1, f);
    fread(&imgi->biSizeImage, sizeof(imgi->biSizeImage), 1, f);
    fread(&imgi->biXPelsPerMeter, sizeof(imgi->biXPelsPerMeter), 1, f);
    fread(&imgi->biYPelsPerMeter, sizeof(imgi->biYPelsPerMeter), 1, f);
    fread(&imgi->biClrUsed, sizeof(imgi->biClrUsed), 1, f);
    fread(&imgi->biClrImportant, sizeof(imgi->biClrImportant), 1, f);

    fseek(f, imgf->bfOffBits, SEEK_SET);
    unsigned char *bmpimg = (unsigned char *)malloc(imgi->biSizeImage);

    fread(bmpimg, sizeof(unsigned char), imgi->biSizeImage, f);

    return bmpimg;
}

void writeBMP(struct tagBITMAPFILEHEADER imgf, struct tagBITMAPINFOHEADER imgi, unsigned char *bmpimg, FILE *fw)
{
    int sum = 0;
    fwrite(&imgf.bfType, sizeof(imgf.bfType), 1, fw);
    fwrite(&imgf.bfSize, sizeof(imgf.bfSize), 1, fw);
    fwrite(&imgf.bfReserved1, sizeof(imgf.bfReserved1), 1, fw);
    fwrite(&imgf.bfReserved2, sizeof(imgf.bfReserved2), 1, fw);
    fwrite(&imgf.bfOffBits, sizeof(imgf.bfOffBits), 1, fw);

    fwrite(&imgi.biSize, sizeof(imgi.biSize), 1, fw);
    fwrite(&imgi.biWidth, sizeof(imgi.biWidth), 1, fw);
    fwrite(&imgi.biHeight, sizeof(imgi.biHeight), 1, fw);
    fwrite(&imgi.biPlanes, sizeof(imgi.biPlanes), 1, fw);
    fwrite(&imgi.biBitCount, sizeof(imgi.biBitCount), 1, fw);
    fwrite(&imgi.biCompression, sizeof(imgi.biCompression), 1, fw);
    fwrite(&imgi.biSizeImage, sizeof(imgi.biSizeImage), 1, fw);
    fwrite(&imgi.biXPelsPerMeter, sizeof(imgi.biXPelsPerMeter), 1, fw);
    fwrite(&imgi.biYPelsPerMeter, sizeof(imgi.biYPelsPerMeter), 1, fw);
    fwrite(&imgi.biClrUsed, sizeof(imgi.biClrUsed), 1, fw);
    fwrite(&imgi.biClrImportant, sizeof(imgi.biClrImportant), 1, fw);

    fwrite(bmpimg, sizeof(unsigned char), imgi.biSizeImage, fw);
}

unsigned char getColor(int x, int y, struct tagBITMAPINFOHEADER imgi, unsigned char *bmp, int c, int *offset)
{
    int index2 = ((y * imgi.biWidth + x) * 3);
    if (imgi.biWidth % 4 != 0)
    {
        if ((index2 + 1) > imgi.biWidth % 4)
        {

            if ((index2 + 1) % (imgi.biWidth * 3) > 0)
            {

                if ((index2 + 1) % (imgi.biWidth * 3) <= imgi.biWidth % 4)
                {
                    int factor = imgi.biWidth % 4;
                    *offset = y * (factor);
                }
            }
        }
    }
    return bmp[index2 + c + *offset];
}