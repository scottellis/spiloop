/*
 * spidev loopback test
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define DEFAULT_DEVICE "/dev/spidev1.0"
#define MAX_SPIDEV_DATA 4096
#define DEFAULT_TRANSFER_COUNT 32
#define DEFAULT_ITERATIONS 1

void usage();
void dump_data(char *prompt, uint8_t *buff, int count);
void register_sig_handler();
void sigint_handler(int sig);
void msleep(int ms);

int abort_transfers = 0;

void usage()
{
    printf("\nUsage: spiloop [-s <speed>] [-d <device>] [-c <count>] [-i <iterations>] [-vh]\n");
    printf("  -s <speed>      Set SPI bus speed in Hz\n");
    printf("  -d <device>     Device, defaults to /dev/spidev1.0\n");
    printf("  -c <count>      Number of bytes to transfer, default is 32, max is 4096\n"); 
    printf("  -i <iterations> Number of times to repeat, default is 1, 0 means forever\n");
    printf("  -v              Be verbose\n");
    printf("  -h              Show this help\n\n");  
    printf("Use Ctrl-C to stop.\n\n");

    exit(1);
} 

int main(int argc, char *argv[])
{
    int opt, fd;
    uint32_t speed = 0;
    int iterations = DEFAULT_ITERATIONS;    
    int count = DEFAULT_TRANSFER_COUNT;
    int verbose = 0;
    uint8_t tx[MAX_SPIDEV_DATA];
    uint8_t rx[MAX_SPIDEV_DATA];
    int i;
    struct spi_ioc_transfer tr;
    char device[64];

    memset(device, 0, sizeof(device));
    strcpy(device, DEFAULT_DEVICE);
 
    while ((opt = getopt(argc, argv, "s:d:c:i:vh")) != -1) {
        switch (opt) {
        case 's':
            speed = atoi(optarg);

            if (speed < 100000 || speed > 24000000) {
                printf("\nInvalid speed: %s\n", optarg);
                usage();
            }

            break;

        case 'd':
            if (strlen(optarg) == 0 || strlen(optarg) > sizeof(device) - 2) {
                printf("\nInvalid device: %s\n", optarg);
                usage();
            }
           
            strcpy(device, optarg); 
            break;

        case 'c':
            count = atoi(optarg);

            if (count < 1 || count > MAX_SPIDEV_DATA) {
                printf("\nInvalid count: %s\n", optarg);
                usage();
            }

            break;

        case 'i':
            iterations = atoi(optarg);

            if (iterations < 0) {
                printf("\nInvalid iterations: %s\n", optarg);
                usage();
            }

            break;

        case 'v':
            verbose = 1;
            break;

        case 'h':
        default:
            usage();
            break;
        }
    }

	fd = open(device, O_RDWR);
	if (fd < 0) {
        perror("open");
        exit(1);
    }

    if (speed) { 
	    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
            perror("ioctl(SPI_IOC_WR_MAX_SPEED_HZ)");
            exit(1);
        }

        if (verbose) {
            if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
                perror("ioctl(SPI_IOC_RD_MAX_SPEED_HZ)");
                exit(1);
            }

            printf("Using speed: %u Hz\n", speed);
        }
    }

    register_sig_handler();

    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = (unsigned long)&tx;
    tr.len = count;
    tr.rx_buf = (unsigned long)rx;
    tr.len = count;

    i = 0;
    while (!abort_transfers) {
        memset(tx, (i & 0xff), count);
        memset(rx, 0, count);

        if (verbose)
            dump_data("tx", tx, count);

        if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1)
            perror("ioctl(SPI_IOC_MESSAGE)");
        else if (verbose)
            dump_data("rx", rx, count);

        i++;

        if (iterations != 0) {
            if (i > iterations)
                break;
        } 
    }

    printf("\nStopped after %d transfers\n", i);

    close(fd);

    return 0;
}

void dump_data(char *prompt, uint8_t *buff, int count)
{
    int i;

    if (prompt)
        printf("%s: ", prompt);

    for (i = 0; i < count; i++)
        printf("%02X ", buff[i]);

    printf("\n");
}

void register_sig_handler()
{
    struct sigaction sia;

    bzero(&sia, sizeof(sia));
    sia.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &sia, NULL) < 0) {
        perror("sigaction(SIGINT)");
        exit(1);
    }
}

void sigint_handler(int sig)
{
    abort_transfers = 1;
}

void msleep(int ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, NULL);
}
