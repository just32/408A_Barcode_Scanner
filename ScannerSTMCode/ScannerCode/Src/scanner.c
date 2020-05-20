/* ENEE408A Barcode Scanner Functionality
 * Christopher Leser, Abdul Koroma, Justin Frankle
 */

#include <stdio.h>
#include <math.h>
#include "scanner.h"

void rgbToGreyscale() {
  int x, y;

  for (x = 0; x < X_SIZE; x ++) {
    for (y = 0; y < Y_SIZE; y ++) {
      greyImg[x][y] = 0.3 * img[x][y][0] + 0.59 * img[x][y][1] + 0.11 * img[x][y][2]; // (0.3 * R) + (0.59 * G) + (0.11 * B)
    }
  }
}

void thresholdGreyImg() {
  int x, y;

  for (x = 0; x < X_SIZE; x ++) {
    for (y = 0; y < Y_SIZE; y ++) {
      if (greyImg[x][y] < 170) {
    	  greyImg[x][y] = 0;
      } else {
    	  greyImg[x][y] = 255;
      }
    }
  }
}

void normalizeBars(float bars[Y_SIZE], int maxBars) {
  float avg = (bars[0] + bars[1] + bars[2]) / 3.0;
  int i;

  //printf("avg: %f\n", avg);

  for (i = 0; i < maxBars; i ++) {
    bars[i] /= avg;
    if (i % 4 == 2) {
      //printf("%f\n", bars[i]);
    } else {
      //printf("%f ", bars[i]);
    }
    bars[i] = round(bars[i]);

    if (bars[i] == 0.0) {
      bars[i] = 1.0;
    }
  }
  printf("\n");
}

int processCode(int code1, int code2, int code3, int code4) {
  if (code1 == 3 && code2 == 2 && code3 == 1 && code4 == 1) {
    return 0;
  } else if (code1 == 2 && code2 == 2 && code3 == 2 && code4 == 1) {
    return 1;
  } else if (code1 == 2 && code2 == 1 && code3 == 2 && code4 == 2) {
    return 2;
  } else if (code1 == 1 && code2 == 4 && code3 == 1 && code4 == 1) {
    return 3;
  } else if (code1 == 1 && code2 == 1 && code3 == 3 && code4 == 2) {
    return 4;
  } else if (code1 == 1 && code2 == 2 && code3 == 3 && code4 == 1) {
    return 5;
  } else if (code1 == 1 && code2 == 1 && code3 == 1 && code4 == 4) {
    return 6;
  } else if (code1 == 1 && code2 == 3 && code3 == 1 && code4 == 2) {
    return 7;
  } else if (code1 == 1 && code2 == 2 && code3 == 1 && code4 == 3) {
    return 8;
  } else if (code1 == 3 && code2 == 1 && code3 == 1 && code4 == 2) {
    return 9;
  } else {
    return -1;
  }
}

void decodeImg(char *result) {
  rgbToGreyscale();
  thresholdImg();

  int y;
  int middle = X_SIZE / 2;
  int isBlack = 0, pixelCount = 0, init = 0;
  float bars[Y_SIZE] = {};
  int currBar = 0;

  for (y = 0; y < Y_SIZE; y ++) {
    if (init == 0) {
      if (greyImg[middle][y] == 0) {
        init = 1;
        isBlack = 1;
        pixelCount ++;
      }
    } else {
      if (greyImg[middle][y] != 0 && isBlack == 1) { // if was black but isn't black now, section is over, save pixelcount
        printf("y: %d, pixelcount: %d\n", y, pixelCount);
        bars[currBar ++] = pixelCount;
        pixelCount = 0;
        isBlack = 0;
      } else if (greyImg[middle][y] != 255 && isBlack == 0) { // if was white but isn't white now, section is over, save pixelcount
        printf("y: %d, pixelcount: %d\n", y, pixelCount);
        bars[currBar ++] = pixelCount;
        pixelCount = 0;
        isBlack = 1;
      }

      pixelCount ++;
    }
  }

  normalizeBars(bars, currBar);

  for (y = 0; y < currBar; y ++) {
    if (y % 4 == 2) {
      //printf("%f\n", bars[y]);
    } else {
      //printf("%f ", bars[y]);
    }
  }
  printf("\n");

  int currNum = 0;
  int offset = 3; // Start bars are 1 black, 1 white, 1 black, can skip
  result[12] = '\0';
  int error = 0;

  for (y = 0; y < 12; y ++) {
    printf("y: %d, offset: %d\n", y, offset);
    int code = processCode((int) bars[y * 4 + offset], (int) bars[y * 4 + 1 + offset], (int) bars[y * 4 + 2 + offset], (int) bars[y * 4 + 3 + offset]);
    printf("%d %d %d %d, code: %d\n", (int) bars[y * 4 + offset], (int) bars[y * 4 + 1 + offset], (int) bars[y * 4 + 2 + offset], (int) bars[y * 4 + 3 + offset], code);

    currNum ++;

    if (code != -1) {
      result[y] = (char) 48 + code;
      printf("result: %s\n", result);

      if (currNum == 6) { // Accounting for the middle deliniation on the barcode
        offset += 5;
      }
    } else {
      result[y] = '?';
      error = -1;
    }
  }
}
