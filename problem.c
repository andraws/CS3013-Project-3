#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/*
Project 3: threads and Syncronization

Library: pthread
    - Mutexes:
        pthread_mutex_lock & pthread_mutex_unlocked
    - Semaphores (condition)
        sem_wait, sem_post, sem_init

Goal: Create threads for each of our actors and debug synchronously.
    - Each thread uses randomness for scheduling (for testing use same seed)


Command Line Args:
    1. Number of costuming teams (2 min, 4 max)
    2. Number of pirates (10-50)
    3. Number of ninjas (10-50)
    4. Average time pirates spend in costume shop
    5. Average time ninjas spend in costume shop
    6. Average time pirates spend before revisiting costume shop
    7. Average time ninjas spend before revisiting costume shop

Problem:
    Any any given day, 10-50 pirates or ninjas enter our store
    Each staff can take care of appropriate amount of pirates or ninjas but never both
    Pirates and ninjas should identify themselves and which team it is using when entering or leaving
    Arrivals will be at random, and the amount of time that any individual takes to be outfited is random
    At end of custome session, there is a 25% chance they will come back
    Department keeps stats for billing, 1 gold, if wait is over 30 mins prior to entering, it is free

    Each Pirate and ninja is represented as a unique thread
    Each execution represents the operation of costume shop over a single day
    Code that creates the requisite number of ninjas and pirate threads and use either semaphores or locks to synchronize costume department
    Costume prep using sleep() with val from random distribution

Costume Department Usage Stats for Billing
    Bill for each Pirates and Ninjas
        -Number of visits
        -Amount of time of each visit
        -Wait times
        -Total gold owed


    Expenses & Profits
        -Each time costs 5 gold per day
        -Amount of time each team was busy
        -Amount of time each team was free
        -average queue length
        -Gross revenue (amount of gold)
        -Gold-per-visit
        -Profit

Strategy:
    50/50 chance of making a pirate or ninja thread, add in the list of actors
    Who ever comes first claims their turn
    for any actor (thread), they can enter the building if:
        1. There is no opponent in the building
        2. There is available spot in the building
            A) An ally holds the turn and the thread is first in queue amongst allies
            or
            B) The thread is the first in entire Queue

    We need to change turn if amount of time reaches 20 minutes?

DataTypes?:

2 avgTimeList: 1 for pirate and another for ninja
    - Sumation of list divided by number of pirates or ninjas created equals given AvgTime from Args

2 avgRevisitTime list: 1 for pirate and another for ninja
    - Sumation of this list divided by number of pirates or ninjas revisiting equals given AvgTime from Args

2 Structs
    - Struct Actors{
        Actor = (Ninja = 0, Pirate =1)
        ActorID = ID where our actor is located in the avgTimeList;
        Time Inside = avgTimeList[ID];
        Time before Revisit  = avgRevisitTime[ID]; (If -1, won't be revisitng);
        Times visited;
        Gold Owned
    }
    Actor Actors[#P+#N] - list holding all Actors (maybe make a queue?)
    Or
        Actor Pirates[#P] & Actor Ninjas[#N]

    -Struct Team{
        Avg Time Busy;
        Avg Time Free;
        Avg Que length;
        Amount of gold;
        Gold per visit;
    }
    Team Department[# of teams] - list holding all teams in department;

State of Actor? Not ready, Waiting(in queue), InShop

Questions:

How many threads can we create?

Can we create each actor right away with its own thread before passing them into the department?
Do the amount of pirates or ninjas in the department depend on the amount of teams?
*/

/* Global Variables, Arrays, Structs */
typedef struct{
  int avgBusy; // avg time the team was busy
  int avgFree; // avg time the team was free
  int avgQueue; // avg queue length
  int gold; // amount of gold collected
  int visits; // amount of visits it got
}team;

team* Department;

typedef struct{
  int type; //1 for ninja, 0 for pirate
  int tid; //thread id
  int avgCosTime; // average costume time
  int avgAT; //average arrival time
  int comeBack; //if actor will come back that day
}actor;

// Helper Functions
//=========================================================================================================

// Functions to get 1/4 chance.
int rand50();
int rand25();

//function to calculate usage statstics for billing
//p / n spend 1 gold per minute inside costume Department
// but if pirates wait more than 30 minutes, custome = Free

// calculate and print itemized bill for both pirates and Ninjas
// list number of visits, amount of time each visit, wait times, gold owed to costum department
int itemizedBill(int numvisits, int times[], int waittimes[], int goldowed){

  return 1;
}

//calculate department Expenses
// 5 gold pieces of day to staff each team
// print amount of time each team was busy, amount of time each team was free
// average queue length, gross revenue, gold-per-visit (amount of gold / # visits)
// total profits
int deptExp(int days, int timesbusy[], int timesfree[], int grossrevenue){

  return 1;
}

//=========================================================================================================

//TODO: Run costume department simulator


//=========================================================================================================

/* Main Functions */
int main(int argc, char *argv[]) {
  printf("Hello! Let's try to prevent WWWIII on these poor pirates and ninjas.\n");
  printf("Checking command line args:...\n\n");
    if (argc == 8) { //check valid # of input
        /* Check that there are no more than 4 teams, 50 pirates or 50 ninjas */
        if (atoi(argv[1]) > 4 || atoi(argv[2]) > 50 || atoi(argv[3]) > 50) {
            int checker1 = atoi(argv[1]);
            int checker2 = atoi(argv[2]);
            int checker3 = atoi(argv[3]);
            if(checker1 > 4){
                printf("Error: Invalid Input. There can only be a max of 4 teams.\n");
            }
            if(checker2 > 50 || checker3 > 50){
                printf("Error: Invalid Input. There can only be a max of 50 pirates or ninjas.\n");
          }
          exit(1);
        }
        /* Check that are are no less than 2 teams, 10 pirates, or 10 ninjas */
        if (atoi(argv[1]) < 2 || atoi(argv[2]) < 10 || atoi(argv[3]) < 10) {
            int checker1 = atoi(argv[1]);
            int checker2 = atoi(argv[2]);
            int checker3 = atoi(argv[3]);
            if(checker1 < 2){
                printf("Error: Invalid Input. There can only be a min of 2 teams.\n");
            }
            if(checker2 < 10 || checker3 < 10){
                printf("Error: Invalid Input. There can only be a min of 10 pirates or ninjas.\n");
            }
          exit(1);
        }


        int teams = atoi(argv[1]); //teams
        int numPirates = atoi(argv[2]); //number of pirates (10-50)
        int numNinjas = atoi(argv[3]); //number of ninjas (10-50)
        int avgTPirate = atoi(argv[4]); //average costume time pirate (seconds)
        int avgTNinja = atoi(argv[5]); //average costume time ninja (seconds)
        int avgATPirate = atoi(argv[6]); //average arrival time pirate (waiting)
        int avgATNinja = atoi(argv[7]); //average arrival time ninjas (waiting)


        printf("Good job, entered valid inputs:\n");
        printf("Number of Teams: %d\n", teams);
        printf("Number of Pirates: %d\n", numPirates);
        printf("Number of Ninjas: %d\n", numNinjas);
        printf("Average costume time for Pirates: %d\n", avgTPirate);
        printf("Average costume time for Ninjas: %d\n", avgTNinja);
        printf("Average arrival time for Pirates: %d\n", avgATPirate);
        printf("Average arrival time for Pirates: %d\n", avgATNinja);

        Department = (team*)malloc(teams * sizeof(*Department));

        //Initialize pirate threads
        for(int i = 0; i <= numPirates; i++){
          actor * pirate = malloc(sizeof(actor *) * 20);
          pirate->type = 1;
          pirate->tid = i;
          pirate->avgCosTime = avgTPirate;
          pirate->avgAT = avgATPirate;

          int coming_back = rand25(); //need to be a 25% chance of coming back
          pirate->comeBack = coming_back;

          // TODO: make this a thread (pirates)
        }
        //Initialize ninja threads
        for(int i = 0; i <= numNinjas; i++){
          actor * ninja = malloc(sizeof(actor *) * 20);
          ninja->type = 0;
          ninja->tid = i;
          ninja->avgCosTime = avgTNinja;
          ninja->avgAT = avgATNinja;

          int coming_back = drand48(); //need to be a 25% chance of coming back
          ninja->comeBack = coming_back;

          //TODO: make this a thread (ninjas)
        }

    } else { //Invalid Command Args
        printf("Error: Invalid Command line args:\n");
        printf("./problem #teams, #pirates, #ninjas, #avgTPirate, avgTNinja, avgATPirate, avgATNinja\n\n");
        exit(1);
    }


    return 0;
}

int rand50(){
  return rand() & 1;
}

int rand25(){
    return !(rand50() | rand50());
}
