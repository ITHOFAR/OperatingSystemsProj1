// must compile with: gcc  -std=c99 -Wall -o scheduling scheduling.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//Code of Isaac Harris: ith5263

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
    int timeRun;
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
    newProc->timeRun = proc->timeRun;
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

//GETTERS: Used for easier reading in parts of code:Not necessary
int getID(struct Process *proc) {
    return proc->id;
}

int getCpuTime(struct Process *proc) {
    return proc->cpuTime;
}

int getHalfTime(struct Process *proc) {
    return proc->halfTime;
}

int getTimeRun(struct Process *proc){
    return proc->timeRun;
}

int getIoTime(struct Process *proc) {
    return proc->ioTime;
}

//SETTERS
void setHalfTime(struct Process *proc, int num) {
    proc->halfTime = num;
}

void setTimeRun(struct Process *proc, int num) {
    proc->timeRun = num;
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

void decIOTime(struct Node* head) {
    struct Node* curr = head;
    while (curr != NULL) {
        curr->currProc->ioTime -=1;
        curr = curr->nextProc;
    }
}

char* getState(struct Process* proc) {
    char* string;
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

void sort(struct Node* head) {
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

void sortByTime(struct Node* head) {
    int swapped;
    struct Node* curr;
    struct Node* nodeL = NULL;

    if (head == NULL) {
        return;
    }

    do {
        swapped = 0;
        curr = head;
        int totalTimeCurr = (curr->currProc->cpuTime + 1)/2 + curr->currProc->halfTime;
        while (curr->nextProc != nodeL) {
            int totalTimeCurrNext = (curr->nextProc->currProc->cpuTime + 1)/2 + curr->nextProc->currProc->halfTime;
            if (totalTimeCurr > totalTimeCurrNext) {
                swap(curr, curr->nextProc);
                swapped = 1;
            }
            curr = curr->nextProc;
        }
        nodeL = curr;
    } while (swapped);
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
        sprintf(currLine, "%d:%s ", cp->id, getState(cp));
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

//Algorithms:
char* firstAlgo(struct Node *head, int numOfProcs) {
    struct Node* readyQueue = NULL;
    struct Node* blockedQueue = NULL;
    struct Node* finishedQueue = NULL; //storage of all finished procs
    struct Node* tempReady = NULL; //used to sort based on ID
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

        if (readyQueue != NULL) {
            stateChange(readyQueue->currProc, 0); //set to running

            if (getHalfTime(readyQueue->currProc) == 0) {
                if (readyQueue->nextProc != NULL) {
                    readyQueue->nextProc->currProc->state = 0;
                }

                if (getIoTime(readyQueue->currProc) == 0) { //FINISHED PROC
                    setDone(readyQueue->currProc, 1);
                    readyQueue->currProc->finishTime = sysCount - 1;
                    appendNode(&finishedQueue, copyProc(readyQueue->currProc));
                    deleteFirst(&readyQueue); //proc is finished
                    numFinished += 1;
                    if (numFinished == numOfProcs) {
                        break;
                    }
                }

                if (readyQueue != NULL) {
                    if (getIoTime(readyQueue->currProc) != 0 && getHalfTime(readyQueue->currProc) == 0) { //FIRST HALF TIME DONE: MOVE TO BLOCKED
                        setHalfTime(readyQueue->currProc, (getCpuTime(readyQueue->currProc) + 1)/2);
                        stateChange(readyQueue->currProc, 2); //moving state to blocked
                        appendNode(&blockedQueue, copyProc(readyQueue->currProc));
                        deleteFirst(&readyQueue); //moving from ready to blocked
                    }
                }

                if (readyQueue == NULL) { //if no more ready, then idle time
                    timeIdle++;
                }
                else {
                    stateChange(readyQueue->currProc, 0); //set to running
                    setHalfTime(readyQueue->currProc, getHalfTime(readyQueue->currProc) - 1);
                }
                char sysCountString[10];
                sprintf(sysCountString, "%d ", sysCount);
                strcat(outputString, sysCountString);
                updatePrinter(&printList, readyQueue, blockedQueue);
                strcat(outputString, productionPrintString(printList));
                sysCount++;
                continue;
            }
            setHalfTime(readyQueue->currProc, readyQueue->currProc->halfTime - 1);
        }
        if (readyQueue == NULL) { //if no more ready, then idle time
            timeIdle++;
        }

        char sysCountString[10];
        sprintf(sysCountString, "%d ", sysCount);
        strcat(outputString, sysCountString);
        updatePrinter(&printList, readyQueue, blockedQueue);
        strcat(outputString, productionPrintString(printList));
        sysCount++;
    } //main while loop

    char finishString[50];
    sprintf(finishString, "\nFinishing Time: %d\n", sysCount-1);
    strcat(outputString, finishString);
    float util = 1 - ((float) timeIdle/(float)sysCount);
    char utilString[50];
    sprintf(utilString, "CPU utilization: %.2f\n", util);
    strcat(outputString, utilString);
    sort(finishedQueue);
    strcat(outputString, printTurnaroundString(finishedQueue));
    return outputString;
}

char* roundAlgo(struct Node *head, int numOfProcs) {
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

        if (readyQueue != NULL) {
            if (getTimeRun(readyQueue->currProc) == 2 && readyQueue->currProc->halfTime != 0) {
                setTimeRun(readyQueue->currProc, 0);
                stateChange(readyQueue->currProc, 1); //moving state to blocked
                appendNode(&tempReady, copyProc(readyQueue->currProc));
                deleteFirst(&readyQueue);
            }
        }

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

        if (readyQueue != NULL) {
            stateChange(readyQueue->currProc, 0); //set to running

            if (getHalfTime(readyQueue->currProc) == 0) {
                if (readyQueue->nextProc != NULL) {
                    readyQueue->nextProc->currProc->state = 0;
                }

                if (getIoTime(readyQueue->currProc) == 0) { //FINISHED PROC
                    setDone(readyQueue->currProc, 1);
                    readyQueue->currProc->finishTime = sysCount - 1;
                    appendNode(&finishedQueue, copyProc(readyQueue->currProc));
                    deleteFirst(&readyQueue); //proc is finished
                    numFinished += 1;
                    if (numFinished == numOfProcs) {
                        break;
                    }
                }

                if (readyQueue != NULL) {
                    if (getIoTime(readyQueue->currProc) != 0 && getHalfTime(readyQueue->currProc) == 0) { //FIRST HALF TIME DONE: MOVE TO BLOCKED
                        setTimeRun(readyQueue->currProc, 0);
                        setHalfTime(readyQueue->currProc, (getCpuTime(readyQueue->currProc) + 1)/2);
                        stateChange(readyQueue->currProc, 2); //moving state to blocked
                        appendNode(&blockedQueue, copyProc(readyQueue->currProc));
                        deleteFirst(&readyQueue); //moving from ready to blocked
                    }
                }

                if (readyQueue == NULL) { //if no more ready, then idle time
                    timeIdle++;
                } else {
                    stateChange(readyQueue->currProc, 0); //set to running
                    setTimeRun(readyQueue->currProc, getTimeRun(readyQueue->currProc) + 1);
                    setHalfTime(readyQueue->currProc, getHalfTime(readyQueue->currProc) - 1);
                }
                char sysCountString[10];
                sprintf(sysCountString, "%d ", sysCount);
                strcat(outputString, sysCountString);
                updatePrinter(&printList, readyQueue, blockedQueue);
                strcat(outputString, productionPrintString(printList));

                sysCount++;
                continue;
            }
            setTimeRun(readyQueue->currProc, getTimeRun(readyQueue->currProc) + 1);
            setHalfTime(readyQueue->currProc, readyQueue->currProc->halfTime - 1);
        }
        if (readyQueue == NULL) { //if no more ready, then idle time
            timeIdle++;
        }

        char sysCountString[10];
        sprintf(sysCountString, "%d ", sysCount);
        strcat(outputString, sysCountString);
        updatePrinter(&printList, readyQueue, blockedQueue);
        strcat(outputString, productionPrintString(printList));

        sysCount++;
    } //main while loop

    char finishString[50];
    sprintf(finishString, "\nFinishing Time: %d\n", sysCount-1);
    strcat(outputString, finishString);
    float util = 1 - ((float) timeIdle/(float)sysCount);
    char utilString[50];
    sprintf(utilString, "CPU utilization: %.2f\n", util);
    strcat(outputString, utilString);
    sort(finishedQueue);
    strcat(outputString, printTurnaroundString(finishedQueue));
    return outputString;
}

char* shortestAlgo(struct Node *head, int numOfProcs) {
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

        if (readyQueue != NULL) {
            struct Node* currNode = readyQueue;
            while (currNode != NULL) {
                currNode->currProc->state = 1;
                currNode = currNode->nextProc;
            }
            free(currNode);

            sort(readyQueue); //sorting by CPUID
            sortByTime(readyQueue); //sorting on time left
            stateChange(readyQueue->currProc, 0); //set to running

            if (getHalfTime(readyQueue->currProc) == 0) {
                if (readyQueue->nextProc != NULL) {
                    readyQueue->nextProc->currProc->state = 0;
                }

                if (getIoTime(readyQueue->currProc) == 0) { //FINISHED PROC
                    setDone(readyQueue->currProc, 1);
                    readyQueue->currProc->finishTime = sysCount - 1;
                    appendNode(&finishedQueue, copyProc(readyQueue->currProc));
                    deleteFirst(&readyQueue); //proc is finished
                    numFinished += 1;
                    if (numFinished == numOfProcs) {
                        break;
                    }
                }

                if (readyQueue != NULL) {
                    if (getIoTime(readyQueue->currProc) != 0 && getHalfTime(readyQueue->currProc) == 0) { //FIRST HALF TIME DONE: MOVE TO BLOCKED
                        setHalfTime(readyQueue->currProc, (getCpuTime(readyQueue->currProc) + 1)/2);
                        stateChange(readyQueue->currProc, 2); //moving state to blocked
                        appendNode(&blockedQueue, copyProc(readyQueue->currProc));
                        deleteFirst(&readyQueue); //moving from ready to blocked
                    }
                }

                if (readyQueue == NULL) { //if no more ready, then idle time
                    timeIdle++;
                } else {
//                    stateChange(readyQueue->currProc, 0); //set to running
                    setHalfTime(readyQueue->currProc, getHalfTime(readyQueue->currProc) - 1);
                }
                char sysCountString[10];
                sprintf(sysCountString, "%d ", sysCount);
                strcat(outputString, sysCountString);
                updatePrinter(&printList, readyQueue, blockedQueue);
                strcat(outputString, productionPrintString(printList));
                sysCount++;
                continue;
            }
            setHalfTime(readyQueue->currProc, readyQueue->currProc->halfTime - 1);
        }
        if (readyQueue == NULL) { //if no more ready, then idle time
            timeIdle++;
        }

        char sysCountString[10];
        sprintf(sysCountString, "%d ", sysCount);
        strcat(outputString, sysCountString);
        updatePrinter(&printList, readyQueue, blockedQueue);
        strcat(outputString, productionPrintString(printList));
        sysCount++;
    } //main while loop

    char finishString[50];
    sprintf(finishString, "\nFinishing Time: %d\n", sysCount-1);
    strcat(outputString, finishString);
    float util = 1 - ((float) timeIdle/(float)sysCount);
    char utilString[50];
    sprintf(utilString, "CPU utilization: %.2f\n", util);
    strcat(outputString, utilString);
    sort(finishedQueue);
    strcat(outputString, printTurnaroundString(finishedQueue));
    return outputString;
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
    if( !(fp = fopen(argv[2],"r"))) {
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
        //Making a proc: id, cpuTime, ioTime, arrivalTime, state, done, halftime, finishTime, timeRun
        struct Process *current = (struct Process*) malloc(sizeof(struct Process));
        current->id = procInfo[0];
        current->cpuTime = procInfo[1];
        current->ioTime = procInfo[2];
        current->arrivalTime = procInfo[3];
        current->state = 1;
        current->done = 0;
        current->halfTime = (procInfo[1] + 1)/2;
        current->finishTime = 0;
        current->timeRun = 0;

        appendNode(&listOfProcs, current); //adding to end of list
   }

   char* outputString;
   switch(scheduling) {
        case 0:
            outputString = firstAlgo(listOfProcs, numofProc);
            break;
        case 1:
            outputString = roundAlgo(listOfProcs, numofProc);
            break;
        case 2:
            outputString = shortestAlgo(listOfProcs, numofProc);
            break;
        default:
            printf("Scheduling type is not supported, \n Please input: 0, 1, or 2.");
            exit(1);
   }
    free(listOfProcs);

// form the output file name
    sprintf(filename,"%d-%s",scheduling, argv[2]);
//print to file
    FILE* output;
//    if( !(output = fopen("/Users/ithofar/Downloads/myExfile.txt","w"))) //For home testing: not CIMS
//    {
//        printf("Cannot open file %s\n", "/Users/ithofar/Downloads/myExfile.txt");
//        exit(1);
//    }
    if( !(output = fopen(filename,"w"))) //for production
    {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }
    fputs(outputString, output);


// close the processes file
    fclose(fp);
    fclose(output);
    free(outputString);

    return 0;
}


