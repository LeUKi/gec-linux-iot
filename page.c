#include "page.h"
extern int tflag;
extern int x, y, ab_play;
extern int lcd_fd, ts_fd, sr04_fd, dht11_fd, light_fd;
extern int *lcd_ptr;
extern pthread_t tid1, tid2, camera_tid, rec_tid, loading_tid, sr04_tid, dht11_tid, light_tid;
extern struct buffer jpeg_buf;

int isTouchArea(int x1, int y1, int x2, int y2)
{
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
    {
        return 1;
    }
    return 0;
}
int isTouchCircle(int x1, int y1, int r)
{
    if ((x - x1) * (x - x1) + (y - y1) * (y - y1) <= r * r)
    {
        return 1;
    }
    return 0;
}
void getxy(int *x, int *y)
{
    struct input_event ts;
    while (1)
    {
        read(ts_fd, &ts, sizeof(ts));

        if (ts.type == EV_ABS && ts.code == ABS_X)
            *x = ts.value / 1024.0 * 800;

        if (ts.type == EV_ABS && ts.code == ABS_Y)
            *y = ts.value / 600.0 * 480;

        if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
            ;

        if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 1)
            return;
    }
}
int getxys(int *x, int *y)
{
    //获取设备的信息 (x, y) (y, y) (y, x) (x, x)
    int x_ready = 0, y_ready = 1;
    int x1 = -1, y1 = -1;
    int x2, y2;
    struct input_event ts_buf;

    while (1)
    {
        read(ts_fd, &ts_buf, sizeof(ts_buf));
        //同步时，所有为0，跳过
        if (ts_buf.type == EV_SYN)
        {
            continue;
        }

        if (ts_buf.type == EV_KEY)
        {
            if (ts_buf.code == BTN_TOUCH)
            {
                if (ts_buf.value == KEY_RESERVED)
                {
                    int xc = x2 - x1;
                    int yc = y2 - y1;

                    int abs_x = abs(xc);
                    int abs_y = abs(yc);

                    if (xc > 30 && abs_x > abs_y * 2)
                    {
                        return 1; //右划
                    }
                    else if (xc < -30 && abs_x > abs_y * 2)
                    {
                        return 2; //左划
                    }
                    else if (yc > 30 && abs_y > abs_x * 2)
                    {
                        return 3; //下划
                    }
                    else if (yc < -30 && abs_y > abs_x * 2)
                    {
                        return 4; //上划
                    }
                    else
                        return 5; //点击
                }
            }
        }
        if ("1024" == "1024")
        {
            if (ts_buf.type == EV_ABS && ts_buf.code == ABS_X)
            {
                if (x1 == -1)
                {
                    x1 = ts_buf.value * 800 / 1024;
                }
                x2 = ts_buf.value * 800 / 1024;
                *x = ts_buf.value * 800 / 1024;
                x_ready = 1;
                y_ready = 0; //保证x坐标先输出
            }
            else if (ts_buf.type == EV_ABS && ts_buf.code == ABS_Y)
            {
                if (y1 == -1)
                {
                    y1 = ts_buf.value * 480 / 600;
                }
                y2 = ts_buf.value * 480 / 600;
                *y = ts_buf.value * 480 / 600;
                x_ready = 0;
                y_ready = 1;
            }
            if (x_ready == 1 && y_ready == 1)
                break;
        }
        else
        {
            if (ts_buf.type == EV_ABS && ts_buf.code == ABS_X)
            {
                if (x1 == -1)
                {
                    x1 = ts_buf.value;
                    // * 800 / 1024;
                }
                x2 = ts_buf.value;
                // * 800 / 1024;
                *x = ts_buf.value;
                // * 800 / 1024;
                x_ready = 1;
                y_ready = 0; //保证x坐标先输出
            }
            else if (ts_buf.type == EV_ABS && ts_buf.code == ABS_Y)
            {
                if (y1 == -1)
                {
                    y1 = ts_buf.value;
                    // * 480 / 600;
                }
                y2 = ts_buf.value;
                // * 480 / 600;
                *y = ts_buf.value;
                // * 480 / 600;
                x_ready = 0;
                y_ready = 1;
            }
            if (x_ready == 1 && y_ready == 1)
                break;
        }
    }

    return 0;
}
void *showgifRec()
{
    tflag = 1;
    char buf[4096];
    int add = 1;
    int time = 0;
    int i = 0;
    while (1)
    {
        time++;
        pthread_testcancel();

        if (i == 43)
            add = -1;
        if (i == 0)
            add = 1;

        if (time == 500)
        {
            time = 0;
            if (i >= 0 && i < 10)
            {
                sprintf(buf, "gifs/IMG0000%d.jpg", i);
            }
            else if (i >= 10 && i < 100)
            {
                sprintf(buf, "gifs/IMG000%d.jpg", i);
            }
            lcd_draw_jpeg(160, 100, buf, NULL, 0, 1);

            i += add;
        }
    }
    return NULL;
}
void menuPage()
{
    lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
    pngdraw("bg/photo.png", 90, 176);
    pngdraw("bg/state.png", 254, 176);
    pngdraw("bg/play.png", 418, 176);
    pngdraw("bg/take.png", 582, 176);
    pngdraw("bg/exit0.png", 736, 416);
}
void photoShow(char *pathname, int i, int len)
{

    lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
    lcd_draw_jpeg(0, 0, pathname, NULL, 0, 1);
    pngdraw("bg/back.png", 10, 408);
    pngdraw("bg/up0.png", 720, 324);
    pngdraw("bg/down0.png", 720, 408);
    char cbuf[4096];
    sprintf(cbuf, "%d/%d", i + 1, len);
    fontdraw(cbuf, 20, 664, 430, 64, 20, 0, 0xffffffff);
    puts(pathname);
}
void abplayinit(void)
{
    if (ab_play == 1)
    {
    }
}
void abShow(char *buf, int i, int len)
{
    system("killall -9  mplayer");
    if (!strncmp(buf + (strlen(buf) - strlen(".jpg")), ".jpg", strlen(".jpg")))
    {
        lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
        lcd_draw_jpeg(0, 0, buf, NULL, 0, 1);
        ab_play = 0;
    }
    else if (!strncmp(buf + (strlen(buf) - strlen(".avi")), ".avi", strlen(".avi")))
    {
        lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
        pngdraw("bg/play0.png", 368, 408);
        ab_play = 1;
    }
    pngdraw("bg/back.png", 10, 408);
    pngdraw("bg/del0.png", 480, 408);
    pngdraw("bg/left0.png", 600, 408);
    pngdraw("bg/right0.png", 720, 408);
    char cbuf[4096];
    sprintf(cbuf, "%d/%d", i + 1, len);
    fontdraw(cbuf, 20, 664, 430, 56, 20, 0, 0xffffffff);
    puts(buf);
}
void photoPage()
{
    // char(*jpg_path)[4096] = calloc(1024, 4096);
    char(*ab_path)[4096] = calloc(1024, 4096);
    // int jpg_len = 0;
    int ab_len = 0;
    // read_dir("ps", jpg_path, ".jpg", &jpg_len);
    read_ab_dir("ps", ab_path, &ab_len);

    int t, i;

    if (ab_len == 0)
    {
        printf("no jpg file\n");
        lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
        pngdraw("bg/back.png", 10, 408);
        while (1)
        {
            t = getxys(&x, &y);
            if (t == 5 && isTouchArea(0, 400, 90, 480))
            {
                break;
            }
        }
        return;
    }
    i = ab_len - 1;
    abShow(ab_path[i], i, ab_len);

    while (1)
    {
        t = getxys(&x, &y);
        printf("(%d,%d)\n", x, y);
        printf("%d/%d\n", i, ab_len);
        if (t == 5 && isTouchArea(0, 400, 90, 480))
        {
            system("killall -9  mplayer");
            break;
        }
        if (t == 5 && isTouchArea(480, 408, 480 + 64, 408 + 64))
        {
            remove(ab_path[i]);
            if (i != 0)
            {
                i--;
            }
            ab_len = 0;
            read_ab_dir("ps", ab_path, &ab_len);
            if (ab_len == 0)
                break;

            abShow(ab_path[i], i, ab_len);
        }
        else if (
            (t == 5 && isTouchArea(600, 408, 600 + 64, 408 + 64)) || t == 1 || t == 3)
        {
            puts("up");
            if (i == 0)
                i = ab_len - 1;
            else
                i--;
            abShow(ab_path[i], i, ab_len);
        }
        else if ((t == 5 && isTouchArea(720, 408, 720 + 64, 408 + 64)) || t == 2 || t == 4)
        {
            puts("down");
            if (i == ab_len - 1)
                i = 0;
            else
                i++;
            abShow(ab_path[i], i, ab_len);
        }
        else if ((t == 5 && isTouchArea(368, 408, 368 + 64, 408 + 64)))
        {
            if (!strncmp(ab_path[i] + (strlen(ab_path[i]) - strlen(".avi")), ".avi", strlen(".avi")))
            {
                char play_cmd[4096];
                sprintf(play_cmd, "mplayer  %s  -slave -quiet -input file=/tmp/fifo  -zoom -x 800 -y 400 -geometry 0:0 -vo fbdev2 -vf scale -ac mad -cache 8192 &", ab_path[i]);
                system(play_cmd);
            }
        }
    }
    // menuPage();
}
void takePageMenu()
{
    lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
    pngdraw("bg/back.png", 688, 42);
    pngdraw("bg/ok0.png", 688, 148);
    pngdraw("bg/start0.png", 688, 254);
    pngdraw("bg/photo.png", 656, 344);
    // pngdraw("bg/c0.png", 688, 254);
}
void takePage()
{
    int t = -1;
    takePageMenu();
    puts("takePage");
    linux_v4l2_device_start_capturing();
    pthread_create(&camera_tid, NULL, read_camera_data, NULL);
    puts("takePage2");
    int recing = 0;
    while (1)
    {
        t = getxys(&x, &y);

        if (t == 5 && isTouchArea(688, 42, 688 + 64, 42 + 64))
        {
            pthread_cancel(camera_tid);
            pthread_join(camera_tid, NULL);
            linux_v4l2_device_stop_capturing();
            break;
        }
        if (t == 5 && isTouchArea(688, 148, 688 + 64, 148 + 64))
        {
            linux_v4l2_device_stop_capturing();
            pthread_create(&loading_tid, NULL, wait_makeavi, NULL);
            snapcamera();
            // usleep(1000*1000);
            lcd_draw_jpeg(680, 380, NULL, jpeg_buf.start, jpeg_buf.length, 8);
            linux_v4l2_device_start_capturing();
            pthread_cancel(loading_tid);
            pthread_join(loading_tid, NULL);
        }
        if (t == 5 && isTouchArea(640, 360, 800, 480))
        {
            pthread_cancel(camera_tid);
            pthread_join(camera_tid, NULL);
            linux_v4l2_device_stop_capturing();
            photoPage();
            linux_v4l2_device_start_capturing();
            takePageMenu();
            pthread_create(&camera_tid, NULL, read_camera_data, NULL);
        }
        if (t == 5 && isTouchArea(688, 254, 688 + 64, 254 + 64))
        {
            if (recing == 0)
            {

                pthread_create(&rec_tid, NULL, record_jpg, NULL);
                recing = 1;
                pngdraw("bg/c0.png", 688, 254);
            }
            else if (recing == 1)
            {
                pthread_cancel(rec_tid);
                pthread_join(rec_tid, NULL);
                linux_v4l2_device_stop_capturing();
                // pthread_cancel(camera_tid);
                // pthread_join(camera_tid, NULL);
                pthread_create(&loading_tid, NULL, wait_makeavi, NULL);
                lcd_draw_jpeg(680, 380, NULL, jpeg_buf.start, jpeg_buf.length, 8);
                sleep(1);
                char command_buf1[4096] = "ffmpeg -f image2 -i tmp/%d.jpg -r 25 -y ";
                char command_buf2[4096];
                char av_name1[256] = {0};
                gettime(av_name1);
                sprintf(command_buf2, "%s ps/%s.avi", command_buf1, av_name1);
                system(command_buf2);
                recing = 0;
                // pthread_create(&camera_tid, NULL, read_camera_data, NULL);
                sleep(1);
                pthread_cancel(loading_tid);
                pthread_join(loading_tid, NULL);
                linux_v4l2_device_start_capturing();
                pngdraw("bg/start0.png", 688, 254);
            }
        }
    }

    // menuPage();
}

void musicPage()
{
    lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
    pngdraw("bg/back.png", 10, 10);
    pthread_create(&tid1, NULL, showgifRec, NULL);

    while (1)
    {
        int t = getxys(&x, &y);

        if (t == 5 && isTouchArea(10, 10, 74, 74))
        {
            pthread_cancel(tid1);
            pthread_join(tid1, NULL);
            break;
        }
    }
    // menuPage();
}

void statePage()
{
    lcd_draw_jpeg(0, 0, "bg/bg.jpg", NULL, 0, 1);
    pngdraw("bg/back.png", 10, 10);
    pthread_create(&tid1, NULL, showgifRec, NULL);
    if (sr04_fd = open("/dev/sr04_drv", O_RDONLY))
        pthread_create(&sr04_tid, NULL, getenv_sr04, NULL);
    if (dht11_fd = open("/dev/dht11_dev", O_RDONLY))
        pthread_create(&dht11_tid, NULL, getenv_dht11, NULL);
    if (light_fd = open("/dev/light_drv", O_RDONLY))
        pthread_create(&light_tid, NULL, getenv_light, NULL);

    while (1)
    {
        int t = getxys(&x, &y);

        if (t == 5 && isTouchArea(10, 10, 74, 74))
        {
            pthread_cancel(tid1);
            pthread_join(tid1, NULL);
            pthread_cancel(sr04_tid);
            pthread_join(sr04_tid, NULL);
            pthread_cancel(dht11_tid);
            pthread_join(dht11_tid, NULL);
            pthread_cancel(light_tid);
            pthread_join(light_tid, NULL);
            break;
        }
    }
    close(sr04_fd);
    close(dht11_fd);
    close(light_fd);
}