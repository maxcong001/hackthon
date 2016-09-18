#for hackthon.

it is for rpi 1,rpi A+. docker image can find in pan.baidu.com. Will upload later(blocked by China gateway....)

##how to start the image
sudo docker run -p 25341:25341 --device /dev/ttyAMA0:/dev/ttyAMA0 --device /dev/mem:/dev/mem --privileged -ti imagename bash

#gateway side


##build dynamic lib -- librf24_wrap.so 
g++  -fpic   -shared  rf24_wrap.cpp -o librf24_wrap.so
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
