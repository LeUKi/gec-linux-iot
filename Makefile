all: edp

CC=arm-linux-gcc
CFLAGS= -D_LINUX -lm -pthread -I ./inc -L ./lib  -I /home/gec/Downloads/jpeg2arm/include/ -L /home/gec/Downloads/jpeg2arm/lib -ljpeg -lpng -lfont -lv4l2_device_api -I.
CLIENT_OBJ = cJSON.o EdpKit.o main.o ConnectOneNet.o font.o page.o

# 如果需要加密功能且系统内已经安装openssl，
# 取消以下两行注释 
#CFLAGS+=-D_ENCRYPT -lcrypto
#CLIENT_OBJ += Openssl.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

edp: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f edp $(CLIENT_OBJ)
