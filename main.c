// must compile with: gcc  -std=c99 -Wall -o scheduling scheduling.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct Process {
    int id;
    int cpuTime;
    int ioTime;
    int arrivalTime;
    //extra info
    int state; //0: Running, 1: Ready, 2: Blocked
    int done; //1: done, 0: in use
};

struct Node {
    struct Process *currProc;
    struct Node *nextProc;
};

//GETTERS TODO DECIDE IF NEEDED
int getID(struct Process *proc) {
    return proc->id;
}
int getCpuTime(struct Process *proc) {
    return proc->cpuTime;
}

int getIoTime(struct Process *proc) {
    return proc->ioTime;
}

int getArrivalTime(struct Process *proc) {
    return proc->arrivalTime;
}
//SETTERS
void setID(struct Process *proc, int num) {
    proc->id = num;
}
void setCpuTime(struct Process *proc, int num) {
    proc->cpuTime = num;
}

void setIoTime(struct Process *proc, int num) {
    proc->ioTime = num;
}

void setArrivalTime(struct Process *proc, int num) {
    proc->arrivalTime = num;
}

void setDone(struct Process *proc, int num) {
    proc->done = num;
}

void decCpuTime(struct Process *proc) {
    proc->cpuTime -=1;
}
void stateChange(struct Process* proc, int newState) {
    proc->state = newState;
}

int getSize(struct Node* head) {
    int count = 0;
    struct Node* current = head;
    while (current != NULL) {
        count++;
        current = current->nextProc;
    }
    return count;
}

void insertFirst(struct Node** head, struct Process *proc) {

    struct Node *newNode = (struct Node*) malloc(sizeof(struct Node));

    struct Process* newProc = (struct Process*) malloc(sizeof(struct Process));
    newProc->id = proc->id;
    newProc->cpuTime = proc->cpuTime;
    newProc->ioTime = proc->ioTime;
    newProc->arrivalTime = proc->arrivalTime;
    newProc->state = proc->state;

    newNode->currProc = proc;
    newNode->nextProc = *head;
    *head = newNode;
}

void appendNode(struct Node** head, struct Process *proc) {

    struct Node *newNode = (struct Node*) malloc(sizeof(struct Node));
    struct Node *end = *head;

    struct Process* newProc = (struct Process*) malloc(sizeof(struct Process));
    newProc->id = proc->id;
    newProc->cpuTime = proc->cpuTime;
    newProc->ioTime = proc->ioTime;
    newProc->arrivalTime = proc->arrivalTime;
    newProc->state = proc->state;
    newProc->done = proc->done;

    newNode->currProc = newProc;
    newNode->nextProc = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    }
    else {
        while (end->nextProc != NULL) {
            end = end->nextProc;
        }
        end->nextProc = newNode;
    }
}

void deleteFirst(struct Node** head) {
    if (*head == NULL) {
        return;
    }
    struct Node* temp = *head;
    *head = (*head)->nextProc;
    free(temp);
}

void deleteKey(struct Node** head, int cpuID) {
    struct Node* temp = *head, *prev;
    if (temp != NULL && temp->currProc->id == cpuID) {
        *head = temp->nextProc;
        free(temp);
        return;
    }

    while (temp != NULL && temp->currProc->id != cpuID) {
        prev = temp;
        temp = temp->nextProc;
    }

    if (temp == NULL) {
        return;
    }

    prev->nextProc = temp->nextProc;
    free(temp);
}

struct Process* findID(struct Node *head, int cpuID) {
    struct Node *current = head;

    if (head == NULL) {
        printf("Could not be found");
        return NULL;
    }

    while(current->currProc->id != cpuID && current != NULL) {
        if (current->nextProc == NULL) {
            printf("Could not be found");
            return NULL;
        } else {
            current = current->nextProc;
        }
    }
    return current->currProc;
}
//TODO: DECIDE TO KEEP OR DELETE: not working well

void printNodeList(struct Node *head) {
    struct Node *currNode = head;
    while(currNode != NULL) {
        struct Process *cp = currNode->currProc;
        printf("ID: %d, CPU Time: %d, I/O Time: %d, Arrival Time: %d, State: %d | ",
               cp->id, cp->cpuTime, cp->ioTime, cp->arrivalTime, cp->state);
        currNode = currNode->nextProc;
    }
    printf(" \n");
}

struct Process* copyProc(struct Process* proc) {
    struct Process* newProc = (struct Process*) malloc(sizeof(struct Process));
    newProc->id = proc->id;
    newProc->cpuTime = proc->cpuTime;
    newProc->ioTime = proc->ioTime;
    newProc->arrivalTime = proc->arrivalTime;
    newProc->state = proc->state;
    newProc->done = proc->done;
    return newProc;
}

struct Node* copyList(struct Node* head) {
    if (head == NULL) {
        return NULL;
    }
    else {
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->currProc = copyProc(head->currProc);
        newNode->nextProc = copyList(head->nextProc);
        return newNode;
    }

}

void decIOTime(struct Node** head) {
    struct Node* curr = *head;
    while (curr != NULL) {
        curr->currProc->ioTime -=1;
        curr = curr->nextProc;
    }
}

//Algorithms
//TODO
void firstAlgo(struct Node *head, int numOfProcs) {
    struct Node* readyQueue = NULL;
    struct Node* blockedQueue = NULL;
    struct Node* finishedQueue = NULL;

    int sysCount = 0;
    int numFinished = 0;
    //TODO BUILD TEMP READY QUEUE and sort it by CPU ID then add to real ready queue
    while (numFinished != numOfProcs && sysCount < 10) { //manually add to ready queue based on count timer

        struct Node* generatorNode = copyList(head); //used for adding to ready queue
        while (generatorNode != NULL) { //logic for adding to ready queue based on systime/arrival time
            if (generatorNode->currProc->arrivalTime == sysCount) {
                appendNode(&readyQueue, generatorNode->currProc);
            }
            generatorNode = generatorNode->nextProc;
        } //ready queue formed from original list
        free(generatorNode); //no longer needed
        stateChange(readyQueue->currProc, 0); //set to running

        //TODO SORT BY CPUID
        if (getSize(blockedQueue) != 0) {
            printf("CYCLE in blocked: %d ", sysCount);
            decIOTime(&blockedQueue); //decs time for all
            printNodeList(blockedQueue);

            struct Node* currBlock = blockedQueue;
            while (currBlock != NULL) { //decreasing all ioTime
//                setIoTime(blockedQueue->currProc, getIoTime(blockedQueue->currProc) - 1);
                if (getIoTime(currBlock->currProc) == 0) { //adding to ready if done
                    printf("DID I EVER RUN?");
                    stateChange(currBlock->currProc, 1); //moving state to ready
                    appendNode(&readyQueue, copyProc(currBlock->currProc));
                    deleteKey(&blockedQueue, getID(currBlock->currProc)); //moving from ready to blocked
                }
                currBlock = currBlock->nextProc;
            }
        }//handles blocked Queue checks, dec, and moving

        int size  = getSize(readyQueue);
        printf("CYCLE: %d, SIZE: %d ",sysCount,size);
        printNodeList(readyQueue); //TODO TESTING
        int cpuTime = getCpuTime(readyQueue->currProc);
        int ioTime = getIoTime(readyQueue->currProc);
        int halfTime = (getCpuTime(readyQueue->currProc) + 1) / 2;

        if (halfTime == 0 && ioTime != 0) { //FIRST HALF TIME DONE
            stateChange(readyQueue->currProc, 2); //moving state to blocked
            appendNode(&blockedQueue, copyProc(readyQueue->currProc));
            deleteFirst(&readyQueue); //moving from ready to blocked
            sysCount++;
            continue;
//                readyQueue = readyQueue->nextProc;
        }
        if (cpuTime == 0 && ioTime == 0) { //FINISHED PROCESS
            setDone(readyQueue->currProc, 1);
            appendNode(&finishedQueue, copyProc(readyQueue->currProc));
            deleteFirst(&readyQueue); //proc is finished
            numFinished += 1;
            sysCount++;
            continue;
//                readyQueue = readyQueue->nextProc;
        }
        if (halfTime != 0) {
            setCpuTime(readyQueue->currProc, cpuTime - 1);
        }
        else {
//                readyQueue = readyQueue->nextProc;
        }
        sysCount++;
    } //main while loop
}

void roundAlgo() {

}

void shortestAlgo() {

}

int main(int argc, char *argv[]){

    int scheduling;
    FILE * fp; //for creating the output file
    char filename[100]=""; // the file name

// Check that the command line is correct
    if(argc != 3){

        printf("usage:  ./scheduling alg input\n");
        printf("alg: the scheduling algorithm: 0, 1, or 2\n");
        printf("input: the processes inut file\n");
        exit(1);
    }

    scheduling = (int)atoi(argv[1]); // the scheduling algorithm

//Check that the file specified by the user exists and open it
    if( !(fp = fopen(argv[2],"r")))
    {
        printf("Cannot open file %s\n", argv[2]);
        exit(1);
    }

    //Getting information from the file
    int numofProc;
    char currentLine[100];
    fgets(currentLine, 100, fp);
    numofProc = currentLine[0] - '0';

    //Generating our Procs and our list of Procs;
   struct Node *listOfProcs = NULL;
   while(fgets(currentLine, 100, fp) != NULL) {
        int procInfo[4];
        for (int i = 0; i < 4; i++) {
            procInfo[i] = currentLine[2*i] - '0';
        }
        //Making a proc: id, cpuTime, ioTime, arrivalTime, state, done
        struct Process *current = (struct Process*) malloc(sizeof(struct Process));
        current->id = procInfo[0];
        current->cpuTime = procInfo[1];
        current->ioTime = procInfo[2];
        current->arrivalTime = procInfo[3];
        current->state = 1;
        current->done = 0;

        appendNode(&listOfProcs, current); //adding to end of list
   }

   switch(scheduling) {
        case 0:
            firstAlgo(listOfProcs, numofProc);
            break;
        case 1:
            roundAlgo();
            break;
        case 2:
            shortestAlgo();
            break;
        default:
            printf("Scheduling type is not supported, \n Please input: 0, 1, or 2.");
            exit(1);
   }
    free(listOfProcs);

// form the output file name
    sprintf(filename,"%d-%s",scheduling, argv[2]);

// close the processes file
    fclose(fp);

    return 0;
}
