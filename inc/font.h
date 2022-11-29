#ifndef __font_h__
#define __font_h__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <sys/mman.h>
#include "jpeglib.h" //开发者指定头文件路径
#include <setjmp.h>
#include "png_show.h"
#include "linux_v4l2_device.h"
#include <sys/ioctl.h>
#include <memory.h>

#define MAX_KB 1024
#define MAXN 1024 * MAX_KB
#define UCHAR unsigned char
#define GEC6818_GET_DHTDATA _IOR('K', 0, unsigned int) //注册宏

#define color u32

#define getColor(a, b, c, d) (a | b << 8 | c << 16 | d << 24)

//定义幻数
#define LEDS_MAGIC 'l'
//定义命令的最大序数
#define LEDS_MAX_NR 4

//定义LED的魔幻数
#define LED1 _IO(LEDS_MAGIC, 0)
#define LED2 _IO(LEDS_MAGIC, 1)
#define LED3 _IO(LEDS_MAGIC, 2)
#define LED4 _IO(LEDS_MAGIC, 3)

#define LED_ON 0
#define LED_OFF 1

#define DEVICE_BUZZ "/dev/buzz_misc"

#define BUZZ_MAGIC 'b'

#define BUZZ_ON _IOW(BUZZ_MAGIC, 1, unsigned long)
#define BUZZ_OFF _IOW(BUZZ_MAGIC, 0, unsigned long)

#define DEVCIE_LEDS "/dev/leds_misc"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

extern int *lcd_ptr;

typedef struct stbtt_fontinfo
{
	void *userdata;
	unsigned char *data; // pointer to .ttf file
	int fontstart;		 // offset of start of font

	int numGlyphs; // number of glyphs, needed for range checking

	int loca, head, glyf, hhea, hmtx, kern; // table locations as offset from start of .ttf
	int index_map;							// a cmap mapping for our chosen character encoding
	int indexToLocFormat;					// format needed to map from glyph index to glyph
} stbtt_fontinfo;

typedef struct
{
	u32 height;
	u32 width;
	u32 byteperpixel;
	u8 *map;
} bitmap;

typedef struct
{
	stbtt_fontinfo *info;
	u8 *buffer;
	float scale;
} font;

// lcd设备结构体
struct LcdDevice
{
	int fd;
	unsigned int *mp; //保存映射首地址
};
void send_cmd(char *cmd);
void mplayer_init();

// 1.初始化字库
font *fontLoad(char *fontPath);

// 2.设置字体的大小
void fontSetSize(font *f, s32 pixels);

// 3.设置字体输出框的大小
bitmap *createBitmap(u32 width, u32 height, u32 byteperpixel);

//可以指定输出框的颜色
bitmap *createBitmapWithInit(u32 width, u32 height, u32 byteperpixel, color c);

// 4,把字体输出到输出框中
void fontPrint(font *f, bitmap *screen, s32 x, s32 y, char *text, color c, s32 maxWidth);

// 5,把输出框的所有信息显示到LCD屏幕中
void show_font_to_lcd(unsigned int *p, int px, int py, bitmap *bm);

// 6,关闭字体库
void fontUnload(font *f);

// 7,关闭bitmap
void destroyBitmap(bitmap *bm);

struct LcdDevice *init_lcd(const char *device);
void font_init(int num);
void create_map(int W, int H, unsigned int mapcolor);
void create_font(int X, int Y, char *buf, unsigned int fontcolor);

extern struct LcdDevice *lcd; // lcd屏幕指针
extern font *f;				  //字体指针
extern bitmap *bm;			  //内存映射指针
extern char font_str[64];
int dev_init(void);
void dev_uninit(void);
int lcd_draw_jpeg(int x, int y, char *pathname, char *jpeg_buf, int jpeg_size, int zoom_flag);
int fontdraw(char *str, s32 s, u32 x, u32 y, int w, int h, color c, color bgc);
void pngdraw(char *pathname, u32 x, u32 y);
void *read_camera_data(void *arg);
void *record_jpg(void *arg);
void *wait_makeavi(void *arg);
void snapcamera(void);
int read_dir(const char *pathname, char (*files)[4096], char *type, int *len);
int read_ab_dir(const char *pathname, char (*files)[4096], int *len);
int file_exist(const char *path);
int dev_exist(const char *path);
void *ledwater(void *arg);
void gettime(char *buf);
void *bb(void *arg);
void *getenv_sr04(void *arg);
void *getenv_dht11(void *arg);
void *getenv_light(void *arg);
void send_onenet(char *tag, int num);
void base64Encode(UCHAR *, int, char *);
char *parseBase64(const char *);
#endif