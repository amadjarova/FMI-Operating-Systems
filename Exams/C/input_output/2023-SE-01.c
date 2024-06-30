#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
        if( argc != 3) {
                errx(1, "Invalid args count");
        }

        int fd1 = open(argv[1], O_RDONLY);
        if(fd1 == -1) {
                err(3, "open");
        }

        int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if(fd2 == -1) {
                err(4, "open");
        }
  
        char ch;
        while(read(fd1, &ch, sizeof(ch)) == sizeof(ch)) {
                if(ch == 0x55) {
                        uint8_t check = 0x55;
                        uint8_t n;
                        off_t offset = lseek(fd1, 0, SEEK_CUR);
                        if(offset == -1) {
                                err(3, "lseeek");
                        }

                        if(read(fd1, &n, sizeof(n)) != sizeof(n)) {
                                err(5, "read");
                        }
                        check^=n;
                        uint8_t c;
                        bool endFound = false;
                  
                        for(int i = 3; i<n; i++) {
                                if(read(fd1, &c, sizeof(c)) != sizeof(c)) {
                                        endFound=true;
                                        break;
                                }
                                check^=c;
                        }
                  
                        if(endFound) {
                                break;
                        }
                        uint8_t checkSum;
                        if(read(fd1, &checkSum, sizeof(checkSum)) == 0) {
                                break;
                        }
                  
                        if(checkSum == check) {
                                if(lseek(fd1, offset, SEEK_SET) == -1) {
                                        err(5, "lseek");
                                }
                                uint8_t magic = 0x55;
                                if(write(fd2, &magic, sizeof(magic)) != sizeof(magic)) {
                                        err(6, "write");
                                }
                                uint8_t ch2;
                                for(int i = 2; i<=n; i++) {
                                        if(read(fd1, &ch2, sizeof(ch2)) == -1) {
                                                err(3, "read");
                                        }
                                        if(write(fd2, &ch2, sizeof(ch2)) == -1) {
                                                err(4, "write");
                                        }
                                }
                        }
                        if(lseek(fd1, offset,SEEK_SET) == -1) {
                                err(10, "lseek");
                        }
                }
        }

                close(fd1);
                close(fd2);
}
