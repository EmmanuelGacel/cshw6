/*******************************************************************************
 * Name          :
 * Author        :
 * Version       :
 * Date          :
 * Last Modified :
 * Description   : 
*******************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"
#define PATH_MAX 4096

#define NEWLINE 10
#define SPACE   32
#define LCASE_C 99
#define LCASE_D 100
#define LCASE_E 101
#define LCASE_X 120
#define LCASE_I 105
#define LCASE_T 116

volatile sig_atomic_t signal_val = 0;




/**
 * Incomplete CD function
 * 
 * int chdir(const char * path) 
 */ 
int cd (char *command){
    printf("Inside cd fucntion \n");
    int index = 0;
    while (command[index] == SPACE) index ++; //Skips any spaces before a command
    index = index + 2;// skips past the "cd" command 
    chdir("~");
    return EXIT_SUCCESS;
}


int main (){
    
    while (true){ //While true keep printing the command line
        char current_working_directory[PATH_MAX];
        //Stores the cd inside the $current_working_directory buffer
        if(getcwd(current_working_directory, PATH_MAX) == NULL){
            fprintf(stderr, "Error reading CWD: %d\n", errno);
            return EXIT_FAILURE;
        }

        printf("%s", BRIGHTBLUE);
        printf("[%s]$ ", current_working_directory);//Prints the cwd
        printf(DEFAULT);
        fflush(stdout);//Flushes the print stream

        char * user_cmd = malloc(PATH_MAX);
        //Stores the user input into $user_cmd buffer
        if (fgets(user_cmd, PATH_MAX, stdin) == NULL){ 
           if (errno == EINTR){
                printf("Read interrupted. \n");
                errno = 0;
                continue;
           } 
        }else if (feof(stdin)){
            printf("\n");
            return EXIT_SUCCESS;
        }else if (ferror(stdin)){
            printf("\n");
            return EXIT_FAILURE;
        }

        int index = 0;
        while (user_cmd[index] == SPACE) index ++; //Skips any spaces before a command
        
        //Checks for "cd" command
        if ((user_cmd[index] == LCASE_C) && (user_cmd[index + 1] == LCASE_D)){
            printf("Inside cd if statment \n");
            cd(user_cmd);
        }

        //Checks for "exit" command
        if ((user_cmd[index] == LCASE_E) && (user_cmd[index + 1] == LCASE_X) 
        && (user_cmd[index + 2] == LCASE_I) && (user_cmd[index + 3] == LCASE_T)){
            printf("Inside exit if statment \n");
            
            //If not a ' ' character and if not a '\n' character move on to exec.
            if (user_cmd[index + 4] != SPACE &&  user_cmd[index + 4] != NEWLINE){
                if (isascii(user_cmd[index + 4]) != 0){
                    printf("ascii: %c \n", user_cmd[index + 4]);
                    printf("Not a valied 'exit' --> moving to exec");
                }
            }else{
                goto END; // If a ' ' or a '\n' character exit.
            }
            
        }


        
        
        
        
        printf("Made It!!! \n");

        

        //printf("\n%s", user_cmd);SSSSSS
        //scanf("%s", user_cmd);
    }
    END:
        return EXIT_SUCCESS;

    //if(user_cmd[])
    
}



