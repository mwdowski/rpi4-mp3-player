.PHONY := all clean
EXEC_NAME := player
CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Wno-unknown-pragmas -g3
LIBS := -lgpiod

$(EXEC_NAME): main.c my_gpiod.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(LIBS)

my_gpiod.o: my_gpiod.c 
	$(CC) -c $^ -o $@ $(CFLAGS) $(LDFLAGS) $(LIBS)

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${EXEC_NAME}