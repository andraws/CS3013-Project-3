/*
Andrew Shanaj
Testing simple Concurrency 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
//
 //================================================================================================

typedef enum type{
  PIRATE = 0,
  NINJA,
  MAX
}type_t;


typedef struct actors{
  type_t type; // 0 for pirate, 1 for ninja
  int ID;
  int hasEntered;
  int DressTime;
}actor_t;

int currentActorType;
int countInStore;
int teams;

sem_t StoreMaxCount; // protect the max number of threads in store
sem_t ProtectCount; // protects the number of ninjas or pirates coming in
sem_t QueueProtect; // protect the queue when we edit/check

int *queue;
int numPCreated = 0;
int numNCreated = 0;
int currID;
int nextActor;
//================================================================================================

void initSems(void); // init all the semathors
void SetUpActors(actor_t* actor, int ID); // init actors
void SendActors(int numNinjas, int numPirates);
void ResetStore(void);
void* Dress(void *args);
void printType(int type, int ID, int InorOut);

//================================================================================================


void initSems(void){
  sem_init(&StoreMaxCount, 0, teams);
  sem_init(&ProtectCount, 0, teams);
  sem_init(&QueueProtect, 0, 1);
}

void ResetStore(void){
  if(&StoreMaxCount)
    sem_close(&StoreMaxCount);
  if(&ProtectCount)
    sem_close(&ProtectCount);
  if(&QueueProtect)
    sem_close(&QueueProtect);
}

void SetUpActor(actor_t *actor, int ID, int numNinjas, int numPirates){
  int type = rand() % 2;

  if(type && !(numNinjas == numNCreated)){
    actor->type = type;
    actor->hasEntered = 0;
    actor->ID = ID;
    actor->DressTime = 1;
    numNCreated++;
  }else if(!type && !(numPirates == numPCreated)){
    actor->type = type;
    actor->hasEntered = 0;
    actor->ID = ID;
    actor->DressTime = 1;
    numPCreated++;
  }

}

void SendActors(int numNinjas, int numPirates){
  pthread_t* threads;
  actor_t* ActorsArgs;
  int numTotal = (numPirates + numNinjas);
  int ID;

  threads = (pthread_t*)malloc(numTotal*sizeof(pthread_t));
  if(threads == NULL){
    printf("Threads malloc failed\n");
    free(threads);
  }
  ActorsArgs = (actor_t*)malloc(numTotal*sizeof(actor_t));
  if(ActorsArgs == NULL){
    printf("ARGS malloc failed\n");
    free(ActorsArgs);
  }
  queue = (int*)malloc(sizeof(int)*numTotal);

  currID = 0;
  nextActor = 0;

  for(ID=0; ID<=numTotal; ID++){
    SetUpActor(&ActorsArgs[ID],ID, numNinjas, numPirates);
    if(pthread_create(&threads[ID],NULL,Dress,(void*)&ActorsArgs[ID]) != 0){
      ResetStore();
    }
  }

  for(ID=0; ID<=numTotal; ID++){
    pthread_join(threads[ID],NULL);
  }
}

void updateQueue(actor_t *ActorsArgs){
  sem_wait(&QueueProtect);
  if(0 == currID)
    currentActorType = ActorsArgs->type;
  queue[currID++] = ActorsArgs->ID;
  sem_post(&QueueProtect);
}

void* Dress(void *args){
  actor_t *CurrentActor = (actor_t*)args;

  if(CurrentActor){
    updateQueue(CurrentActor);
    while(CurrentActor->hasEntered != 1){

        if(queue[nextActor] == CurrentActor->ID){
          // let the right type of actor in
            if(CurrentActor->type == currentActorType){ // update count
              sem_wait(&ProtectCount);
              countInStore++;
              nextActor++;
              sem_post(&ProtectCount);
            }else{
              continue; // else next direction isnt the same so just continue
            }
            // Allow only the max amount of people in the store
            sem_wait(&StoreMaxCount);
            printType(CurrentActor->type,CurrentActor->ID,1);
            sleep(CurrentActor->DressTime);
            printType(CurrentActor->type,CurrentActor->ID,0);
            sem_wait(&ProtectCount); // take them out of the store
            countInStore--;
            if(countInStore == 0){
              currentActorType = ((currentActorType == PIRATE)?NINJA:PIRATE);
              printf("Changed type of actor entering\n");
            }
            sem_post(&ProtectCount);
            sem_post(&StoreMaxCount);

            CurrentActor->hasEntered = 1;

        }
    }
  }else{
    printf("Args are null\n");
  }
  return NULL;
}

void printType(int type, int ID, int InorOut){
  if(type && InorOut){
    printf("Ninja with ID [%d] has entered the store\n", ID);
  }else if(type && !InorOut){
    printf("Ninja with ID [%d] has left the store\n", ID);
  }else if(!type && InorOut){
    printf("Pirate with ID [%d] has entered the store\n",ID);
  }else if(!type && !InorOut){
    printf("Pirate with ID [%d] has left the store\n",ID);
  }
}

int main(int argc, char *argv[]){
  srand(time(NULL));
  printf("Hello! Let's try to prevent WWWIII on these poor pirates and ninjas.\n");
        printf("Checking command line args:...\n\n");
        if(argc != 4){
          printf("Not the right amount of args given");
          return -1;
        }

        teams = atoi(argv[1]);
        int numPirates = atoi(argv[2]);
        int numNinjas = atoi(argv[3]);

        printf("Good job, entered valid inputs:\n");
        printf("Number of Teams: %d\n", teams);
        printf("Number of Pirates: %d\n", numPirates);
        printf("Number of Ninjas: %d\n", numNinjas);
        initSems();
        SendActors(numNinjas,numPirates);
        printf("Number of Ninjas: [%d], Number of Pirates: [%d]\n",numNCreated, numPCreated );
        printf("Finished\n");

}
