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
    int halfTime;
    int finishTime;
};

struct Node {
    struct Process *currProc;
    struct Node *nextProc;
};

struct Process* copyProc(struct Process* proc) {
    struct Process* newProc = (struct Process*) malloc(sizeof(struct Process));
    newProc->id = proc->id;
    newProc->cpuTime = proc->cpuTime;
    newProc->ioTime = proc->ioTime;
    newProc->arrivalTime = proc->arrivalTime;
    newProc->state = proc->state;
    newProc->done = proc->done;
    newProc->halfTime = proc->halfTime;
    newProc->finishTime = proc->finishTime;
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

//GETTERS TODO DECIDE IF NEEDED
int getID(struct Process *proc) {
    return proc->id;
}

int getCpuTime(struct Process *proc) {
    return proc->cpuTime;
}

int getHalfTime(struct Process *proc) {
    return proc->halfTime;
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

void setHalfTime(struct Process *proc, int num) {
    proc->halfTime = num;
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
    struct Process* newProc = copyProc(proc);
    newNode->currProc = proc;
    newNode->nextProc = *head;
    *head = newNode;
}

void appendNode(struct Node** head, struct Process *proc) {

    struct Node *newNode = (struct Node*) malloc(sizeof(struct Node));
    struct Node *end = *head;

    struct Process* newProc = copyProc(proc);
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

void decIOTime(struct Node* head) {
    struct Node* curr = head;
    while (curr != NULL) {
        curr->currProc->ioTime -=1;
        curr = curr->nextProc;
    }
}

char* getState(struct Process* proc) {
    char* string = malloc(10);
    if (proc->state == 0) {
        string = "running";
    }
    else if (proc->state == 1) {
        string = "ready";
    }
    else {
        string = "blocked";
    }
    return string;
 }

 void swap(struct Node *a, struct Node *b) {
    struct Process *temp = copyProc(a->currProc);
    a->currProc = b->currProc;
    b->currProc = temp;
}

 void sort(struct Node* head) { //Bubble sort
    int swapped;
    struct Node* curr;
    struct Node* nodeL = NULL;

    if (head == NULL) {
        return;
    }

    do {
        swapped = 0;
        curr = head;
        while (curr->nextProc != nodeL) {
            if (curr->currProc->id > curr->nextProc->currProc->id) {
                swap(curr, curr->nextProc);
                swapped = 1;
            }
            curr = curr->nextProc;
        }
        nodeL = curr;
    } while (swapped);
}

//TODO REMOVE
void productionPrint(struct Node *head) {
    sort(head);
    struct Node *currNode = head;
    while(currNode != NULL) {
        struct Process *cp = currNode->currProc;
        printf("%d: %s ", cp->id, getState(cp));
        currNode = currNode->nextProc;
    }
    printf(" \n");
}

//TODO REMOVE AFTER TESTING
void printNodeList(struct Node *head) {
    struct Node *currNode = head;
    while(currNode != NULL) {
        struct Process *cp = currNode->currProc;
        char* state = getState(cp);
        printf("ID: %d, CPU Time: %d, I/O Time: %d, Arrival Time: %d, State: %s, HalfTime: %d | ",
               cp->id, cp->cpuTime, cp->ioTime, cp->arrivalTime, state, cp->halfTime);
        currNode = currNode->nextProc;
    }
    printf(" \n");
}

//TODO REMOVE
void printTurnaround(struct Node* head) {
    struct Node *currNode = head;
    while(currNode != NULL) {
        struct Process *cp = currNode->currProc;
        printf("Turnaround Process %d: %d\n", cp->id, cp->finishTime - cp->arrivalTime + 1);
        currNode = currNode->nextProc;
    }
}

void updatePrinter(struct Node** print, struct Node* ready, struct Node* blocked) {
    struct Node* readyPrinter = copyList(ready);
    struct Node* blockedPrinter = copyList(blocked);
    while (readyPrinter != NULL) {
        appendNode(print, readyPrinter->currProc);
        readyPrinter = readyPrinter->nextProc;
    }
    while (blockedPrinter != NULL) {
        appendNode(print, blockedPrinter->currProc);
        blockedPrinter = blockedPrinter->nextProc;
    }
    free(readyPrinter);
    free(blockedPrinter);
}

char* productionPrintString(struct Node *head) {
    sort(head);
    struct Node *currNode = head;
    char *output = malloc(50);
    while(currNode != NULL) {
        struct Process *cp = currNode->currProc;
        char currLine[20];
        sprintf(currLine, "%d: %s ", cp->id, getState(cp));
        strcat(output, currLine);
        currNode = currNode->nextProc;
    }
    strcat(output," \n");
    return output;
}

char* printTurnaroundString(struct Node* head) {
    struct Node *currNode = head;
    char* output = malloc(100);
    while(currNode != NULL) {
        char currLine[30];
        struct Process *cp = currNode->currProc;
        sprintf(currLine, "Turnaround Process %d: %d\n", cp->id, cp->finishTime - cp->arrivalTime + 1);
        strcat(output, currLine);
        currNode = currNode->nextProc;
    }
    return output;
}

//Algorithms
//TODO ADD STRING BUILDER FOR OUTPUT
char* firstAlgo(struct Node *head, int numOfProcs) {
    struct Node* readyQueue = NULL;
    struct Node* blockedQueue = NULL;
    struct Node* finishedQueue = NULL;
    struct Node* tempReady = NULL;
    char *outputString = malloc(1000);

    int sysCount = 0;
    int numFinished = 0;
    int timeIdle = 0;
    while (numFinished != numOfProcs) { //manually add to ready queue based on count timer
        struct Node* printList = NULL;
        struct Node* generatorNode = copyList(head); //used for adding to ready queue
        while (generatorNode != NULL) { //logic for adding to ready queue based on systime/arrival time
            if (generatorNode->currProc->arrivalTime == sysCount) {
                appendNode(&tempReady, generatorNode->currProc);
            }
            generatorNode = generatorNode->nextProc;
        }
        free(generatorNode); //no longer needed

        if (getSize(blockedQueue) != 0) {
            decIOTime(blockedQueue); //decs time for all
            struct Node* currBlock = copyList(blockedQueue);
            while (currBlock != NULL) {
                if (currBlock->currProc->ioTime == 0) { //adding to ready if done
                    stateChange(currBlock->currProc, 1); //moving state to ready
                    appendNode(&tempReady, copyProc(currBlock->currProc));
                    deleteKey(&blockedQueue, getID(currBlock->currProc)); //moving from ready to blocked
                }
                currBlock = currBlock->nextProc;
            }
            free(currBlock);
        }//handles blocked Queue checks, dec, and moving

        if (getSize(tempReady) != 0) {
            sort(tempReady);
            struct Node *tempCur = copyList(tempReady);
            while (tempCur != NULL) {
                appendNode(&readyQueue, tempCur->currProc);
                deleteKey(&tempReady, getID(tempCur->currProc));
                tempCur = tempCur->nextProc;
            }
            free(tempCur);
        }

        stateChange(readyQueue->currProc, 0); //set to running
        if (getHalfTime(readyQueue->currProc) == 0) {
            if (readyQueue->nextProc != NULL) {
                readyQueue->nextProc->currProc->state = 0;
            }

            if (getIoTime(readyQueue->currProc) == 0) { //FINISHED PROC
                setDone(readyQueue->currProc, 1);
                readyQueue->currProc->finishTime = sysCount -1;
                appendNode(&finishedQueue, copyProc(readyQueue->currProc));
                deleteFirst(&readyQueue); //proc is finished
                numFinished += 1;
                if (numFinished == numOfProcs) {
                    break;
                }
            }

            if (getIoTime(readyQueue->currProc) != 0) { //FIRST HALF TIME DONE: MOVE TO BLOCKED
                setHalfTime(readyQueue->currProc, ((int) (getCpuTime(readyQueue->currProc) * .5)));
                stateChange(readyQueue->currProc, 2); //moving state to blocked
                appendNode(&blockedQueue, copyProc(readyQueue->currProc));
                deleteFirst(&readyQueue); //moving from ready to blocked
            }

            if (readyQueue == NULL){ //if no more ready, then idle time
                timeIdle++;
            }
            else {
                setHalfTime(readyQueue->currProc, ((int) (getCpuTime(readyQueue->currProc) * .5)) - 1);
            }
            //TODO PRINTING
            char sysCountString[10];
            sprintf(sysCountString, "%d ", sysCount);
            strcat(outputString, sysCountString);
            updatePrinter(&printList, readyQueue, blockedQueue);
            strcat(outputString, productionPrintString(printList));
            sysCount++;
            continue;
        }
        //TODO PRINTING
        char sysCountString[10];
        sprintf(sysCountString, "%d ", sysCount);
        strcat(outputString, sysCountString);
        updatePrinter(&printList, readyQueue, blockedQueue);
        strcat(outputString, productionPrintString(printList));

        setHalfTime(readyQueue->currProc, readyQueue->currProc->halfTime - 1);
        sysCount++;
    } //main while loop
    //TODO PRINTING
    char finishString[20];
    sprintf(finishString, "\nFinishing Time: %d\n", sysCount -1);
    strcat(outputString, finishString);
    float util = 1 - ((float) timeIdle/(float)sysCount);
    char utilString[30];
    sprintf(utilString, "CPU utilization: %.2f\n", util);
    strcat(outputString, utilString);
    strcat(outputString, printTurnaroundString(finishedQueue));
    return outputString;
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
        //Making a proc: id, cpuTime, ioTime, arrivalTime, state, done, halftime
        struct Process *current = (struct Process*) malloc(sizeof(struct Process));
        current->id = procInfo[0];
        current->cpuTime = procInfo[1];
        current->ioTime = procInfo[2];
        current->arrivalTime = procInfo[3];
        current->state = 1;
        current->done = 0;
        current->halfTime = (int) procInfo[1] * .5;

        appendNode(&listOfProcs, current); //adding to end of list
   }

   char* outputString = malloc(1000);
   switch(scheduling) {
        case 0:
            outputString = firstAlgo(listOfProcs, numofProc);
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
//print to file
    FILE* output = fopen(filename, "w");
    fputs(outputString, output);


// close the processes file
    fclose(fp);
    fclose(output);

    return 0;
}


