#include <stdlib.h>
#include <stdio.h>
#include <gpiod.h>
#include "my_gpiod.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define FIFO_NAME "mypipe"
#define MUSIC_DIR "/home/Music"
#define PANIC(message)    \
    PRINT_ERROR(message); \
    kill(0, SIGKILL);     \
    exit(EXIT_FAILURE);

void play_or_pause();
void next_song();
void volume_up();
void volume_down();

int fifo_fd = -1;
int volume = 50;
char *music_dir = MUSIC_DIR;
int current_song = 0;

int main(int argc, char **argv)
{
    char *fifo_name = argc > 1 ? argv[1] : FIFO_NAME;
    music_dir = argc > 2 ? argv[2] : MUSIC_DIR;
    /*
    struct gpiod_chip *chip = NULL;
    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip)
    {
        PRINT_ERROR("Open chip failed");
        goto end;
    }
    */

    if ((fifo_fd = open(fifo_name, O_WRONLY)) < 0)
    {
        PRINT_ERROR("Open FIFO failed");
        goto chip_end;
    }

    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();
    sleep(5);
    next_song();

    // await_buttons(chip, play_or_pause, next_song, volume_up, volume_down);

    if (close(fifo_fd) < 0)
    {
        PRINT_ERROR("Close FIFO failed");
    }

chip_end:
    // gpiod_chip_close(chip);
    // chip = NULL;
end:
    return EXIT_SUCCESS;
}

void mpg123_command(char *command)
{
    if (write(fifo_fd, command, sizeof(char) * (strlen(command))) < 0)
    {
        PANIC("Cannot write to fifo.");
    }
}

void play_or_pause()
{
    printf("a\n");
    char command[BUFSIZ];
    snprintf(command, BUFSIZ - 1, "P\n");
    mpg123_command(command);
}

void next_song()
{
    char command[BUFSIZ];

    DIR *dir;
    if ((dir = opendir(music_dir)) == NULL)
    {
        PANIC("Cannot open music directory");
    }
    char next_number[4];
    char next_song[BUFSIZ - 4];
    snprintf(next_number, BUFSIZ, "%03d", current_song);

    struct dirent *dp;
    int ctr = 0;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strncmp(dp->d_name, next_number, 3) == 0)
        {
            strcpy(next_song, dp->d_name);
        }
        ctr++;
    }

    if (closedir(dir) < 0)
    {
        PANIC("Cannot close opened music directory.");
    }
    ctr -= 3;
    current_song = (current_song + 1) % ctr;
    printf("%s\n", next_song);

    snprintf(command, BUFSIZ, "L %s\n", next_song);
    mpg123_command(command);
}

void volume_up()
{
    char command[BUFSIZ];

    if (volume < 100)
    {
        volume += 10;
    }

    snprintf(command, BUFSIZ - 1, "V %d\n", volume);
    mpg123_command(command);
}

void volume_down()
{
    char command[BUFSIZ];

    if (volume > 0)
    {
        volume -= 10;
    }

    snprintf(command, BUFSIZ - 1, "V %d\n", volume);
    mpg123_command(command);
}
