#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"
#define PATH_MAX 4096

volatile sig_atomic_t signal_val = 0;

int main (){
    
    while (true){ //While true keep printing the command line
        char current_working_directory[PATH_MAX];
    
        if(getcwd(current_working_directory, sizeof(current_working_directory)) == NULL){
            fprintf(stderr, "Error reading CWD: %d\n", errno);
        }
        char * user_cmd = malloc(PATH_MAX);
        printf("%s", BRIGHTBLUE);
        printf("[%s]$ ", current_working_directory);
        printf(DEFAULT);
        fgets (user_cmd, sizeof(PATH_MAX), stdin);
        //printf("\n%s", user_cmd);SSSSSS
        //scanf("%s", user_cmd);
    }
    
    

    
}



