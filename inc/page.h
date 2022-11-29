#ifndef __page_h__
#define __page_h__
#include "font.h"
#include "linux_v4l2_device.h"
#include <linux/input.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <setjmp.h>
int isTouchArea(int x1, int y1, int x2, int y2);
int isTouchCircle(int x1, int y1, int r);
void getxy(int *x, int *y);
int getxys(int *x, int *y);
void menuPage(void);
void photoPage(void);
void takePage(void);
void musicPage(void);
void statePage(void);
#endif