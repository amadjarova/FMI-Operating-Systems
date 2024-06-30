
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdio.h>

typedef struct {
        uint32_t identifier;
        uint16_t unused1;
        uint16_t unused2;
        uint32_t startTime;
        uint32_t endTime;
} file_struct;

uint32_t findFileSize(int fd) {
        struct stat file_stats;
        if(fstat(fd, &file_stats) == -1) {
                err(2, "fstat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(1, "Wrong arguments count");
        }

        int fd;

        if((fd = open(argv[1], O_RDONLY)) == -1) {
                err(3, "open");
        }

        uint32_t fileSize = findFileSize(fd);

        if((fileSize % sizeof(file_struct)) != 0) {
                errx(4, "file size");
        }

        uint64_t average =0;
        uint32_t n = fileSize/sizeof(file_struct);
        long long dispersion = 0.0;
        file_struct str;

        int res;
        while((res = read(fd, &str, sizeof(str))) == sizeof(str)) {
                uint32_t duration = str.endTime - str.startTime;
                average += duration;
        }

        if(res != 0) {
                err(6, "read");
        }

        average /= n;

        if(lseek(fd, 0, SEEK_SET) == -1) {
                err(5, "lseek");
        }

        while((res = read(fd, &str, sizeof(str))) == sizeof(str)) {
                uint32_t duration = str.endTime - str.startTime;
                dispersion += (duration - average)*(duration - average);
        }

        if(res != 0) {
                err(6, "read");
        }

        dispersion /= n;

        if(lseek(fd, 0, SEEK_SET) == -1) {
                err(5, "lseek");
        }

        uint64_t identifiers[2048];
        uint64_t sessions[2048];
        uint32_t arrSize = 0;
        uint64_t dis = dispersion;
        while((res = read(fd, &str, sizeof(str))) == sizeof(str)) {
                uint64_t duration = str.endTime - str.startTime;
                if((duration*duration) > dis) {
                        bool found = false;
                        for(int i = 0; i < arrSize; i++) {
                                if(identifiers[i] == str.identifier) {
                                        if(sessions[i] < duration) {
                                                sessions[i] = duration;
                                        }
                                        found = true;
                                        break;
                                }
                        }
                        if(!found) {
                                identifiers[arrSize] = str.identifier;
                                sessions[arrSize++] = duration;
                        }
                }
        }

        if(res != 0) {
                err(8, "read");
        }

        for(uint32_t i = 0; i< arrSize; i++) {
                dprintf(1,"%ld %ld \n", identifiers[i], sessions[i]);
        }

        close(fd);
}
