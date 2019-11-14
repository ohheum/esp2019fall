#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#define FILESIZE 10

int main(int argc, char *argv[])    {
    int fd;
    char *map;  		/* mmapped array of char's */
    fd = open("sample.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        exit(1);
    }

    map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED) {
       close(fd);
       perror("Error mmapping the file");
       exit(EXIT_FAILURE);
    }

    for (i = 1; i <=NUMINTS; ++i)      /* Read the file int-by-int from the mmap*/
       printf("%d: %d\n", i, map[i]);

    if (munmap(map, FILESIZE) == -1)
        perror("Error un-mmapping the file");
    close(fd);
    return 0;
}
