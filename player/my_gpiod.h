#ifndef _MY_GPIOD_H_
#define _MY_GPIOD_H_

#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <gpiod.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define PRINT_ERROR(source) (perror(source), \
                             fprintf(stderr, "%s:%d\n", __FILE__, __LINE__))

#define CHIP_NAME "gpiochip0"
#define LINE_LED0 24
#define LINE_LED1 22
#define LINE_LED2 23
#define LINE_LED3 27
static const int LINES_SIZE = 4;
static const unsigned int LINES_LED[] = {LINE_LED0, LINE_LED1, LINE_LED2, LINE_LED3};

#define LINE_BUTTON0 25
#define LINE_BUTTON1 10
#define LINE_BUTTON2 17
#define LINE_BUTTON3 18
static const unsigned int LINES_BUTTONS[] = {LINE_BUTTON0, LINE_BUTTON1, LINE_BUTTON2, LINE_BUTTON3};
#define CONSUMER "Consumer2137"

void await_buttons(
    struct gpiod_chip *chip,
    void (*button0)(void),
    void (*button1)(void),
    void (*button2)(void),
    void (*button3)(void));

#endif // _MY_GPIOD_H_
