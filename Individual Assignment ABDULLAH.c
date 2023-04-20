#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>

#define MAX_CHILD 7 //num child process
#define MESSAGE_SIZE 100 //buffer 

int num_child = 0;
pid_t child_pids[MAX_CHILD]; 
int pipeA[MAX_CHILD][2]; 

void sigint_handler(int sig) {
    printf ("\n Interruption has occured.Proceed exiting Program...\n");
    for (int i =0; i< MAX_CHILD;i++){
        kill(child_pids[i],SIGTERM);
    }
    exit(0); 
}

    void parent_process(){
    sleep(1);
    fflush(stdin);
    // starting to loop infinitely until interrupt occur 
    while(1){
        for(int i=0; i< MAX_CHILD; i++) {
        printf("\n Please enter the message to child %d:",i+1);
        char message[MESSAGE_SIZE];
            if (fgets (message, sizeof(message),stdin)==NULL){
                printf ("\nERROR!!");
            exit(1);
            }
        printf("Parent is Sending the Message to child %d\n",i+1);
        write(pipeA[i][1],message, sizeof(message));
        sleep(1);
        }
    }
}

void child_process(int id){
    printf ("child %d is reading from pipe\n",id+1);
    char message [MESSAGE_SIZE];
    read(pipeA[id][0],message,sizeof(message));
    printf ("Child %d has RECEIVED MESSAGE from Parent: %s",id+1,message);
    sleep(1);
}

int main(){
    signal (SIGINT, sigint_handler);

    for (int i = 0;i < MAX_CHILD; i++){
    if (pipe(pipeA[i]) == -1){
    printf ("ERROR CANNOT CREATE PIPE for Child %d \n",i+1);
    exit(1);
    }
    pid_t pid = fork();
    if (pid == -1){
    printf ("ERROR FORK child %d\n", i+1);
    exit(1);
    }
    else if (pid == 0){
    child_process(i);
    exit(0);
    }
    else{
    child_pids[i] = pid;
    num_child++;
    if(i == MAX_CHILD-1){
            parent_process();
    }
    }    
}
return 0;
}