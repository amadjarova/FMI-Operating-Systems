#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
        uint16_t offset;
        uint8_t length;
        uint8_t notUsed;
} idx_content;

int file_size(int fd) {
        struct stat file_stats;
        if(fstat(fd, &file_stats) == -1) {
                err(5, "stat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 5) {
                errx(1, "Wrong args count");
        }

        int fd1;
        int fd2;
        int fd3;
        int fd4;
        if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
                err(1,"open");
        }

        if((fd2 = open(argv[2], O_RDONLY)) == -1) {
                err(2,"open");
        }

        if((fd3 = open(argv[3], O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU)) == -1) {
                err(3, "open");
        }

        if((fd4 = open(argv[4], O_TRUNC |  O_WRONLY | O_CREAT, S_IRWXU)) == -1) {
                err(4, "open");
        }

        idx_content buff;

        if(file_size(fd2) % sizeof(buff) != 0) {
                errx(6, "file2 size");
        }

        int res;

        while((res = read(fd2, &buff, sizeof(buff))) == sizeof(buff)) {
                        if(lseek(fd1, buff.offset, SEEK_SET) == -1) {
                                err(7, "lseek");
                        }

                        char searchedString[256];

                        if(read(fd1, &searchedString, buff.length) != buff.length) {
                                err(8, "read");
                        }

                        if(searchedString[0]>= 'A' && searchedString[0] <= 'Z') {
                                idx_content file4Content;

                                file4Content.length = buff.length;

                                if((file4Content.offset = lseek(fd3, 0, SEEK_CUR)) == -1) {
                                        err(10, "lseek");
                                }

                                if(write(fd3, &searchedString, buff.length) != buff.length) {
                                        err(9, "write");
                                }

                                if(write(fd4, &file4Content, sizeof(file4Content)) != sizeof(file4Content)) {
                                        err(11, "write");
                                }
                        }

        }

        if(res != 0) {
                err(12, "read");
        }

        close(fd1);
        close(fd2);
        close(fd3);
        close(fd4);
}
