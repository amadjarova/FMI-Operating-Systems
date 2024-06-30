
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(3, "Wrong args count");
        }

        int n = argv[1][0] - '0';
        int d = argv[2][0] - '0';

        if(n<1 || n>9 || d<1 || d>9) {
                errx(4, "Invalid args");
        }

        int parentToChild[2];
        int childToParent[2];

        if(pipe(parentToChild) == -1) {
                err(4, "pipe");
        }
        if(pipe(childToParent) == -1) {
                err(5, "pipe");
        }
        char ch = 'a';
        if(write(childToParent[1], &ch, sizeof(ch)) != sizeof(ch)) {
                err(3, "write");
        }

        int counter = 0;

        int pid = fork();

        if(pid == -1) {
                err(6, "fork");
        }

        if(pid == 0) {
                close(childToParent[0]);
                close(parentToChild[1]);
                while(true) {
                char fromParent;
                if(read(parentToChild[0], &fromParent, sizeof(fromParent)) != sizeof(fromParent)) {
                        err(9, "read in child");
                }
                const char* str = "DONG ";
                if(write(1, str, 5) != 5) {
                        err(7, "read");
                }
                counter ++;
                if(counter == n) {
                        char toParent = 'c';
                        if(write(childToParent[1], &toParent, sizeof(toParent)) != sizeof(toParent)) {
                                err(4, "write");
                        }
                        break;
                }
                char toParent ='a';
                if(write(childToParent[1], &toParent, sizeof(toParent)) != sizeof(toParent)) {
                        err(8, "write");
                }

                }

                close(childToParent[1]);
                close(parentToChild[0]);
        } else {
                close(parentToChild[0]);
                close(childToParent[1]);

                while(true) {
                        char fromChild;
                        if(read(childToParent[0], &fromChild, sizeof(fromChild)) != sizeof(fromChild)) {
                                err(10, "read in parent");
                        }
                        if(sleep(d) != 0) {
                                err(11, "sleep");
                        }
                        if(fromChild == 'c') {
                                close(parentToChild[1]);
                                close(childToParent[0]);
                                break;
                        }
                        const char* str = "DING ";
                        if(write(1, str, 5) != 5) {
                                err(12, "write ding");
                        }
                        char toChild = 'd';
                        if(write(parentToChild[1], &toChild, sizeof(toChild)) != sizeof(toChild)) {
                                err(13, "write");
                        }
                }

                int status;
                if(wait(&status) == -1) {
                        err(4, "wait");
                }
                if(WIFEXITED(status)) {
                        if(WEXITSTATUS(status) != 0) {
                                errx(5, "exit status not 0");
                        }
                } else {
                        errx(4, "child was killed");
                }
        }
}
