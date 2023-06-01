#include <stdlib.h>
#include <stdio.h>
#include <gpiod.h>
#include "my_gpiod.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define FIFO_NAME "fifo2137"
#define PANIC(message)    \
    PRINT_ERROR(message); \
    kill(0, SIGKILL);     \
    exit(EXIT_FAILURE);

void play_or_pause();
void skip_song();
void volume_up();
void volume_down();

int fifo_fd = -1;

int main(int argc, char **argv)
{
    struct gpiod_chip *chip = NULL;
    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip)
    {
        PRINT_ERROR("Open chip failed");
        goto end;
    }

    if ((fifo_fd = open(FIFO_NAME, O_WRONLY)) < 0)
    {
        PRINT_ERROR("Open FIFO failed");
        goto chip_end;
    }

    await_buttons(chip, play_or_pause, skip_song, volume_up, volume_down);

fifo_end:
    if (close(fifo_fd) < 0)
    {
        PRINT_ERROR("Close FIFO failed");
    }

chip_end:
    gpiod_chip_close(chip);
    chip = NULL;
end:
    return EXIT_SUCCESS;
}

void mpg123_command(char command)
{
    if (write(fifo_fd, &command, sizeof(char)) < 0)
    {
        PANIC("Cannot write to fifo.");
    }
}

void play_or_pause()
{
    mpg123_command('s');
}

void skip_song()
{
    mpg123_command('f');
}

void volume_up()
{
    mpg123_command('+');
}

void volume_down()
{
    mpg123_command('-');
}
