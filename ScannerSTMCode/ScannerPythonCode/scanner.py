#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import math
import os
import random as rand
import matplotlib.pyplot as plt
import matplotlib.image as img
import cv2
from mpl_toolkits.mplot3d import Axes3D
from PIL import Image
get_ipython().run_line_magic('matplotlib', 'inline')


# In[2]:


train_img = "W:/Documents/College/ENEE408A/imgs/sample-barcode4.jpg"
image = cv2.imread(train_img) # Load an image as numpy array
greyImage = cv2.imread(train_img, cv2.IMREAD_GRAYSCALE)
print(image.shape)
##for x in range(image.shape[0]):
##    print('{')
##    for y in range(image.shape[1]):
##        if (y < image.shape[1] - 1):
##            print("%d," % (image[x][y]), end =" ")
##        else:
##            print("%d},"% (image[x][y]), end =" ")
plt.gray()
plt.imshow(image)


# In[136]:


def thresholdImg():
    for x in range(0, image.shape[0]):
        for y in range(0, image.shape[1]):
            if (image[x][y] < 180):
                image[x][y] = 0;
            else:
                image[x][y] = 255;
                
#thresholdImg()
plt.gray()
plt.imshow(image)

def eq(slope, y, centerX, centerY):
    return int((slope * (y - centerY)) + centerX)

center = [int(image.shape[0] / 2), int(image.shape[1] / 2)]
print(center)
for theta in range(0, 180, 2):
    for y in range(0, image.shape[1]):
        ##print(eq(math.sin(math.radians(theta)), x, center[0], center[1]))
        newX = eq(math.sin(math.radians(theta)) / math.cos(math.radians(theta)), y, center[0], center[1])
        if (newX < image.shape[0] and newX >= 0 and y < image.shape[1] / 20):
            if (theta < 60):
                image[newX][y][0] = 255
                image[newX][y][1] = 0
                image[newX][y][2] = 0
            elif (theta < 120):
                image[newX][y][0] = 0
                image[newX][y][1] = 255
                image[newX][y][2] = 0
            elif (theta <= 180):
                image[newX][y][0] = 0
                image[newX][y][1] = 0
                image[newX][y][2] = 255
        #else:
            #print("x: %d, y: %d" % (x, newY))
        
plt.gray()
plt.imshow(image)
    


# In[3]:


def thresholdGreyImg(threshold):
    for x in range(0, greyImage.shape[0]):
        for y in range(0, greyImage.shape[1]):
            if (greyImage[x][y] < threshold):
                greyImage[x][y] = 0;
            else:
                greyImage[x][y] = 255;
                
thresholdGreyImg(195)
plt.gray()
plt.imshow(greyImage)

def eq(slope, y, centerX, centerY):
    return int((slope * (y - centerY)) + centerX)

center = [int(image.shape[0] / 2), int(image.shape[1] / 2)]
edgeX = [image.shape[0], 0]
edgeY = [image.shape[1], 0]
print(center)

for theta in range(0, 180, 2):
    isWhite = 0
    
    for y in range(0, image.shape[1]):
        ##print(eq(math.sin(math.radians(theta)), x, center[0], center[1]))
        newX = eq(math.sin(math.radians(theta)) / math.cos(math.radians(theta)), y, center[0], center[1])
        if (newX < image.shape[0] and newX >= 0):
            if (greyImage[newX][y] == 255):
                isWhite += 1
                
                if (isWhite >= 35):
                    print("x: %d, y: %d" % (newX, y))

                    if (newX < center[0]):
                        if (edgeX[0] > newX):
                            edgeX[0] = newX
                    else:
                        if (edgeX[1] < newX):
                            edgeX[1] = newX
                    
                    if (y < center[1]):
                        if (edgeY[0] > y):
                            edgeY[0] = y
                    else:
                        if (edgeY[1] < y):
                            edgeY[1] = y
                    break
            else:
                isWhite = 0

print(edgeX)
print(edgeY)
for y in range(0, (edgeY[1] - 16) - (edgeY[0] + 16)):
    newY = min(y + edgeY[0], image.shape[1] - 1)
    image[int((edgeX[1] - edgeX[0]) / 2)][newY][0] = 255
    image[int((edgeX[1] - edgeX[0]) / 2)][newY][1] = 0
    image[int((edgeX[1] - edgeX[0]) / 2)][newY][2] = 0
    image[int((edgeX[1] - edgeX[0]) / 2) - 1][newY][0] = 255
    image[int((edgeX[1] - edgeX[0]) / 2) - 1][newY][1] = 0
    image[int((edgeX[1] - edgeX[0]) / 2) - 1][newY][2] = 0
    image[int((edgeX[1] - edgeX[0]) / 2) + 1][newY][0] = 255
    image[int((edgeX[1] - edgeX[0]) / 2) + 1][newY][1] = 0
    image[int((edgeX[1] - edgeX[0]) / 2) + 1][newY][2] = 0
plt.figure()        
plt.gray()
plt.imshow(image)
    


# In[10]:


def thresholdGreyImg(threshold):
    for x in range(0, greyImage.shape[0]):
        for y in range(0, greyImage.shape[1]):
            if (greyImage[x][y] < threshold):
                greyImage[x][y] = 0;
            else:
                greyImage[x][y] = 255;

def processCode(code1, code2, code3, code4):
    if (code1 == 3 and code2 == 2 and code3 == 1 and code4 == 1):
        return 0
    elif (code1 == 2 and code2 == 2 and code3 == 2 and code4 == 1):
        return 1
    elif (code1 == 2 and code2 == 1 and code3 == 2 and code4 == 2):
        return 2 
    elif (code1 == 1 and code2 == 4 and code3 == 1 and code4 == 1):
        return 3
    elif (code1 == 1 and code2 == 1 and code3 == 3 and code4 == 2):
        return 4
    elif (code1 == 1 and code2 == 2 and code3 == 3 and code4 == 1):
        return 5
    elif (code1 == 1 and code2 == 1 and code3 == 1 and code4 == 4):
        return 6
    elif (code1 == 1 and code2 == 3 and code3 == 1 and code4 == 2):
        return 7
    elif (code1 == 1 and code2 == 2 and code3 == 1 and code4 == 3):
        return 8
    elif (code1 == 3 and code2 == 1 and code3 == 1 and code4 == 2):
        return 9 
    else:
        return -1

def normalizeBars(maxBars):
    avg = (bars[0] + bars[1] + bars[2]) / 3.0
    diff = 3.0 - avg
    print("avg: %f" % (avg))
    for i in range(0, maxBars):
        bars[i] /= avg
        if (np.round(bars[i]) == 0):
            bars[i] = 0.5
        
        if (i % 4 == 2):
            print("%f" % (bars[i]))
        else:
            print("%f" % (bars[i]), end =" ")
        #bars[i] = np.round(bars[i] + diff)
    print()
    
    offset = 3
    
    for i in range(0, 12):
        if (i == 6):
            #bars[i * 4 + offset + 0] = np.round(bars[i * 4 + offset + 0])
            #bars[i * 4 + offset +1] = np.round(bars[i * 4 + offset + 1])
            offset += 5
        
        if (np.round(bars[i * 4 + offset]) + np.round(bars[i * 4 + offset + 1]) + np.round(bars[i * 4 + offset + 2]) + np.round(bars[i * 4 + offset + 3]) == 7):
            for n in range(0,4):
                bars[i * 4 + offset + n] = np.round(bars[i * 4 + offset + n])
        elif (np.round(bars[i * 4 + offset] + diff) + np.round(bars[i * 4 + offset + 1] + diff) + np.round(bars[i * 4 + offset + 2] + diff) + np.round(bars[i * 4 + offset + 3] + diff) == 7):
            for n in range(0,4):
                bars[i * 4 + offset + n] = np.round(bars[i * 4 + offset + n] + diff)
        elif (np.round(bars[i * 4 + offset] - diff) + np.round(bars[i * 4 + offset + 1] - diff) + np.round(bars[i * 4 + offset + 2] - diff) + np.round(bars[i * 4 + offset + 3] - diff) == 7):
            for n in range(0,4):
                bars[i * 4 + offset + n] = np.round(bars[i * 4 + offset + n] - diff)
    
    for i in range(0, maxBars):
        bars[i] = np.round(bars[i])


# In[11]:


thresholdGreyImg(170)
plt.gray()
plt.imshow(greyImage)

init = 0
#middle = int(greyImage.shape[0] / 2)
middle = int((edgeX[1] - edgeX[0]) / 2)
pixelCount = 0
bars = np.zeros(greyImage.shape[1])
currBar = 0

#for y in range(0, greyImage.shape[1]):
for y in range(edgeY[0], edgeY[1]):
    if (init == 0):
        if (greyImage[middle][y] == 0):
            init = 1
            isBlack = 1
            pixelCount += 1
    else:
        if (greyImage[middle][y] != 0 and isBlack == 1):
            #print("y: %d, pixelcount: %d" % (y, pixelCount))
            bars[currBar] = pixelCount
            currBar += 1
            pixelCount = 0
            isBlack = 0
        elif (greyImage[middle][y] != 255 and isBlack == 0):
            #print("y: %d, pixelcount: %d" % (y, pixelCount))
            bars[currBar] = pixelCount
            currBar += 1
            pixelCount = 0
            isBlack = 1
        
        pixelCount += 1

normalizeBars(currBar)

for bar in range(currBar):
    if (bar % 4 == 2):
        print("%f" % (bars[bar]))
    else:
        print("%f" % (bars[bar]), end =" ")
print()

currNum = 0
offset = 3
result = ""
error = 0

for i in range(0, 12):
    print("i: %d, offset: %d" % (i, offset))
    code = processCode(int(bars[i * 4 + offset]), int(bars[i * 4 + 1 + offset]), int(bars[i * 4 + 2 + offset]),
                                     int(bars[i * 4 + 3 + offset]))
    print("%d %d %d %d, code: %d" % (int(bars[i * 4 + offset]), int(bars[i * 4 + 1 + offset]), int(bars[i * 4 + 2 + offset]),
                                     int(bars[i * 4 + 3 + offset]), code))
    currNum += 1
    
    if (code != -1):
        result += chr(48 + code)
        print("result: %s" % (result))
        
        if (currNum == 6):
            offset += 5
    else:
        result += "?"
        error = -1


# In[29]:


thresholdGreyImg(170)
plt.gray()
plt.imshow(greyImage)

init = 0
#middle = int(greyImage.shape[0] / 2)
middle = int((edgeX[1] - edgeX[0]) / 2)
pixelCount = 0
bars = np.zeros(greyImage.shape[1])
currBar = 0

#for y in range(0, greyImage.shape[1]):
for y in range(edgeY[0], greyImage.shape[1]):
    # Chunk labels:
    # _ _#1#
    # #2#_ _
    
    chunk1 = greyImage[middle][y]
    chunk2 = greyImage[middle - 1][y]
    chunk3 = greyImage[middle - 2][y]
    print("%d %d %d" % (chunk1, chunk2, chunk3))
    weight = (int(chunk1) + int(chunk2) + int(chunk3))
    print(weight)
    
    # keep checking until black is found
    if (init == 0):
        if (weight < 256):
            init = 1
            isBlack = 1
            pixelCount += 1
    else:
        if (chunk1 == chunk2): 
            if (chunk1 != 0 and isBlack == 1):
                #print("y: %d, pixelcount: %d" % (y, pixelCount))
                bars[currBar] = pixelCount
                currBar += 1
                pixelCount = 0
                isBlack = 0
            elif (chunk1 != 255 and isBlack == 0):
                #print("y: %d, pixelcount: %d" % (y, pixelCount))
                bars[currBar] = pixelCount
                currBar += 1
                pixelCount = 0
                isBlack = 1
        pixelCount += 1

normalizeBars(currBar)

for bar in range(currBar):
    if (bar % 4 == 2):
        print("%f" % (bars[bar]))
    else:
        print("%f" % (bars[bar]), end =" ")
print()

currNum = 0
offset = 3
result = ""
error = 0

for i in range(0, 12):
    print("i: %d, offset: %d" % (i, offset))
    code = processCode(int(bars[i * 4 + offset]), int(bars[i * 4 + 1 + offset]), int(bars[i * 4 + 2 + offset]),
                                     int(bars[i * 4 + 3 + offset]))
    print("%d %d %d %d, code: %d" % (int(bars[i * 4 + offset]), int(bars[i * 4 + 1 + offset]), int(bars[i * 4 + 2 + offset]),
                                     int(bars[i * 4 + 3 + offset]), code))
    currNum += 1
    
    if (code != -1):
        result += chr(48 + code)
        print("result: %s" % (result))
        
        if (currNum == 6):
            offset += 5
    else:
        result += "?"
        error = -1
        print("result: %s" % (result))


# In[ ]:




