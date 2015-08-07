ROOT_PATH=$(shell pwd )

CLIENT_PATH=$(ROOT_PATH)/udp_client
COMM_PATH=$(ROOT_PATH)/comm
DATA_POOL_PATH=$(ROOT_PATH)/data_pool
LIB_PATH=$(ROOT_PATH)/lib
LOG_PATH=$(ROOT_PATH)/log
SERVER_PATH=$(ROOT_PATH)/udp_server

CC=g++
CFLAGS=-g -Wall
LIB= -lpthread -ljsoncpp
LDFLAGS=#-static
INCLUDE=-I$(CLIENT_PATH) -I$(COMM_PATH) -I$(DATA_POOL_PATH) \
		-I$(LIB_PATH) -I$(LOG_PATH) -I$(SERVER_PATH)

SRC=$(shell ls -R | egrep '*.cpp')
OBJ=$(SRC:.cpp=.o)
CLI_OBJ=$(shell echo $(OBJ) | sed 's/server.o//' | sed 's/data_pool.o//')
SER_OBJ=$(shell echo $(OBJ) | sed 's/client.o//')
CLI_BIN=client
SER_BIN=server


.PHONY:all
all:$(CLI_BIN) $(SER_BIN)

$(CLI_BIN):$(CLI_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIB)
$(SER_BIN):$(SER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIB)

%.o:$(COMM_PATH)/%.cpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE)  
%.o:$(DATA_POOL_PATH)/%.cpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE)  
%.o:$(SERVER_PATH)/%.cpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE)  
%.o:$(CLIENT_PATH)/%.cpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE)  


clean:
	rm -rf *.o $(CLI_BIN) $(SER_BIN) output

output:all
.PHONY:output
output:
	mkdir -p output/server
	mkdir -p output/client
	cp -R plugin/server_ctl.sh outpout
	cp -R server output/server
	cp -R client output/client
	cp -R conf 	 output/server
	cp -R conf 	 output/client
	




