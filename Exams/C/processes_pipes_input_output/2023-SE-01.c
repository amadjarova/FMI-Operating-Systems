#include <stdint.h>
#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(1,"Invalid args count");
        }

        int findPipe[2];

        if(pipe(findPipe) == -1) {
                err(2, "pipe");
        }

        pid_t pid = fork();
        if(pid == -1) {
                errx(3, "fork");
        }

        if(pid == 0) {
                close(findPipe[0]);
                if(dup2(findPipe[1], 1) == -1) {
                        err(2, "dup");
                }
                close(findPipe[1]);
                execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", (char*)NULL);
                err(3, "exec");
        }
        close(findPipe[1]);

        char buff[256];
        char ch;
        int res;
        int index = 0;
        int children = 0;
  
        while((res = read(findPipe[0], &ch, sizeof(ch))) == sizeof(ch)) {
                if(ch == '\n') {
                        buff[index] = '\0';
                        index = 0;
                        int mdPipe[2];
                        if(pipe(mdPipe) == -1) {
                                err(3, "pipe");
                        }
                        children++;

                        pid_t pidd = fork();

                        if(pidd == -1) {
                                err(3, "fork");
                        }
                  
                        if(pidd == 0) {
                                close(mdPipe[0]);
                                if(dup2(mdPipe[1], 1) == -1) {
                                        err(5, "dup");
                                }
                                close(mdPipe[1]);
                                execlp("md5sum", "md5sum", buff, (char*)NULL);
                                err(7, "exec");
                        }
                  
                        close(mdPipe[1]);
                        char newName[128];
                        strcpy(newName, buff);
                        strncat(newName, ".hash", 128 - strlen(newName) - 1);
                        int fd1 = open(newName, O_CREAT | O_TRUNC | O_WRONLY, 0666);
                        if(fd1 == -1) {
                                err(4, "open");
                        }

                        char buffMd5[64];
                        uint8_t read_size;
                        if((read_size = read(mdPipe[0], buffMd5, 64)) != -1) {
                                if(write(fd1, buffMd5, read_size) != read_size) {
                                        err(3, "write");
                                }
                        } else {
                                err(4, "read");
                        }
                        close(mdPipe[0]);
                        close(fd1);

                } else {
                  buff[index++] = ch;
                }
        }

        close(findPipe[0]);

        for(int i = 0; i< children; i++) {
                int status;
                if(wait(&status) == -1) {
                        err(2, "wait");
                }
                if(WIFEXITED(status)) {
                        if(WEXITSTATUS(status) != 0) {
                                errx(4, "status not 0");
                        }
                } else {
                        errx(5, "child killed");
                }
        }
}
