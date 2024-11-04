CC = gcc
CFAGS = -Wall
SRCS = UpsideDown.cpp
LIBS = -lpng
TARGET = Change

$(TARGET) :
	$(CC) $(CFAGS) $(SRCS) $(LIBS) -o $(TARGET)

clean: 
	rm $(TARGET)
