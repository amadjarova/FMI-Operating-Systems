#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

int file_size(int fd);
int file_size(int fd) {
        struct stat st;
        if(fstat(fd, &st) == -1) {
                err(4, "fstat");
        }
        return st.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 4) {
                errx(2, "Invalid arguments count");
        }

        int idxFd = open(argv[3], O_RDONLY);
        if(idxFd == -1) {
                err(3, "open");
        }
        int dicFd = open(argv[2], O_RDONLY);
        if(dicFd == -1) {
                err(4, "open");
        }

        int idxSize=file_size(idxFd);
        int idxElements = idxSize/sizeof(uint32_t);

        int start = 0;
        int end = idxElements-1;
        bool found = false;
  
        while(start <= end) {
                int mid = start + (end -start)/2;
                uint32_t index1;
                if(lseek(idxFd, mid*sizeof(uint32_t), SEEK_SET) == -1) {
                        err(3, "lseek");
                }
                if(read(idxFd, &index1, sizeof(index1)) != sizeof(index1)) {
                        err(4, "read");
                }
                if(lseek(dicFd, index1+1, SEEK_SET) == -1) {
                        err(6, "lseek");
                }
                char buff[64];
                int res;
                char ch;
                int index = 0;
                while((res = read(dicFd, &ch, sizeof(ch))) == sizeof(ch)) {

                        if(ch == '\n') {
                                buff[index] = '\0';
                                break;
                        }
                        buff[index++] = ch;
                }
                if(res == -1) {
                        err(7, "read");
                }

                if(strcmp(buff, argv[1]) == 0) {
                        //read new line
                        int res1;
                        char byte;
                        while((res1 = read(dicFd, &byte, sizeof(byte))) == sizeof(byte)) {
                                if(byte == '\0') {
                                        break;
                                }
                                if(write(1, &byte, sizeof(byte)) != sizeof(byte)) {
                                        err(11, "write");
                                }

                        }
                        if(res1 == -1) {
                                err(12, "write");
                        }
                        found = true;
                        break;
                } else if(strcmp(argv[1], buff) < 0) {
                        end = mid -1;
                } else {
                        start = mid +1;
                }
        }
        const char* message = "Not found";
        if(!found) {
                if(write(1, message, sizeof(message) - 1) != sizeof(message) -1) {
                        err(13, "write");
                }
        }
        close(dicFd);
        close(idxFd);
}
