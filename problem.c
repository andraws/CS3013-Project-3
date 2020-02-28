/*
   Andrew Shanaj
   Roman Wicky
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//================================================================================================

typedef enum type {
        PIRATE = 0,
        NINJA,
        MAX
}type_t;


typedef struct actors {
        type_t type; // 0 for pirate, 1 for ninja
        int ID;
        int typeID;
        int hasEntered;
        int DressTime[10];
        int TimesReEntering;
        int teamUsed;
        int waitTime[10];
        int TimesEntered;
        int totalVisits;
}actor_t;


int currentActorType;
int countInStore;
int avgDressTimePirate;
int avgDressTimeNinja;
int avgRoamTimePirate;
int avgRoamTimeNinja;
int Vtime = 0;
int teams;
int totalGoldPirate;
int totalGoldNinja;
int visits;

sem_t StoreMaxCount; // protect the max number of threads in store
sem_t ProtectCount; // protects the number of ninjas or pirates coming in
sem_t QueueProtect; // protect the queue when we edit/check
sem_t Print;
sem_t Push;
sem_t WaitTime;
sem_t CheckRejoin;

int *queue;
int numPCreated = 0;
int numNCreated = 0;
int currID;
int nextActor;

//=====================================NODES =====================================================
typedef struct node {
        struct actors actor;
        struct node *next;
}node_t;

node_t* Piratehead =NULL;
node_t* Ninjahead = NULL;
node_t* waitingLine = NULL;

void push(node_t **head, actor_t tActor);
void print_list(node_t *head);
struct actors remove_by_id(node_t **head, int ID);
void pop(node_t **head);

//================================================================================================

void initSems(void); // init all the semathors
void SetUpActor(actor_t *actor, int ID, int numNinjas, int numPirates); // init actors
void SetUpTeams(int numTeams);
void SendActors(int numNinjas, int numPirates);
void ResetStore(void);
void* Dress(void *args);
void printType(int type, int ID, int InorOut, int teamNum);
int WhatTypeToMake(int numNinjas, int numPirates);
int OneFourth();
int TimesEnter();
void updateData(node_t *head, actor_t *actComp);

//================================================================================================

int OneFourth(){
        return !(rand() & 1 | rand() & 1);
}

int TimesEnter(){
        int val = OneFourth();
        int x=0;

        while(val) {
                x++;
                val = OneFourth();
        }
        return x;
}

int  getRandom(int avg){
        double a = drand48();
        double b = drand48();

        float z = sqrt(-2 * log(a)) * cos(2 * M_PI * b);
        float num = ((avg/2) * z) + avg;
        if (num < 1) {
                num = 1;
        }

        num = (int) floor(num);
        //printf("%f\n", num);
        return num;
}


void updateData(node_t *head, actor_t *actComp){
        node_t *current = head;
        node_t *temp = current;
        while(current != NULL) {
                if(current->actor.ID == actComp->ID) {
                        current->actor = *actComp;
                        //  current->actor.teamUsed = teamService;
                }
                current = current->next;
        }
        head = temp;
        free(current);
        sem_post(&Push);
}

struct actors get_by_id(node_t *head, int ID){
        node_t *current = head;

        while(current != NULL) {
                if(current->actor.ID == ID) {
                        printf("Id: [%d], Dress Time: [%d], Used Team: [%d]\n",current->actor.ID, current->actor.DressTime[current->actor.TimesEntered], current->actor.teamUsed);
                }
                current = current->next;
        }
        free(current);
}

// adding item to beginning of list
void push(node_t **head, actor_t tActor){
        sem_wait(&Push);
        node_t *new_node;
        new_node = (node_t*)malloc(sizeof(node_t));
        new_node->actor = tActor;
        new_node->next = *head;
        *head = new_node;
        sem_post(&Push);
}

//print list
void print_list(node_t *head){
        node_t *current = head;
        while(current != NULL) {
                if(current->actor.type) {
                        int totalPrice;
                        printf("Pirate: [%d]\n",current->actor.typeID);
                        for(int i = 0; i < current->actor.TimesEntered; i++) {
                                int paying = current->actor.waitTime[i];
                                totalGoldPirate += paying;
                                printf("Visit %d Waited for %d minutes and was in shop for %d\n", (i+1), current->actor.waitTime[i], current->actor.DressTime[i]);
                                totalPrice = totalPrice + current->actor.waitTime[i];
                        }
                        printf("Total Number of Visits: %d. Cost for Pirate %d: %d\n\n", current->actor.totalVisits, current->actor.typeID, totalPrice);
                }else{
                        int totalPrice;
                        printf("Ninja: [%d]\n",current->actor.typeID);
                        for(int i = 0; i < current->actor.TimesEntered; i++) {
                                int paying = current->actor.waitTime[i];
                                totalGoldNinja += paying;
                                printf("Visit %d Waited for %d minutes and was in shop for %d\n", (i+1), current->actor.waitTime[i], current->actor.DressTime[i]);
                                totalPrice = totalPrice + current->actor.waitTime[i];
                        }
                        printf("Total Number of Visits: %d. Cost for Ninja %d: %d\n\n", current->actor.totalVisits, current->actor.typeID, totalPrice);
                }
                current = current->next;
        }
}

void initSems(void){
        sem_init(&StoreMaxCount, 0, (teams));
        sem_init(&ProtectCount, 0, (teams));
        sem_init(&QueueProtect, 0, 1);
        sem_init(&Print,0,1);
        sem_init(&Push,0,1);
        sem_init(&CheckRejoin,0,1);
        sem_init(&WaitTime,0,1);
}


void ResetStore(void){
        if(&StoreMaxCount)
                sem_close(&StoreMaxCount);
        if(&ProtectCount)
                sem_close(&ProtectCount);
        if(&QueueProtect)
                sem_close(&QueueProtect);
        if(&Print)
                sem_close(&Print);
        if(&Push)
                sem_close(&Push);
}

int WhatTypeToMake(int numNinjas, int numPirates){
        if(numNinjas == 0 && numPirates == 0) {
                return -1;
        }
        if(numNinjas == numNCreated) {
                return 0;
        }else if(numPirates == numPCreated) {
                return 1;
        }else{
                return rand() % 2;
        }
}

void SetUpActor(actor_t *actor, int ID, int numNinjas, int numPirates){
        int type = WhatTypeToMake(numNinjas, numPirates);
        if(type)
        { // creating a ninja
                actor->type = type;
                actor->TimesEntered = 1;
                actor->hasEntered = 0;
                actor->ID = ID;
                actor->DressTime[0] = getRandom(avgDressTimeNinja);
                actor->typeID = numNCreated;
                actor->TimesReEntering = TimesEnter();
                //  sem_wait(&Push);
                push(&Ninjahead, *actor);
                //  sem_post(&Push);
                numNCreated++;
        }
        else if(!type)
        { // creating a pirate
                actor->type = type;
                actor->hasEntered = 0;
                actor->TimesEntered = 1;
                actor->ID = ID;
                actor->DressTime[0] = getRandom(avgDressTimePirate);
                actor->typeID = numPCreated;
                actor->TimesReEntering = TimesEnter();
                //  sem_wait(&Push);
                push(&Piratehead, *actor);
                //  sem_post(&Push);
                numPCreated++;
        }
}

void SendActors(int numNinjas, int numPirates){
        pthread_t* threads;
        actor_t* ActorsArgs;
        int numTotal = (numPirates + numNinjas);
        int ID;

        threads = (pthread_t*)malloc(numTotal*sizeof(pthread_t));
        if(threads == NULL) {
                printf("Threads malloc failed\n");
                free(threads);
        }
        ActorsArgs = (actor_t*)malloc(numTotal*sizeof(actor_t));
        if(ActorsArgs == NULL) {
                printf("ARGS malloc failed\n");
                free(ActorsArgs);
        }
        queue = (int*)malloc(sizeof(int)*(numTotal*2));


        currID = 0;
        nextActor = 0;

        for(ID=0; ID<numTotal; ID++) {
                SetUpActor(&ActorsArgs[ID],ID, numNinjas, numPirates);
                if(pthread_create(&threads[ID],NULL,Dress,(void*)&ActorsArgs[ID]) != 0) {
                        ResetStore();
                }
        }

        for(ID=0; ID<numTotal; ID++) {
                pthread_join(threads[ID],NULL);
        }
}

void updateQueue(actor_t *ActorsArgs){
        sem_wait(&QueueProtect);
        if(0 == currID) {
                currentActorType = ActorsArgs->type;
        }
        queue[currID++] = ActorsArgs->ID;
        sem_post(&QueueProtect);
}


void* Dress(void *args){
        actor_t *CurrentActor = (actor_t*)args;

        if(CurrentActor) {
                updateQueue(CurrentActor);
                while(CurrentActor->hasEntered != 1) {
                        if(queue[nextActor] == CurrentActor->ID) {
                                // let the right type of actor in
                                //    while(CurrentActor->TimesReEntering != -1){
                                if(CurrentActor->type == currentActorType) { // update count
                                        sem_wait(&ProtectCount);
                                        countInStore++;
                                        CurrentActor->teamUsed = countInStore%(teams);
                                        nextActor++;
                                        sem_post(&ProtectCount);
                                }else{
                                        continue; // else next direction isnt the same so just continue
                                }
                                // Allow only the max amount of people in the store
                                sem_wait(&StoreMaxCount);
                                printType(CurrentActor->type,CurrentActor->typeID,1, CurrentActor->teamUsed);
                                CurrentActor->totalVisits+=1;
                                sleep(CurrentActor->DressTime[CurrentActor->TimesEntered]);
                                sem_wait(&WaitTime);
                                Vtime = Vtime + CurrentActor->DressTime[CurrentActor->TimesEntered];
                                //CurrentActor->waitTime = Vtime;
                                CurrentActor->waitTime[CurrentActor->TimesEntered] = Vtime;
                                updateData(Piratehead, CurrentActor);
                                updateData(Ninjahead, CurrentActor);
                                sem_post(&WaitTime);
                                printType(CurrentActor->type,CurrentActor->typeID,0, CurrentActor->teamUsed);
                                  CurrentActor->totalVisits+=1;
                                sem_wait(&ProtectCount); // take them out of the store
                                countInStore--;
                                if(countInStore == 0) {
                                        currentActorType = !currentActorType;
                                        printf("Changed type of actor entering\n");
                                }
                                sem_post(&ProtectCount);
                                sem_post(&StoreMaxCount);
                                if(CurrentActor->hasEntered < CurrentActor->TimesReEntering) {
                                        //    sem_wait(&CheckRejoin);
                                        printf("[%d] Will Re enter\n",CurrentActor->typeID);
                                        actor_t *sendActor = CurrentActor;
                                        sendActor->TimesReEntering--;
                                        sendActor->TimesEntered++;
                                        CurrentActor->DressTime[CurrentActor->TimesEntered] = getRandom(avgDressTimePirate);
                                        Dress((void*)CurrentActor);
                                }else
                                        CurrentActor->hasEntered++;
                        }
                }
        }else{
                printf("Args are null\n");
        }
        return NULL;
}


void printType(int type, int ID, int InorOut, int teamNum){
        sem_wait(&Print);
        if(type && InorOut) {
                printf("Ninja [%d] entering shop with team [%d]\n", ID, teamNum);
        }else if(type && !InorOut) {
                printf("Ninja [%d] leaving shop with team [%d]\n", ID, teamNum);
        }else if(!type && InorOut) {
                printf("Pirate [%d] entering shop with team [%d]\n",ID, teamNum);
        }else if(!type && !InorOut) {
                printf("Pirate [%d] leaving shop with team [%d]\n",ID, teamNum);
        }
        sem_post(&Print);

}

void printDepStats(int teams){
        int costTeams = teams * 5;
        printf("All the Ninjas cost %d gold pieces\n", totalGoldNinja);
        printf("All the Pirates cost %d gold pieces\n", totalGoldPirate);
        printf("Expenses for Employing the costume teams is: %d gold pieces\n", costTeams);
        // Add each team busy and free
        // add averageq queue length


        int totalRev = (int)(totalGoldNinja + totalGoldPirate) - costTeams;
        int grossRev = totalGoldNinja + totalGoldPirate;
        int averageGoldPerVisit = grossRev / visits;
        printf("Gross Revenue is %d gold pieces\n", grossRev);
        printf("Gold Per Visit: %d\n", averageGoldPerVisit);
        printf("Total Revenue: %d\n", totalRev);

}

/* Main Functions */
int main(int argc, char *argv[]) {
        srand(time(NULL));
        printf("Hello! Let's try to prevent WWWIII on these poor pirates and ninjas.\n");
        printf("Checking command line args:...\n\n");
        if (argc == 8) { //check valid # of input

                /* Check that there are no more than 4 teams, 50 pirates or 50 ninjas */
                if (atoi(argv[1]) > 4 || atoi(argv[2]) > 50 || atoi(argv[3]) > 50) {
                        int checker1 = atoi(argv[1]);
                        int checker2 = atoi(argv[2]);
                        int checker3 = atoi(argv[3]);
                        if(checker1 > 4) {
                                printf("Error: Invalid Input. There can only be a max of 4 teams.\n");
                        }
                        if(checker2 > 50 || checker3 > 50) {
                                printf("Error: Invalid Input. There can only be a max of 50 pirates or ninjas.\n");
                        }
                        exit(1);
                }
                /* Check that are are no less than 2 teams, 10 pirates, or 10 ninjas */
                if (atoi(argv[1]) < 2 || atoi(argv[2]) < 10 || atoi(argv[3]) < 10) {
                        int checker1 = atoi(argv[1]);
                        int checker2 = atoi(argv[2]);
                        int checker3 = atoi(argv[3]);
                        if(checker1 < 2) {
                                printf("Error: Invalid Input. There can only be a min of 2 teams.\n");
                        }
                        if(checker2 < 10 || checker3 < 10) {
                                printf("Error: Invalid Input. There can only be a min of 10 pirates or ninjas.\n");
                        }
                        exit(1);
                }
        }

        teams = atoi(argv[1]); //teams
        int numPirates = atoi(argv[2]); //number of pirates (10-50)
        int numNinjas = atoi(argv[3]); //number of ninjas (10-50)


        printf("Good job, entered valid inputs:\n");
        printf("Number of Teams: %d\n", teams);
        printf("Number of Pirates: %d\n", numPirates);
        printf("Number of Ninjas: %d\n", numNinjas);

        avgDressTimePirate = atoi(argv[4]);
        avgDressTimeNinja = atoi(argv[5]);
        avgRoamTimePirate = atoi(argv[6]);
        avgRoamTimeNinja = atoi(argv[7]);

        initSems();
        SendActors(numNinjas,numPirates);

        printf("\n--End of Simmulation--\n");

        printf("Printing Pirates\n");
        print_list(Piratehead);
        printf("Printing Ninjas\n");
        print_list(Ninjahead);
        printf("Finished\n");

        return 0;
}
