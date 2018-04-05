.PHONY: all install uninstall clean

CC = cc
CFLAGS = -c -Wall
TARGET = echoserver
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)
INSTALL_PATH = ~/bin

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)
	
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC)
	
install: $(TARGET)
	install -D -m 744 $(TARGET) $(INSTALL_PATH)/$(TARGET)
	make clean
		
uninstall: $(TARGET)
	rm -rf $(INSTALL_PATH)/$(TARGET)
	
clean:
	rm -rf $(TARGET) *.o 
