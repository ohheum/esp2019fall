#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define GPIO_PATH "/sys/class/gpio/gpio" 
#define MAX_EVENTS 100

void writeToPin(char *number, char *target, char *value);
void usageErr(const char *format, ...);
void exportPin(char *number);

int main(int argc, char *argv[])
{
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS]; 
    char buff[100];
    if (argc < 2)
        usageErr("%s file...\n", argv[0]);

    printf("Number of pins to monitor: %d\n", argc-1);

    int epfd = epoll_create(argc-1);
    if (epfd == -1) {
        printf("Fail to create epoll instance.\n");        
        exit(EXIT_FAILURE);
    }

    for (int j = 1; j < argc; j++) 
        exportPin(argv[j]);
    usleep(100000);

    for (int j = 1; j < argc; j++) {
        writeToPin(argv[j], "direction", "in");
        writeToPin(argv[j], "edge", "rising");
    }

    for (int j = 1; j < argc; j++) {
        sprintf(buff, "%s%s/%s", GPIO_PATH, argv[j], "value");
        int fd = open(buff, O_RDONLY);
        if (fd == -1) 
            exit(EXIT_FAILURE);
        ev.events = EPOLLIN | EPOLLET | EPOLLPRI;   
        ev.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD ,fd, &ev) == -1)
            exit(EXIT_FAILURE);
    }

    int limit = 10;
    while (--limit) {
        int ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1); 
        if (ready == -1) 
            exit(EXIT_FAILURE);

        for (int j = 0; j < ready; j++)
            printf(" fd=%d: %s\n", evlist[j].data.fd, "Button Pressed");
    }
    exit(EXIT_SUCCESS);    
}

void exportPin(char *number) {
    FILE *fp = fopen("/sys/class/gpio/export", "w+");
    if (fp == NULL) {
        printf("File open failed.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, number);
    fclose(fp);
}

void writeToPin(char *number, char *target, char *value) {
    char path[100];
    sprintf(path, "%s%s/%s", GPIO_PATH, number, target);
    FILE *fp = fopen(path, "w+");
    if (fp == NULL) {
        printf("File open failed.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, value);
    fclose(fp);
}

void usageErr(const char *format, ...) {
    va_list argList;
    fprintf(stderr, "Usage: "); 
    va_start(argList, format); 
    vfprintf(stderr, format, argList); 
    va_end(argList);
    exit(EXIT_FAILURE);
}
