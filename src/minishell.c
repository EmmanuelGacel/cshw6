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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

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


void catch_signal(int sig){
	if(sig == SIGINT){
		signal_val = 1;
		printf("\n");
	}
}

int free_tokens(char** tokens, int size){
    int index = size;
    while (index >= 0){
        free(tokens[index]);
        index --;
    }
    free(tokens);
    return EXIT_SUCCESS;
}

/**
 * Incomplete CD function
 * 
 * int chdir(const char * path) 
 */ 
void cd (char *command, int index, char* cwd){
    
    index = index + 2;
    while (command[index] == SPACE) index ++; //skips any spaces 
    
    if((command[index] == NEWLINE) || (command[index] == '~' && command[index + 1] == NEWLINE)){ // go to ~
    	//printf("only cd \n");
    	uid_t uid = getuid(); // Get the user ID of the current user
    	struct passwd *pw = getpwuid(uid); // Get the password entry for the user
	
	if (pw == NULL) {
        	printf("Error: Cannot get passwd entry. %s.\n", strerror(errno));
        }
        	
        int result1 = chdir(pw->pw_dir); //changes to parent directory

    	//int result1 = chdir("/home/user");
        if(result1 == -1){ //couldnt change it to ' '
        	printf("Error: Cannot change directory to ' '. %s. \n", strerror(errno));
        }
        
        
   }
   else if((command[index] == '~' && command[index + 1] != NEWLINE)){ // ~/path entries
   	
    	uid_t uid = getuid(); // Get the user ID of the current user
    	struct passwd *pw = getpwuid(uid); // Get the password entry for the user
	
	if (pw == NULL) {
        	printf("Error: Cannot get passwd entry. %s.\n", strerror(errno));
        }
        
        char *dir = &command[index + 1]; //get past the '~'
        //strcat(fullpath, cwd);
        //strcat(fullpath, "/");
        //strcat(fullpath, dir);
         
        char *fullpath;
        if ((fullpath = malloc(strlen(pw->pw_dir) + strlen(dir) + 1)) == NULL){ //saves space on heap
        	printf("Error: malloc() failed. %s.\n",
                strerror(errno));
    	}
        fullpath[0] = '\0';
        //memset(fullpath, 0, strlen(pw->pw_dir) + strlen(dir) + 1);
        
        strcat(fullpath, pw->pw_dir);
        strcat(fullpath, dir); //dir should contain the '/' already
        
        char *eoln = strchr(fullpath, '\n');
        if(eoln != NULL){
        	*eoln = '\0'; //overwrite
        }
        
        int result0 = chdir(fullpath); //changes to parent directory

    	//int result1 = chdir("/home/user");
        if(result0 == -1){ //couldnt change it to ' '
        	printf("Error: Cannot change directory to '%s'. %s. \n", fullpath, strerror(errno));
        }
        
        free(fullpath);
   }
   else if(command[index] == '.' && command[index + 1] == '.'){
   	
   	//printf(".. portion\n");
   	
   	char *prev;
   	
   	if ((prev = malloc(strlen(cwd) + 1)) == NULL){ //saves space on heap
        	printf("Error: malloc() failed. %s.\n",
                strerror(errno));
    	}
    	strcpy(prev, cwd); // copy current working directory to previous path
    
    	// find the last path component by searching backwards for '/'
    	int i = strlen(prev) - 1;
    	while (i >= 0 && prev[i] != '/') {
        		i--;
    	}
    
    	if (i >= 0) { 		
        	   prev[i] = '\0';
    	}
    
    	int result = chdir(prev); // change directory to previous path
    	if (result == -1) { // couldn't change to previous path
        	printf("Error: Cannot change directory to '%s'. %s.\n", prev, strerror(errno));
    	}
    	
    	free(prev); // free memory for previous path
   }
    else{
    	 char *dir = &command[index];
    	 
    	 if (strchr(dir, ' ') != NULL) {
        	printf("Error: Too many arguments to cd.\n");
        	return;
         }
         
         char *fullpath;
         if ((fullpath = malloc(strlen(cwd) + strlen(dir) + 2)) == NULL){ // 1 for '/', 1 for null terminator
        	printf("Error: malloc() failed. %s.\n",
                strerror(errno));
    	 }
         
         fullpath[0] = '\0'; //make sure its empty before using strcat
         
         strcat(fullpath, cwd);
         strcat(fullpath, "/");
         strcat(fullpath, dir);
         
         char *eoln = strchr(fullpath, '\n');
         if(eoln != NULL){
         	*eoln = '\0'; //overwrite
         }
         
         
         //fullpath[(strlen(fullpath) - 1)] = '\0'; //get rid of newline

         int result = chdir(fullpath);
         if (result == -1) {
             printf("Error: Cannot change directory to '%s'. %s.\n", fullpath, strerror(errno));
         }
         
         free(fullpath);
    }
}


int main (){
    
    
    char current_working_directory[PATH_MAX];
    
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    
    if (sigaction(SIGINT, &action, NULL) == -1) {
		perror("sigaction(SIGINT)");
		catch_signal(SIGINT);
    }
    
    
    while (true){ //While true keep printing the command line
        //Stores the cd inside the $current_working_directory buffer
        
        if (signal_val == 1) { //is true
		//printf("inside sigint statement");
		signal_val = 0;
		printf("\n");
		continue;
	}
        
        if(getcwd(current_working_directory, PATH_MAX) == NULL){
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

        printf("%s", BRIGHTBLUE);
        printf("[%s]$ ", current_working_directory);//Prints the cwd
        printf(DEFAULT);
        fflush(stdout);//Flushes the print stream
        char * user_cmd;
        if((user_cmd = malloc(PATH_MAX)) == NULL){
            fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        //Stores the user input into $user_cmd buffer
        if (fgets(user_cmd, PATH_MAX, stdin) == NULL){
           if(signal_val == 1){ //CHECKS FOR A SIGNAL AFTER READING
           	continue;
           } 
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
        if ((user_cmd[index] == LCASE_C) && (user_cmd[index + 1] == LCASE_D) && ((user_cmd[index + 2] == NEWLINE) || (user_cmd[index + 2] == ' '))){
        	   
        	   cd(user_cmd, index, current_working_directory);
        	   
        	
            //cd(user_cmd); //calls function to deal with cd
        }else if ((user_cmd[index] == LCASE_E) && (user_cmd[index + 1] == LCASE_X)  //Checks for "exit" command
        && (user_cmd[index + 2] == LCASE_I) && (user_cmd[index + 3] == LCASE_T)){
            printf("Inside exit if statment \n");
            
            //If not a ' ' character and if not a '\n' character move on to exec.
            if (user_cmd[index + 4] != SPACE &&  user_cmd[index + 4] != NEWLINE){
                if (isascii(user_cmd[index + 4]) != 0){
                    goto INVALID_EXIT;
                }
            }else{
                goto END; // If a ' ' or a '\n' character exit.
            }
        }else{
            INVALID_EXIT: ;
            char * pointer = user_cmd;
            pointer = pointer + index;
            int nl_index;

            for(int i = 0; i < PATH_MAX && user_cmd[i] != NEWLINE; i++){ //Replaces all spaces and newlines with '\0'
                if (user_cmd[i] == SPACE) user_cmd[i] = '\0';
                if (user_cmd[i+1] == NEWLINE) nl_index = i+1; 
            }

            int t_index = 0;
            char **tokens; ; //Array of tokens to be used as argv
            if ((tokens = malloc((PATH_MAX/2) * sizeof(char *))) == NULL){
                fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
            while((t_index < PATH_MAX / 2) && (index < PATH_MAX)){
                char * word = user_cmd;
                word = word + index;
                
                if(*word == '\n'){
                    if((tokens[t_index] = (char *) malloc(1)) == NULL){ //Malloc size of tokens
                        fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
                        return EXIT_FAILURE;
                    }
                     strcpy(tokens[t_index], "");
                    break;
                }else{
                    //printf("Char: %d, Word: %s\n", word[0], word);
                    if((tokens[t_index] = (char *) malloc(strlen(word) + 1)) == NULL){ //Malloc size of tokens
                        fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
                        return EXIT_FAILURE;
                    }
                    strcpy(tokens[t_index], word); 
                    if((index + strlen(word)) < nl_index){
                        index = index + strlen(word);
                        while(user_cmd[index] == '\0') index++; //Incriments user_cmd to the next token
                    } else{
                        tokens[t_index][strlen(word) -1 ] = '\0';
                        index = nl_index;
                    }
                    t_index ++;//Incriments the token array.
                    //printf("Index: %d\n", index);
                    //printf("t_index: %d\n", t_index);
                   
                }
                
            }
            /*
            for(int j = 0; j < (PATH_MAX/2); j++){
                if (tokens[j] != 0) printf("Token %d: %s\n", j, tokens[j]);
            }
            */
            pid_t child_pid;
            if ((child_pid = fork()) < 0){
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            }else if (child_pid == 0){//Child process
                if (execvp(tokens[0], tokens) == -1){//Execvp command using tokens array
                    fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                    return EXIT_FAILURE;
                }
            }else if (child_pid > 0){
                
                if (waitpid(child_pid, NULL, 0) == -1){//Wait for the child process to terminate
                    fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));                 
                }
                printf("Inside Parent\n");
                free_tokens(tokens, t_index);
                free(user_cmd);
            }

        }
        
        
        //printf("Made It!!! \n");

        

        //printf("\n%s", user_cmd);SSSSSS
        //scanf("%s", user_cmd);
    }
    END:
        return EXIT_SUCCESS;
    
}



