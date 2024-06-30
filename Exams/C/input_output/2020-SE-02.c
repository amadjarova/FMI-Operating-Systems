
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>

int main(int argc, char* argv[]) {
        if(argc != 4) {
                errx(1, "not enough args");
        }

        int scl;
        if((scl = open(argv[1], O_RDONLY)) == -1) {
                err(2, "openn, %s", argv[1]);
        }

        int sdl;
        if((sdl = open(argv[2], O_RDONLY)) == -1) {
                err(3, "open %s", argv[2]);
        }

        struct stat scl_st;
        if(fstat(scl, &scl_st) == -1) {
                err(4, "stat %s", argv[1]);
        }

        struct stat sdl_st;
        if(fstat(sdl, &sdl_st) == -1) {
                err(5, "stat %s", argv[2]);
        }

        if(sdl_st.st_size % sizeof(uint16_t) != 0) {
                errx(3, "bad sdl size");
        }

        int numElements = sdl_st.st_size/sizeof(uint16_t);
        int neededSCL = numElements/8;

        if(numElements % 8 != 0) {
                neededSCL++;
        }

        if(scl_st.st_size != neededSCL) {
                errx(3, "invalid SCL");
        }

        int out;
        if((out = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
                err(3, "open %s", argv[3]);
        }

        uint8_t scl_byte;
        uint16_t sdl_data;
        ssize_t read_size;

        while((read_size = read(scl, &scl_byte, sizeof(scl_byte))) > 0) {
                for(uint8_t mask = 0x80; mask != 0; mask >>=1) {
                        if((read_size = read(sdl, &sdl_data, sizeof(sdl_data))) == -1) {
                                err(4, "can't read from sdl");
                        }

                        uint8_t bit = (scl_byte & mask);

                        if(bit != 0) {
                                if(read_size == 0) {
                                        errx(4, "unexpected 1 at the end of SCL");
                                }

                                if(write(out, &sdl_data, sizeof(sdl_data)) == -1) {
                                        err(4, "can't write");
                                }
                        }
                }
        }

        if(read_size == -1) {
                err(5, "can't read from SCL");
        }

        close(scl);
        close(sdl);
        close(out);
}
