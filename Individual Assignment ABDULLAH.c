#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>

#define MAX_CHILD 7 //maximum number of child processes
#define MESSAGE_SIZE 100 //size of the message buffer 

int num_child = 0; //number of child processes created
pid_t child_pids[MAX_CHILD]; //array to store child process
int pipeA[MAX_CHILD][2]; //array to store pipe

//signal handler function for SIGINT signal
void sigint_handler(int sig) {
    printf ("\n Interruption has occured.Proceed exiting Program...\n");
    //send SIGTERM signal to all child processes to terminate them
    for (int i =0; i< MAX_CHILD;i++){
        kill(child_pids[i],SIGTERM);
    }
    exit(0); 
}

//main function
int main(){
    //register the signal handler function for SIGINT / call signal handler function
    signal (SIGINT, sigint_handler);

    //create pipes for inter-process communication
    for (int i = 0;i < MAX_CHILD; i++){
        if (pipe(pipeA[i]) == -1){
            printf ("ERROR CANNOT CREATE PIPE for Child %d \n",i+1);
            exit(1);
        }

        //create a child process
        pid_t pid = fork();

        //check for errors while forking
        if (pid == -1){
            printf ("ERROR FORK child %d\n", i+1);
            exit(1);
        }
        else if (pid == 0){
            //call child process function
            child_process(i);
            exit(0);
        }
        else{
            //store child process ID in the array and increase the number of child processes created
            child_pids[i] = pid;
            num_child++;

            //if all child processes have been created, start the parent process
            if(i == MAX_CHILD-1){
                parent_process();// call parent function
            }
        }    
    }

    return 0;
}
//function for parent process
void parent_process(){
    sleep(1); //wait for child processes to be created
    fflush(stdin); //flush the input buffer to clean the user prompt

    //start an infinite loop until interrupt occurs
    while(1){
        //sending message to child
        for(int i=0; i< MAX_CHILD; i++) {
            printf("\n Please enter the message to child %d:",i+1);
            char message[MESSAGE_SIZE];

            //read input message from the user
            if (fgets (message, sizeof(message),stdin)==NULL){
                printf ("\nERROR!!");
                exit(1);
            }

            //write message to child through pipe
            printf("Parent is Sending the Message to child %d\n",i+1);
            write(pipeA[i][1],message, sizeof(message));
            sleep(1);
        }
    }
}
//function for child process
void child_process(int id){
    printf ("child %d is reading from pipe\n",id+1);

    //read message from the pipe
    char message [MESSAGE_SIZE];
    read(pipeA[id][0],message,sizeof(message));

    //display the user prompt message
    printf ("Child %d has RECEIVED MESSAGE from Parent: %s",id+1,message);
    sleep(1);
}
