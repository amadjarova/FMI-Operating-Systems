#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>

typedef struct {
        uint16_t offset;
        uint8_t original;
        uint8_t new;
} file_struct;

uint16_t file_size(int fd) {
        struct stat file_stats;
        if(fstat(fd, &file_stats) == -1) {
                err(4, "fstat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 4) {
                errx(1, "Wrong arguments count");
        }

        int fd1;
        int fd2;
        int fd3;
        if((fd1 = open(argv[1], O_RDONLY)) == -1) {
                err(2, "open");
        }
        if((fd2 = open(argv[2], O_RDONLY)) == -1) {
                err(3, "open");
        }
        if((fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
                err(4, "open");
        }

        uint16_t file2Size = file_size(fd2);

        int res;
        char ch2;
        file_struct file1Struct;

        char buff[1024];
        int readSize;

        while((readSize = read(fd2, &buff, sizeof(buff))) > 0) {
                if (write(fd3, &buff, readSize) != readSize) {
                        err(5, "write");
                }
        }

        if(readSize == -1) {
                err(6, "read");
        }

        while((res = read(fd1, &file1Struct, sizeof(file1Struct))) == sizeof(file1Struct)) {
                if( file1Struct.offset > file2Size) {
                        errx(7, "file2 size");

                }

                if( lseek(fd2, file1Struct.offset, SEEK_SET) == -1) {
                        err(8, "lseek");
                }

                uint8_t ch1;
                if( read(fd2, &ch1, sizeof(ch1)) != sizeof(ch1)) {
                        err(9, "read");
                }

                if(ch1 != file1Struct.original) {
                        errx(10, "different chars");
                }

                if( lseek(fd3, file1Struct.offset, SEEK_SET) == -1) {
                        err(8, "lseek");
                }

                if( write(fd3, &file1Struct.new, sizeof(file1Struct.new)) != sizeof(file1Struct.new)) {
                        err(11, "write");
                }

        }

        if(res != 0) {
                err(12, "read");
        }

        close(fd1);
        close(fd2);
        close(fd3);
}
