#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
        char name[8];
        uint32_t offset;
        uint32_t length;
} file_struct;

long unsigned int file_size(int fd);
bool isValidName(char* name);

long unsigned int file_size(int fd) {
        struct stat st;
        if(fstat(fd, &st) == -1) {
                err(3, "fstat");
        }

        return st.st_size;
}

bool isValidName(char* name) {
        return name[7] == 0x00;
}

int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(2, "Wrong args count");
        }

        int fd = open(argv[1], O_RDONLY);
        if(fd == -1) {
                err(3, "open here");
        }

        int fsize = file_size(fd);

        if(fsize > sizeof(file_struct)*8 || fsize % sizeof(file_struct) != 0) {
                errx(4, "Invalid file size");
        }

        int resPipe[2];
        if(pipe(resPipe) == -1) {
                err(8, "pipe");
        }
        int children = 0;
        int res;
        file_struct st;
        while((res = read(fd, &st, sizeof(st))) == sizeof(st)) {
                if(!isValidName(st.name)) {
                        errx(5, "Invalid file format");
                }

                int pid = fork();

                if(pid == -1) { err(4, "fork"); }
                if(pid == 0) {
                        children+=1;
                        close(resPipe[0]);
                        int fd1 = open(st.name, O_RDONLY);
                        if(fd1 == -1) {
                                err(6, "open");
                        }

                        if(lseek(fd1, st.offset*sizeof(uint16_t), SEEK_SET) == -1) {
                                err(9, "lseek");
                        }
                        uint16_t elem;
                        uint16_t res1=0;
                        for(uint32_t i = 0; i<st.length; i++) {
                                if(read(fd1, &elem, sizeof(elem)) != sizeof(elem)) {
                                        err(8, "read");
                                }
                                res1 ^= elem;
                        }
                        if(write(resPipe[1], &res1, sizeof(res1)) == -1) {
                                err(10, "write");
                        }
                        close(fd1);
                        close(resPipe[1]);
                        exit(0);
                }
        }

        if(res != 0) {
                err(12, "read");
        }
  
        close(resPipe[1]);
        uint16_t finalRes=0;
        uint16_t current;
        while(read(resPipe[0], &current, sizeof(current)) == sizeof(current)) {
                finalRes^=current;
        }
        dprintf(1, "%.4X\n", finalRes);
        for(int i = 0; i < children; i++) {
                int status;
                int pid;
                if((pid = wait(&status)) == -1) {
                        err(3, "wait");
                }
                if(WIFEXITED(status)) {
                        if(WEXITSTATUS(status) != 0) {
                                errx(1, "error");
                        }
                } else {
                        errx(1, "error");
                }
        }
        close(resPipe[0]);
        close(fd);
}
