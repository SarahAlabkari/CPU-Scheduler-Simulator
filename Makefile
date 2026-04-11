CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = src/main.c src/parser.c src/simulator.c src/output.c src/metrics.c
TARGET = scheduler

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
