#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "bitmap.h"
#include <fcntl.h>


#define ERROR_MESSAGE "Warning: one or more filter had an error, so the output image may not be correct.\n"
#define SUCCESS_MESSAGE "Image transformed successfully!\n"


/*
 * Check whether the given command is a valid image filter, and if so,
 * run the process.
 *
 * We've given you this function to illustrate the expected command-line
 * arguments for image_filter. No further error-checking is required for
 * the child processes.
 */
void run_command(const char *cmd) {
    if (strcmp(cmd, "copy") == 0 || strcmp(cmd, "./copy") == 0 ||
        strcmp(cmd, "greyscale") == 0 || strcmp(cmd, "./greyscale") == 0 ||
        strcmp(cmd, "gaussian_blur") == 0 || strcmp(cmd, "./gaussian_blur") == 0 ||
        strcmp(cmd, "edge_detection") == 0 || strcmp(cmd, "./edge_detection") == 0) {
        execl(cmd, cmd, NULL);
    } else if (strncmp(cmd, "scale", 5) == 0) {
        // Note: the numeric argument starts at cmd[6]
        execl("scale", "scale", cmd + 6, NULL);
    } else if (strncmp(cmd, "./scale", 7) == 0) {
        // Note: the numeric argument starts at cmd[8]
        execl("./scale", "./scale", cmd + 8, NULL);
    } else {
        fprintf(stderr, "Invalid command '%s'\n", cmd);
        exit(1);
    }
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: image_filter input output [filter ...]\n");
        exit(1);
    }
    int status;
    if(argc == 3){
        int n = fork();
        if(n < 0){
            perror("fork");
            exit(1);
        }
        if(n > 0){
            if((WIFEXITED(status))) {
                int exited = WEXITSTATUS(status);
                if(exited == 0) {
                    fprintf(stdout, "%s", ERROR_MESSAGE);
                } else {
                    fprintf(stdout, "%s", SUCCESS_MESSAGE);
                }
            }

        } else if(n == 0){
            int f, g;
            f = open(argv[1], O_RDONLY);
            if(dup2(f, fileno(stdin)) == -1){
                perror("dup2");
                exit(1);
            }  
            g = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if(dup2(g, 1) == -1){
                perror("dup2");
                exit(1);
            }
            close(g);
            close(f);
            run_command("./copy");
        }
    }

    else if(argc == 4){
        int n = fork();
        if(n < 0){
            perror("fork");
            exit(1);
        }
        if(n > 0){
            if((WIFEXITED(status))) {
                int exited = WEXITSTATUS(status);
                if(exited == 0) {
                    fprintf(stdout, "%s", ERROR_MESSAGE);
                } else {
                    fprintf(stdout, "%s", SUCCESS_MESSAGE);
                }
            }

        } else if(n == 0){
            int f, g;
            f = open(argv[1], O_RDONLY);
            if(dup2(f, fileno(stdin)) == -1){
                perror("dup2");
                exit(1);
            }  
            g = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if(dup2(g, 1) == -1){
                perror("dup2");
                exit(1);
            }
            close(g);
            close(f);
            run_command(argv[argc - 1]);
        }
    }

    else{
        int fd[argc - 4][2];
        for(int i = 0; i < argc - 4; i++){
            pipe(fd[i]);
        }
        for(int j = 0; j < argc - 3; j ++){
            int n = fork();
            if(n == 0){
                if(j == 0){
                    dup2(fd[0][1], fileno(stdout));
                    for(int a = 0; a < argc - 4; a++){
                        close(fd[a][1]);
                        close(fd[a][0]);
                    }
                    int f; 
                    f = open(argv[1], O_RDONLY);
                    dup2(f, fileno(stdin));
                    close(f);
                    run_command(argv[4]);
                }
                else if(j == argc - 4){
                    dup2(fd[j-1][0], fileno(stdin));
                    int g;
                    for(int a = 0; a < argc - 4; a++){
                        close(fd[a][1]);
                        close(fd[a][0]);
                    }
                    g = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(g, fileno(stdout));
                    close(g);
                    run_command(argv[argc - 1]);
                } else {
                    dup2(fd[j-1][0], fileno(stdin));
                    dup2(fd[j][1], fileno(stdout));
                    for(int a = 0; a < argc - 4; a++){
                        close(fd[a][1]);
                        close(fd[a][0]);
                    }
                    run_command(argv[j + 3]);
                }
            }
        }
        while(wait(&status) > 0);
        if((WIFEXITED(status))) {

                int exited = WEXITSTATUS(status);
                if(exited == 0) {
                    fprintf(stdout, "%s", SUCCESS_MESSAGE);
                } else {
                    fprintf(stdout, "%s", ERROR_MESSAGE);
                }
            }
    }

    return 0;
}
