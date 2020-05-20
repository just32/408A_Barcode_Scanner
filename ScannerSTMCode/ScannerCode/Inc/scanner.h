#ifndef __SCANNER_H
#define __SCANNER_H

#define X_SIZE 240
#define Y_SIZE 320

int img[X_SIZE][Y_SIZE][3];
int greyImg[X_SIZE][Y_SIZE];

void rgbToGreyscale();
void thresholdImg();
void normalizeBars(float bars[Y_SIZE], int maxBars);
int processCode(int code1, int code2, int code3, int code4);
void decodeImg(char *str);

#endif
