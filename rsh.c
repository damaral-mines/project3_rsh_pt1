#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12
#define MAX_ARGS 256
#define MAX_ARG_LENGTH 256

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};


void print_help() {
    printf("The allowed commands are:\n");
    for(unsigned int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int isAllowed(const char*cmd) {
    for(unsigned int i = 0; i < N; i++) {
        if(strcmp(cmd, allowed[i]) == 0) {
            return 0;
        }
    }
	
	return 1;
}

int main() {

    pid_t pid;
    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);

    char line[256];
    char *args[MAX_ARGS + 1];

    char *token;
    int count;


    while (1) {

        fprintf(stderr,"rsh>");

        if (fgets(line,256,stdin)==NULL) continue;

        line[strcspn(line, "\n")] = '\0';

        count = 0;

        token = strtok(line, " \t");

        while (token != NULL && count < MAX_ARGS) {
            args[count++] = token;
            token = strtok(NULL, " \t");
        }
        args[count] = NULL;

        if (count == 0) continue;

        if(isAllowed(args[0]) != 0) {
            printf("NOT ALLOWED!\n");
            continue;
        }
        
        if(strcmp("exit", args[0]) == 0) {
            break;
        }

        if(strcmp("cd", args[0]) == 0) {
            if(count > 2) {
                printf("-rsh: cd: too many arguments\n");
                continue;
            }
            if(count < 2) continue;
            chdir(args[1]);
            continue;
        }
        
        if(strcmp("help", args[0]) == 0) {
            print_help();
            continue;
        }
        
        if(posix_spawnp(&pid, args[0], NULL, &attr, args, environ) != 0) {
            perror("spawn failed");
            exit(EXIT_FAILURE);
        }
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

    }
    posix_spawnattr_destroy(&attr);
    return 0;
}
