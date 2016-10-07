#for hackthon.
#this is p2
had bring mesh network in. Bring RF netwrok in. Bring log4cplus in. Bring libevent in. Bring Docker in. Bing ncurses in
lib needed:
        linux-vdso.so.1 (0x7eec7000)
        /usr/lib/arm-linux-gnueabihf/libarmmem.so (0x76f2d000)
        libncurses.so.5 => /lib/arm-linux-gnueabihf/libncurses.so.5 (0x76eed000)
        libtinfo.so.5 => /lib/arm-linux-gnueabihf/libtinfo.so.5 (0x76ebf000)
        librf24.so.1 => /usr/local/lib/librf24.so.1 (0x76ea4000)
        librf24network.so.1 => /usr/local/lib/librf24network.so.1 (0x76e8f000)
        librf24mesh.so.1 => /usr/local/lib/librf24mesh.so.1 (0x76e7c000)
        libevent-2.0.so.5 => /usr/local/lib/libevent-2.0.so.5 (0x76e2f000)
        libpthread.so.0 => /lib/arm-linux-gnueabihf/libpthread.so.0 (0x76e07000)
        liblog4cplus-1.1.so.9 => /usr/local/lib/liblog4cplus-1.1.so.9 (0x76d99000)
        libstdc++.so.6 => /usr/lib/arm-linux-gnueabihf/libstdc++.so.6 (0x76cbd000)
        libm.so.6 => /lib/arm-linux-gnueabihf/libm.so.6 (0x76c42000)
        libgcc_s.so.1 => /lib/arm-linux-gnueabihf/libgcc_s.so.1 (0x76c14000)
        libc.so.6 => /lib/arm-linux-gnueabihf/libc.so.6 (0x76ad3000)
        libdl.so.2 => /lib/arm-linux-gnueabihf/libdl.so.2 (0x76ac0000)
        /lib/ld-linux-armhf.so.3 (0x54af4000)

##build 
g++ -Ofast -fpermissive -mfpu=vfp -mfloat-abi=hard -march=armv7-a -mtune=arm1176jzf-s -Wall -I../ -lncurses -lrf24-bcm -lrf24network -lrf24mesh -levent -lpthread -llog4cplus hack_server.cpp -o hack_server
##add log4cplus
add -llog4cplus while link

#this is ph1

it is for rpi 1,rpi A+. docker image can find in pan.baidu.com. Will upload later(blocked by China gateway....)

##how to start the image
sudo docker run -p 25341:25341 --device /dev/ttyAMA0:/dev/ttyAMA0 --device /dev/mem:/dev/mem --privileged -ti imagename bash

#gateway side


##build dynamic lib -- librf24_wrap.so 
gcc  -fpic   -shared  rf24_wrap.cpp -o librf24_wrap.so
file needed:
rf24_wrap.cpp  rf24_wrap.h rf24_wrap_def.h

##build dynamic lib -- librf24-bcm.so
g++ -Wall -fPIC -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -c RF24.cpp

g++ -Wall -fPIC -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -c utility/RPi/spi.cpp

gcc -Wall -fPIC -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -c utility/RPi/bcm2835.c

g++ -Wall -fPIC -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -c utility/RPi/interrupt.c

g++ -shared -Wl,-soname,librf24-bcm.so.1 -pthread -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -o librf24-bcm.so.1.0 RF24.o spi.o bcm2835.o interrupt.o

file needed: RF24.cpp spi.cpp bcm2835.c interrupt.c

##build dynamic lib --libevent
download libevent-2.0.22-stable
sudo make&&make install

##build elf
gcc libevent.c -levent -lrf24_wrap -lrf24-bcm
file needed:
libevent.c

#client side

##build elf
g++ pingpair_dyn_int.cpp -lrf24-bcm
file needed:
pingpair_dyn_int.cpp



