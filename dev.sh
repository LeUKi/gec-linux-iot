arm-linux-gnueabi-gcc main.c font.c page.c -o main -I /home/gec/Downloads/jpeg2arm/include/ -L /home/gec/Downloads/jpeg2arm/lib -ljpeg -lpng -lfont -lpthread -lm -lv4l2_device_api -I ./inc -L ./lib