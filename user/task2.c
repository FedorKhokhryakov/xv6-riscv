#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int BUF_SIZE = 128;

int main(int argc, char *argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        fprintf(2, "Error when creating pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error when creating process\n");
        exit(1);
    } 
    else if (pid == 0) { // child
        close(pipefd[1]);
        close(0);
        dup(pipefd[0]);
        close(pipefd[0]);
        char *args[] = {"/wc", 0};
        if (exec("/wc", args) < 0) {
            fprintf(2, "Error to call wc\n");
            exit(1);
        }
    } 
    else { // parent
        close(pipefd[0]);
        char buf[BUF_SIZE];
        int pos = 0, lenght2;

        for (int i = 0; i < argc; ++i) {
            int length = strlen(argv[i]);

            if (pos + length + 1 >= BUF_SIZE) {
				lenght2 = pos;

				while (lenght2 > 0) {
					int r = write(pipefd[1], buf, lenght2);
					if (r == -1) {
						fprintf(2, "Error to output\n");
						close(pipefd[1]);
						exit(1);
					}
					lenght2 -= r;
					pos += r;
				}
				pos = 0;
			}

			if (length > BUF_SIZE) {
				int ret = write(pipefd[1], argv[i], length);
                if (ret == -1) {
                    fprintf(2, "Error to output\n");
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
            lenght2 = pos;
            while (lenght2 > 0) {
                int r = write(pipefd[1], buf, lenght2);
                if (r == -1) {
                    fprintf(2, "Error to output\n");
                    close(pipefd[1]);
                    exit(1);
                }
                lenght2 -= r;
                pos += r;
            }
        }

        if( close(pipefd[1]) < 0){
            fprintf(2, "Error to close pipe\n");
            exit(1);
        }
        wait(0);
        exit(0);
	}
    exit(0);
}    