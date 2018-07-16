/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

/* Definition of constants. */

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1
#define GPIO_LOW    0
#define GPIO_HIGH   1

unsigned int RESET = 48;


/* Definitions of GPIO manipulation functions. */

int gpio_export(unsigned int gpio)
{       int fd, len;
        char buf[MAX_BUF];
        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0) 
        {       perror("gpio/export");
                return fd; }
        len = snprintf(buf, sizeof(buf), "%d", gpio);
        write(fd, buf, len);
        close(fd);
        return 0; }

int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{       int fd;
        char buf[MAX_BUF];
        snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
        fd = open(buf, O_WRONLY);
        if (fd < 0) 
        {       perror("gpio/direction");
                return fd; }
        if (out_flag == GPIO_OUTPUT) write(fd, "out", 4); else write(fd, "in", 3);
        close(fd);
        return 0; }

int gpio_set_value(unsigned int gpio, unsigned int value)
{       int fd;
        char buf[MAX_BUF];
        snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        fd = open(buf, O_WRONLY);
        if (fd < 0) 
        {       perror("gpio/set-value");
                return fd; }
        if (value==GPIO_LOW) write(fd, "0", 2); else write(fd, "1", 2);
        close(fd);
        return 0; }

/* Pulse the concentrator reset line once. */
int main(int argc, char *argv[])
{ gpio_export(RESET);
  gpio_set_dir(RESET, GPIO_OUTPUT);
  gpio_set_value(RESET, GPIO_LOW);
  usleep(10000);
  gpio_set_value(RESET, GPIO_HIGH);
  usleep(10000);
  gpio_set_value(RESET, GPIO_LOW);
  return 0; }

