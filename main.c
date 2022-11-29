#include "main.h"
#include "font.h"
#include "page.h"
#include "EdpKit.h"
#include "ConnectOneNet.h"

int lcd_fd, ts_fd, leds_fd, buzz_fd, sr04_fd, dht11_fd, light_fd;
int *lcd_ptr;
pthread_t tid1, tid2, camera_tid, rec_tid, loading_tid, ledw_tid, buzz_tid, sr04_tid, dht11_tid, light_tid;
int tflag = 0;
int x = 0, y = 0, ab_play = 0;
struct buffer jpeg_buf;
int LIGHT_sw = 1;

void showgifOnes()
{
    char buf[4096];
    for (int i = 0; i < 44; i++)
    {
        if (i >= 0 && i < 10)
        {
            sprintf(buf, "gifs/IMG0000%d.jpg", i);
        }
        else if (i >= 10 && i < 100)
        {
            sprintf(buf, "gifs/IMG000%d.jpg", i);
        }
        lcd_draw_jpeg(160, 100, buf, NULL, 0, 1);
        usleep(2000);
    }
}

void *OneNetReacv(void *arg)
{
    char RecvBuffer[10] = {0}; //保存接收的字符串

    while (1)
    {
        memset(RecvBuffer, 0, sizeof(RecvBuffer));
        OneNet_RecvData((void *)&sockfd, RecvBuffer);
        if (RecvBuffer[0] != 0)
        {
            printf("RecvBuffer:%s\n", RecvBuffer);
            if (strcmp(RecvBuffer, "LIGHT_ON"))
            {
                LIGHT_sw = 1;
                puts("light is on");
            }

            if (strcmp(RecvBuffer, "LIGHT_OFF"))
            {
                LIGHT_sw = 0;
                puts("light is off");
            }
        }
        sleep(3);
    }
}

int main(void)
{
    dev_init();
    OneNet_Init();
    pthread_t thread;
    pthread_create(&thread, NULL, OneNetReacv, NULL);
    showgifOnes();
    pthread_create(&ledw_tid, NULL, ledwater, NULL);
    puts("hello world!\n");
    menuPage();
    int t;
    while (1)
    {
        t = getxys(&x, &y);
        printf("(%d, %d)\n", x, y);
        if (t == 5 && isTouchArea(90, 176, 90 + 128, 304))
        {
            photoPage();
            menuPage();
        }
        else if (t == 5 && isTouchArea(254, 176, 254 + 128, 304))
        {
            // musicPage();
            statePage();
            menuPage();
        }
        else if (t == 5 && isTouchArea(418, 176, 418 + 128, 304))
        {
            photoPage();
            menuPage();
            int t = 1000000;
            pthread_create(&buzz_tid, NULL, bb, &t);
        }
        else if (t == 5 && isTouchArea(582, 176, 582 + 128, 304))
        {
            takePage();
            menuPage();
        }
        else if (t == 5 && isTouchArea(736, 416, 800, 480))
        {
            break;
        }
    }
    fontdraw("X", 200, 300, 100, 200, 200, 0x000000ff, 0);
    dev_uninit();
    /*关闭socket连接*/
    TcpClientClose();
    /* 等待线程退出 */
    pthread_join(thread, NULL);
    return 0;
}