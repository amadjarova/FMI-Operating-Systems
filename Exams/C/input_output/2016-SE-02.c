#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <sys/stat.h>

int file_size(int fd) {
        struct stat file_stats;
  
        if(fstat(fd, &file_stats) == -1) {
                err(2, "stat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(2,"Wrong arguments count");
        }

        int fd1;
        int fd2;
        int fd3;
        if((fd1 = open( argv[1], O_RDONLY)) == -1) {
                err(3, "open");
        }

        if((fd2 = open(argv[2], O_RDONLY)) == -1) {
                err(4, "open");
        }

        if((fd3 = open("f3", O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1) {
                err(5, "open");
        }

        int size1 = file_size(fd1);
        int size2 = file_size(fd2);

        if (size1 % 8 != 0 || size2 % 4 != 0) {
                errx(5, "size of files");
        }

        uint32_t pair[2];
        uint32_t num;

        int res;

        while( (res = read(fd1, &pair, sizeof(pair))) == sizeof(pair)) {
                if(lseek(fd2, pair[0]*sizeof(uint32_t), SEEK_SET) == -1) {
                        err(5, "seek");
                }

                for(int i = 0; i < pair[1] ; i++) {
                        if(read(fd2, &num, sizeof(num)) != sizeof(num)) {
                                err(5, "read");
                        }

                        if(write(fd3, &num, sizeof(num)) != sizeof(num)) {
                                        err(6, "write");
                        }
                }
        }

        if(res != 0) {
                err(7, "read");
        }

        close(fd1);
        close(fd2);
        close(fd3);
}
