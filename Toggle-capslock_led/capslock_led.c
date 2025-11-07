#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

int main(int argc, char *argv[]) {
    int fd;
    int led_state;

    // Check if exactly one argument is passed
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [0|1]\n", argv[0]);
        return 1;
    }

    // Parse argument
    if (argv[1][0] == '1' && argv[1][1] == '\0') {
        led_state = LED_CAP;
    } else if (argv[1][0] == '0' && argv[1][1] == '\0') {
        led_state = 0;
    } else {
        // Ignore invalid input
        fprintf(stderr, "Invalid input. Use 0 to turn off, 1 to turn on.\n");
        return 1;
    }

    // Open console device
    fd = open("/dev/console", O_NOCTTY);
    if (fd == -1) {
        perror("Error opening /dev/console");
        return 1;
    }

    // Set LED state (Caps Lock only)
    if (ioctl(fd, KDSETLED, led_state) == -1) {
        perror("Error setting LED state");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
