#ifndef __LINUX_V4L2_DEVICE_H_
#define __LINUX_V4L2_DEVICE_H_

#include <stdio.h>
#include <stdbool.h>
#include "jpeglib.h"

extern int lcd_fd;
extern int *lcd_ptr;

#define DEV_CAMERA0 "/dev/video0"
#define DEV_CAMERA7 "/dev/video7"
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

//摄像头设备文件的打开，需要填写对应的设备文件名称
int linux_v4l2_device_open(const char *pathname);
//摄像头关闭
void linux_v4l2_device_close();

struct buffer {
	void *start;    //数据容器
	size_t length;  //容器大小
};

//开启捕捉
int linux_v4l2_device_start_capturing();
//停止捕捉
void linux_v4l2_device_stop_capturing();

//摄像头初始化
int linux_v4l2_device_init(struct buffer *current_buffer);
//摄像头卸载
void linux_v4l2_device_uninit(struct buffer *current_buffer);

//获取摄像头采集到的数据
int linux_v4l2_device_get_frame(struct buffer * current_buffer);

/* * 如果摄像头格式是yuyv则需要使用yuyv2jpeg函数进行格式转换（yuyv转换jpeg）
   * JSAMPLE *yuyv_buffer : 需要转换的数据源，如current_buffer
   * long unsigned int length：数据源的大小
   * int quality：设置压缩比率值：0~100
   */

int yuyv2jpeg(unsigned char *yuyv_buffer, long unsigned int length, int quality);
bool yuyv_to_rgb888(unsigned char *yuv, unsigned char *rgb, int source_width, int source_height, int target_width, int target_height);
#endif
