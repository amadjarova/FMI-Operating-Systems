#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>

uint8_t decodeFirstBytes(uint8_t num);
uint8_t decodeSecondBytes(uint8_t result, uint8_t num);

uint8_t decodeFirstBytes(uint8_t num) {
        uint8_t mask = 3<<6;
        uint8_t result = 0;
        for(int i = 0; i<4; i++) {
                uint8_t current = num & mask;
                uint8_t num1 = 1<<(7 - i*2);
                uint8_t num2 = 1<<(6 - i*2);
                if(current == num1) {
                        result |= 1<<(7 -i);
                } else if(current != num2) {
                        errx(3, "Invalid file format");
                }
                mask>>=2;

        }
        return result;
}

uint8_t decodeSecondBytes(uint8_t result, uint8_t num) {
        uint8_t mask = 3<<6;
        for(int i = 0; i <4; i++) {
                uint8_t current = num & mask;
                uint8_t num1 = 1<<(7 - i*2);
                uint8_t num2 = 1<<(6 - i*2);
                if(current == num1) {
                        result |= 1<<(3 -i);
                } else if(current != num2) {
                        errx(2, "Invalid file format");
                }
                mask>>=2;
        }
        return result;
}

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(1, "Wrong args count");
        }

        int inputFd = open(argv[1], O_RDONLY);
        if(inputFd == -1) {
                err(2, "open");
        }
        int outputFd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if(outputFd == -1) {
                err(3, "open");
        }

        uint8_t inputBytes[2];

        int res1;

        while((res1 = read(inputFd, inputBytes, sizeof(inputBytes))) == sizeof(inputBytes)) {
          uint8_t res;
          res = decodeFirstBytes(inputBytes[0]);
          res = decodeSecondBytes(res, inputBytes[1]);
          if(write(outputFd, &res, sizeof(res)) != sizeof(res)) {
                  err(3, "write");
          }
        }

        if(res1 != 0) {
                err(4, "read");
        }

        close(inputFd);
        close(outputFd);
}
