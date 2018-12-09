#define _GNU_SOURCE 
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <string.h>
#include <time.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/wait.h>

int my_system(char* line, char * arg1, char * arg2);
int my_system_fork(char *command);
int my_system_vFork(char *command);
int my_system_clone(char *command);
int my_system_pipe(char *command, char * myfifo, char * mode);
int clone_function(void *arg);
void getcmd(char* command, char * args[]);

int main(int argc, char* argv[]) {
    //Initialize file pointer
    char line[100];

    if (argc == 1) {
        while (1) {
            fgets(line, 100, stdin);
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) > 1) {
                if (strcmp(line, "exit") == 0) {
                    exit(0);
                }
                my_system(line, argv[1], argv[2]);
            }
        }
    } else if (argc == 2) {

        FILE *fp = fopen(argv[1], "r");
        if (fp == 0)
        {
            fprintf(stderr, "%s: failed to open file %s for reading\n", argv[0], argv[1]);
            return EXIT_FAILURE;
        }
        else
        {
            while (fgets(line, sizeof(line), fp)) {
                
                my_system(line, argv[1], argv[2]);

                //Exit if end of file is detected
                if(feof(fp)) {
                    break;
                }
            }
            fclose(fp);
        }
    } else if (argc == 3) {

        if (strcmp(argv[2], "R") != 0 && strcmp(argv[2], "W") != 0) {
            fprintf(stderr, "error: wrong number of arguments\n"
                            "usage: %s (textfile)\n"
                            "usage: %s (myFIFO) (R_or_W)\n", argv[0], argv[0]);
            return EXIT_FAILURE;     
        }

        if (strcmp(argv[1], "myfifo") != 0) {
            fprintf(stderr, "error: wrong named_pipe\n"
                            "usage: %s (textfile)\n"
                            "usage: %s (myfifo) (R_or_W)\n", argv[0], argv[0]);
            return EXIT_FAILURE;  
        }

        while (1) {
            fgets(line, 100, stdin);
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) > 1) {
                if (strcmp(line, "exit") == 0) {
                    exit(0);
                }
                my_system(line, argv[1], argv[2]);
            }
        }

    } else {
        fprintf(stderr, "error: wrong number of arguments\n"
                        "usage: %s (textfile)\n"
                        "usage: %s (myFIFO) (R_or_W)\n", argv[0], argv[0]);
        return EXIT_FAILURE;  
    }
    return 0;
}

int my_system(char* line, char * arg1, char * arg2) {
    #ifdef FORK
            my_system_fork(line);
    #elif VFORK
            my_system_vFork(line);
    #elif CLONE
            my_system_clone(line);
    #elif PIPE
            my_system_pipe(line, arg1, arg2);
    #else
            system(line);
    #endif

    return 0;           
}

/*
    -------------------------------------------------------------------------------

    -----------------------   Fork version of my_system  --------------------------

    -------------------------------------------------------------------------------
*/
int my_system_fork(char *command) {
    pid_t pid;
    int status;
    struct timespec tstart={0,0}, tend={0,0};

    pid = fork();

    if (pid == -1) {
        //An error occured while performing fork();
        printf("Error occured!");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        //pid == 0, running child process
        char * args[sizeof(command)];

        getcmd(command, args);      
        execvp(args[0], args);
        
    } else {
        /*
        This is parent process
        The parent process will wait the child process untill 
        the child process is terminated.
        */
       clock_gettime(CLOCK_MONOTONIC, &tstart);
       if (waitpid(pid, &status, 0) > 0) {
           if (WIFEXITED(status)) {
               //Child process is terminated successfully

                    clock_gettime(CLOCK_MONOTONIC, &tend);
                    printf("This command took about %.5f seconds\n",
                    ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
                    ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

               return WEXITSTATUS(status);
           } else if (WIFEXITED(status)){
               if (WEXITSTATUS(status) == 127) {
                   // Command in execv not found
                    printf("Execv failed, command not found!\n"); 
               } else {
                    printf("Program terminated normally, but returned a non-zero status\n"); 
               }
           } else {
               printf("Program did not terminate normally\n");
           }
       } else {
           //waitpid failed
           printf("waitpid() failed\n");
       }
    }
    return WEXITSTATUS(status);
}

/*
    -------------------------------------------------------------------------------
    
    -----------------------   vFork version of my_system  --------------------------

    -------------------------------------------------------------------------------
*/
int my_system_vFork(char *command) {
    pid_t pid;
    int status;
    struct timespec tstart={0,0}, tend={0,0};
    
    pid = vfork();

    if (pid == -1) {
        //An error occured while performing fork();
        printf("Error occured!");
        exit(EXIT_FAILURE); 
    } else if (pid == 0) {
        //pid == 0, running child process
        char * args[sizeof(command)];

        getcmd(command, args);      
        execvp(args[0], args);
    } else {
        /*
        This is parent process
        The parent process will wait the child process untill 
        the child process is terminated.
        */
       clock_gettime(CLOCK_MONOTONIC, &tstart);
       if (waitpid(pid, &status, 0) > 0) {
           if (WIFEXITED(status)) {
               //Child process is terminated successfully

                    clock_gettime(CLOCK_MONOTONIC, &tend);
                    printf("This command took about %.5f seconds\n",
                    ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
                    ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

               return WEXITSTATUS(status);
           } else if (WIFEXITED(status)){
               if (WEXITSTATUS(status) == 127) {
                   // Command in execv not found
                    printf("Execv failed, command not found!\n"); 
               } else {
                    printf("Program terminated normally, but returned a non-zero status\n"); 
               }
           } else {
               printf("Program did not terminate normally\n");
           }
       } else {
           //waitpid failed
           printf("waitpid() failed\n");
       }
    }
    return WEXITSTATUS(status);
}

/*
    -------------------------------------------------------------------------------
    
    -----------------------   clone version of my_system  --------------------------

    -------------------------------------------------------------------------------
*/
int my_system_clone(char *command) {
    pid_t pid;
    int status;
    struct timespec tstart={0,0}, tend={0,0};
    char *stack = malloc(1024 * 1024);

    if (stack == NULL ) {
        printf("ERROR: Unable to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    char *stackTop = stack + 1024 * 1024;

    //Pass CLONE_VFORK, CLONE_VM to emulate fork() and pass CLONE_FS to allow sharing of file system
    pid = clone(clone_function, stackTop, CLONE_VFORK | CLONE_VM | CLONE_FS | SIGCHLD, command);
    if ( pid < 0 ) {
        free(stack);
        printf("ERROR: Unable to Clone.\n");
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    if (waitpid(pid, &status, 0) > 0) {
        if (WIFEXITED(status)) {
            //Child process is terminated successfully

                clock_gettime(CLOCK_MONOTONIC, &tend);
                printf("This command took about %.5f seconds\n",
                ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
                ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

            return WEXITSTATUS(status);
        } else if (WIFEXITED(status)){
            if (WEXITSTATUS(status) == 127) {
                // Command in execv not found
                printf("Execv failed, command not found!\n"); 
            } else {
                printf("Program terminated normally, but returned a non-zero status\n"); 
            }
        } else {
            printf("Program did not terminate normally\n");
        }
    } else {
        //waitpid failed
        printf("waitpid() failed\n");
    }

    free(stack);
    return WEXITSTATUS(status);
}

int clone_function(void *arg) {
    char * args[sizeof(arg)];

    getcmd(arg, args);   

    if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]);
        return 0;
    } 

    execvp(args[0], args);

    return 0;
}

/*
    -------------------------------------------------------------------------------
    
    -----------------------   pipe version of my_system  --------------------------

    -------------------------------------------------------------------------------
*/
int my_system_pipe(char *command, char * myfifo, char * mode) {
    int status;
    int fd;
    pid_t pid = fork();


    if (pid == -1) {
        //An error occured while performing fork();
        printf("Error occured!");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        //pid == 0, running child process
        if (strcmp(mode, "R") == 0) {   //Read from FIFO
            fd = open(myfifo, O_RDONLY);
            dup2(fd, 0);
        } else {
            fd = open(myfifo, O_WRONLY);  //Write to FIFO
            dup2(fd, 1);
        }
        char * args[sizeof(command)];
        getcmd(command, args);      
        execvp(args[0], args); 
        close(fd);
        exit(127);   
    } else {
        /*
        This is parent process
        The parent process will wait the child process untill 
        the child process is terminated.
        */
       if (waitpid(pid, &status, 0) > 0) {
           if (WIFEXITED(status)) {
               //Child process is terminated successfully
               return WEXITSTATUS(status);
           } else if (WIFEXITED(status)){
               if (WEXITSTATUS(status) == 127) {
                   // Command in execv not found
                    printf("Execv failed, command not found!\n"); 
               } else {
                    printf("Program terminated normally, but returned a non-zero status\n"); 
               }
           } else {
               printf("Program did not terminate normally\n");
           }
       } else {
           //waitpid failed
           printf("waitpid() failed\n");
       }
    }
    return WEXITSTATUS(status);
}

void getcmd(char* command, char * args[]) {
    for (int i = 0; i < sizeof(command); i++) {
        args[i] = strsep(&command, " ");
        strtok(args[i], "\n");
        if (args[i] == NULL) {
            break;
        }
    }
}
