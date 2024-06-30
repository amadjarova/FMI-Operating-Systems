#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(1, "Wrong arguments count");
        }

        if(strcmp(argv[1], "-d") == 0) {
                int res;
                char ch;
                while((res=read(0, &ch, sizeof(ch))) == sizeof(ch)) {
                        bool found = false;
                        for(int i = 0; i< strlen(argv[2]); i++) {
                                if(argv[1][i] == ch) {
                                        found = true;
                                        break;
                                }
                        }
                        if(!found) {
                                if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                                                err(2, "write");
                                }
                        }
                }

                if(res != 0) {
                        err(3, "read");
                }
        } else if(strcmp(argv[1], "-s") == 0) {
                int res;
                char ch;
                char prev;

                if(read(0, &prev, sizeof(prev)) == sizeof(prev)) {
                        if(write(1, &prev, sizeof(prev)) != sizeof(prev)) {
                                err(7, "write");
                        }
                } else {
                        err(8, "read");
                }

                while((res = read(0, &ch, sizeof(ch))) == sizeof(ch)) {
                        if(ch == prev) {
                                bool found = false;
                                for(int i = 0; i < strlen(argv[2]); i++) {
                                        if(ch == argv[2][i]) {
                                                found = true;
                                                break;
                                        }
                                }

                                if(found) {
                                        continue;
                                }
                        }
                        if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                                err(7, "write");
                        }

                        prev = ch;
                }
        } else {
                if(strlen(argv[1]) != strlen(argv[2])) {
                        errx(4, "sets with different length");
                }

                int res;
                char ch;
                while((res = read(0, &ch, sizeof(ch))) == sizeof(ch)) {
                        char wr = ch;
                        for(int i = 0; i < strlen(argv[1]); i++) {
                                if(argv[1][i] == ch) {
                                        wr = argv[2][i];
                                        break;
                                }
                        }
                        if(write(1, &wr, sizeof(wr)) != sizeof(wr)) {
                                err(5, "write");
                        }
                }

                if(res != 0) {
                        err(6, "read");
                }
        }
}
