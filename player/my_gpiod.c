#include "my_gpiod.h"

void sleep_in_miliseconds(int miliseconds)
{
    struct timespec timer = {0, miliseconds * 1000000};
    nanosleep(&timer, NULL);
}

volatile sig_atomic_t last_signal = 0;

void set_last_signal(int sig)
{
    last_signal = sig;
}

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        PRINT_ERROR("Sigaction");
}

void await_buttons(
    struct gpiod_chip *chip,
    void (*button0)(void),
    void (*button1)(void),
    void (*button2)(void),
    void (*button3)(void))
{
    sethandler(set_last_signal, SIGINT);

    struct gpiod_line_bulk line_bulk;
    gpiod_line_bulk_init(&line_bulk);

    for (int i = 0; i < LINES_SIZE; i++)
    {
        struct gpiod_line *line = NULL;
        line = gpiod_chip_get_line(chip, LINES_BUTTONS[i]);
        if (!line)
        {
            PRINT_ERROR("Open line failed");
            continue;
        }

        if (gpiod_line_request_falling_edge_events(line, CONSUMER) < 0)
        {
            PRINT_ERROR("Could not request line input");
            continue;
        }

        gpiod_line_bulk_add(&line_bulk, line);
    }

    struct timespec timer = {3600, 0};
    struct gpiod_line_bulk event_bulk;
    gpiod_line_bulk_init(&event_bulk);

    while (last_signal != SIGINT && gpiod_line_event_wait_bulk(&line_bulk, &timer, &event_bulk) == 1)
    {
        int curline;
        for (int i = 0; i < event_bulk.num_lines && event_bulk.lines[i] != NULL; i++)
        {
            struct gpiod_line_event event;
            if (gpiod_line_event_read(event_bulk.lines[i], &event) < 0)
            {
                PRINT_ERROR("Reading line error");
            }
            else
            {
                curline = i;
            }
        }

        // debouncing
        struct timespec timer_cl = {0, 80L * 1000000L};
        struct gpiod_line_bulk event_bulk_cl;
        while (gpiod_line_event_wait_bulk(&line_bulk, &timer_cl, &event_bulk_cl) == 1)
        {
            for (int i = 0; i < event_bulk_cl.num_lines && event_bulk_cl.lines[i] != NULL; i++)
            {
                struct gpiod_line_event event;
                if (gpiod_line_event_read(event_bulk_cl.lines[i], &event) < 0)
                {
                    PRINT_ERROR("Reading line error");
                }
                else
                {
#ifdef DEBUG
                    printf("Ignored bouncing\n");
#endif
                }
            }
        }

        if (gpiod_line_get_value(event_bulk.lines[curline]) == 0)
        {
            switch (gpiod_line_offset(event_bulk.lines[curline]))
            {
            case LINE_BUTTON0:
                button0();
                break;
            case LINE_BUTTON1:
                button1();
                break;
            case LINE_BUTTON2:
                button2();
                break;
            case LINE_BUTTON3:
                button3();
            }
        }
    }

    gpiod_line_bulk_init(&event_bulk);
}
