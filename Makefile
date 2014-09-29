
CC := gcc

TARGET := my_shell
OBJECT := my_shell.o
SOURCE := my_shell.c

all: $(TARGET)

$(TARGET) : $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) $(OBJECT)
