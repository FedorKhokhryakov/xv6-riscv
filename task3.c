#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

const int BUF_SIZE = 2048;

int main(int argc, char* argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "Error when creating pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error when creating process\n");
        exit(1);
    } 
    else if (pid == 0) { // child
        close(pipefd[1]);

        char read_buf[BUF_SIZE];
        ssize_t length, len_write, r;

        while ((length = read(pipefd[0], read_buf, BUF_SIZE)) > 0) {
            len_write = 0;

            while (len_write < length) {
                r = write(1, read_buf + len_write, length - len_write);
                if (r < 0) {
                    fprintf(stderr, "Error to output\n");
                    close(pipefd[0]);
                    exit(1);
                }
                len_write += r;
            }
        }
        if (length < 0) {
            fprintf(stderr, "Error to read\n");
            close(pipefd[0]);
            exit(1);
        }

        if (close(pipefd[0]) < 0) {
            fprintf(stderr, "Error to close pipe\n");
            exit(1);
        }
        exit(0);
    }
    else { // parent
        close(pipefd[0]);

        char buf[BUF_SIZE];
        int i, length, length2, pos = 0;
        for (i = 0; i < argc; ++i) {
            length = strlen(argv[i]);

            if (pos + length + 1 >= BUF_SIZE) {
                length2 = pos;

                while (length2 > 0) {
                    int r = write(pipefd[1], buf + pos, length2);
                    if (r == -1) {
                        fprintf(stderr, "Error to output\n");
                        close(pipefd[1]);
                        exit(1);
                    }
                    length2 -= r;
                    pos += r;
                }
                pos = 0;
            }

            if (length > BUF_SIZE) {
                int r = write(pipefd[1], argv[i], length);
                if (r == -1) {
                    fprintf(stderr, "Error to output\n");
                    close(pipefd[1]);
                    exit(1);
                }
            }
            else {
                memmove(buf + pos, argv[i], length);
                pos += length;
            }
            buf[pos] = '\n';
            ++pos;
        }
        if (pos > 0) {
            length2 = pos;
            while (length2 > 0) {
                int r = write(pipefd[1], buf + pos, length2);
                if (r == -1) {
                    fprintf(stderr, "Error to output\n");
                    close(pipefd[1]);
                    exit(1);
                }
                length2 -= r;
                pos += r;
            }
        }

        if (close(pipefd[1]) < 0) {
            fprintf(stderr, "Error to close pipe\n");
            exit(1);
        }

        wait(0);
        exit(0);
    }

    exit(0);
}