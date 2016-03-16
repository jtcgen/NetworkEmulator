CC     = g++
CFLAGS = -g
INCLUDES = -I./common/ -I./models/ 
CFLAGS = -Wall -g -c 
B_OBJS = models/bridge.o models/w_socket.o models/utility.o

all: bridge 

bridge: $(B_OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(B_OBJS) -o bridge

models/bridge.o: models/bridge.hpp models/bridge.cpp common/w_socket.hpp common/utility.hpp models/clientdata.hpp models/ip.hpp models/ether.hpp
	$(CC) $(CFLAGS) $(INCLUDES) bridge.cpp

common/w_socket.o: common/w_socket.hpp common/w_socket.cpp common/utility.hpp
	$(CC) $(CFLAGS) $(INCLUDES) w_socket.cpp

common/utility.o: common/utility.hpp common/utility.cpp
	$(CC) $(CFLAGS) $(INCLUDES) utility.cpp

clean : 
	rm -f bridge *.o

