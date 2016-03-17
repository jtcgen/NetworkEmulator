### PROJECT SETTINGS
# Name of executables
BIN_BRIDGE := bridge
BIN_STATION := station

# Compiler flags
CC := g++

# Additional compiler flags 
CFLAGS := -Wall -g -c 
LFLAGS := -Wall -g 

# Include paths
INCLUDES := -I./common/ -I./models/

# Path to source directory, relative to makefile
SRC_PATH = .

# Path to destination directory, relative to makefile
DST_PATH = .

# Find all source files 
COMMON_SRC := $(shell find -f -path ./common -name '*.cpp')
BRIDGE_SRC := $(shell find . -name 'main.cpp' -o -name 'bridge.cpp' -o -name 'ip.cpp') $(COMMON_SRC)

# Set object file names
B_OBJ := $(BRIDGE_SRC:%.cpp=%.o) 

#SRC := $(shell find . -name '*.cpp')
#OBJ := $(SRC:%.cpp=%.o)


### COMPILATION RULES
all: $(BIN_BRIDGE) $(BIN_STATION)

$(BIN_BRIDGE): $(B_OBJ) 
	$(CC) $(LFLAGS) $(B_OBJ) -o $@

$(BIN_STATION):
	echo 'station'

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean: 
	find . -type f -name '*.o' -exec rm {} +

realclean: clean
	rm $(BIN_BRIDGE)

