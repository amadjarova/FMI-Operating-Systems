#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
        uint16_t offset;
        uint8_t original;
        uint8_t new;
} file_struct;

int file_size(int fd) {
        struct stat file_size;
        if(fstat(fd, &file_size) == -1) {
                err(2, "fstat");
        }

        return file_size.st_size;
}

int main(int argc, char* argv[]) {
        if (argc != 4) {
                errx(1, "Wrong arguments count");
        }

        int fd1;
        int fd2;
        int fd3;

        if((fd1 = open(argv[1], O_RDONLY)) == -1) {
                err(3, "open");
        }
        if((fd2 = open(argv[2], O_RDONLY)) == -1) {
                err(4, "open");
        }
        if((fd3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) == -1) {
                err(5, "open");
        }

        if(file_size(fd1) != file_size(fd2)) {
                errx(6, "files size");
        }

        char ch1;
        char ch2;

        int res1;
        int res2;

        while((res1 = read(fd1, &ch1, sizeof(ch1))) == sizeof(ch1) && (res2 = read(fd2, &ch2, sizeof(ch2))) == sizeof(ch2)) {
                if(ch1 != ch2) {
                        file_struct file3Content;

                        if((file3Content.offset = lseek(fd1, 0, SEEK_CUR)) == -1) {
                                err(7, "lseek");
                        }
                        file3Content.offset--;
                        file3Content.original = ch1;
                        file3Content.new = ch2;

                        if(write(fd3, &file3Content, sizeof(file3Content)) != sizeof(file3Content)) {
                                err(8, "write");
                        }

                }
        }

        if(res1 != 0 || res2 != 0) {
                err(9,"read");
        }

        close(fd1);
        close(fd2);
        close(fd3);
}
