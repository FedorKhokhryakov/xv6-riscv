#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

const size_t BUF_SIZE = 2048;

int main(int argc, char* argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("Error when creating pipe");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        perror("Error when creating process");
        exit(1);
    } 
    else if (pid == 0) { // child
        if (close(pipefd[1]) < 0) {
            perror("Error closing pipe write end in child");
            exit(1);
        }

        char read_buf[BUF_SIZE];
        ssize_t length, len_write, r;

        while ((length = read(pipefd[0], read_buf, BUF_SIZE)) > 0) {
            len_write = 0;

            while (len_write < length) {
                r = write(STDOUT_FILENO, read_buf + len_write, length - len_write);
                if (r < 0) {
                    perror("Error writing to stdout");
                    close(pipefd[0]);
                    exit(1);
                }
                len_write += r;
            }
        }
        if (length < 0) {
            perror("Error reading from pipe");
            close(pipefd[0]);
            exit(1);
        }

        if (close(pipefd[0]) < 0) {
            perror("Error closing pipe read end in child");
            exit(1);
        }
        exit(0);
    }
    else { // parent
        if (close(pipefd[0]) < 0) {
            perror("Error closing pipe read end in parent");
            exit(1);
        }

        char buf[BUF_SIZE];
        size_t pos = 0;
        ssize_t length2;

        for (int i = 0; i < argc; ++i) {
            size_t length = strlen(argv[i]);

            if (pos + length + 1 >= BUF_SIZE) {
                length2 = pos;

                while (length2 > 0) {
                    ssize_t r = write(pipefd[1], buf, length2);
                    if (r == -1) {
                        perror("Error writing to pipe");
                        close(pipefd[1]);
                        exit(1);
                    }
                    length2 -= r;
                    pos += r;
                }
                pos = 0;
            }

            if (length > BUF_SIZE) {
                const char *p = argv[i];
                size_t remaining = length;
                while (remaining > 0) {
                    ssize_t r = write(pipefd[1], p, remaining);
                    if (r == -1) {
                        perror("Error writing argument to pipe");
                        close(pipefd[1]);
                        exit(1);
                    }
                    p += r;
                    remaining -= r;
                }
            }
            else {
                memcpy(buf + pos, argv[i], length); // memcpy вместо memmove
                pos += length;
            }
            buf[pos] = '\n';
            ++pos;
        }

        if (pos > 0) {
            length2 = pos;
            while (length2 > 0) {
                ssize_t r = write(pipefd[1], buf, length2);
                if (r == -1) {
                    perror("Error writing final buffer to pipe");
                    close(pipefd[1]);
                    exit(1);
                }
                length2 -= r;
            }
        }

        if (close(pipefd[1]) < 0) {
            perror("Error closing pipe write end in parent");
            exit(1);
        }

        wait(0);
        exit(0);
    }
}
