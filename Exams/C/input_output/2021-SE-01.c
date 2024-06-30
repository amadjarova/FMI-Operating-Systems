#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>

uint8_t encode(uint8_t num);
uint8_t encode(uint8_t num) {
        uint8_t result = 0;
        uint8_t mask = 1<<7;

        for(int i = 0; i < 4; i++) {
                if((num & mask) == 0) {
                        result |= 1<<(6 - 2*i);
                } else {
                        result |= 1<<(7 - 2*i);
                }
                mask >>= 1;
        }

        return result;
}

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(2, "Invalid args count");
        }

        int inputFd = open(argv[1], O_RDONLY);
        if(inputFd == -1) {
                err(2, "open");
        }

        int outputFd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if(outputFd == -1) {
                err(3, "open");
        }

        uint8_t input;
        int res;

        while((res = read(inputFd, &input, sizeof(input))) == sizeof(input)) {
                uint8_t result1 = encode(input);
                uint8_t result2 = encode(input<<4);
                if(write(outputFd, &result1, sizeof(result1)) != sizeof(result1)) {
                        err(5, "write");
                }
                if(write(outputFd, &result2, sizeof(result2)) != sizeof(result2)) {
                        err(6, "write");
                }
        }

        if(res != 0) {
                err(7, "write");
        }

        close(inputFd);
        close(outputFd);
}
