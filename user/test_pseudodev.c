#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static const char hex_chars[] = "0123456789abcdef";

void print_hex_byte(unsigned char byte) {
    char hex[3];
    hex[0] = hex_chars[byte >> 4];
    hex[1] = hex_chars[byte & 0xF];
    hex[2] = ' ';
    if (write(1, hex, 3) < 0) {
        fprintf(2, "Error: write failed\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: %s <file> <number_of_bytes>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(2, "Error: can't open file '%s'\n", argv[1]);
        exit(1);
    }

    int bytes_to_read = atoi(argv[2]);
    if (bytes_to_read <= 0) {
        fprintf(2, "Error: invalid number of bytes '%s'\n", argv[2]);
        close(fd);
        exit(1);
    }

    unsigned char buffer[16];
    int total_read = 0;

    while (total_read < bytes_to_read) {
        int chunk = (bytes_to_read - total_read > sizeof(buffer)) ? sizeof(buffer) : (bytes_to_read - total_read);
        int n = read(fd, buffer, chunk);

        if (n < 0) {
            fprintf(2, "Error: read failed\n");
            close(fd);
            exit(1);
        }
        if (n == 0) break;

        for (int i = 0; i < n; ++i) {
            print_hex_byte(buffer[i]);
            if ((total_read + i + 1) % 16 == 0) {
                if (write(1, "\n", 1) < 0) {
                    fprintf(2, "Error: write failed\n");
                    exit(1);
                }
            }
        }

        total_read += n;
    }

    if (total_read % 16 != 0) {
        if (write(1, "\n", 1) < 0) {
            fprintf(2, "Error: write failed\n");
            exit(1);
        }
    }

    close(fd);
    exit(0);
}
