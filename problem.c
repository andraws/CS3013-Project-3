#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Global Variables, Arrays, Structs */
typedef struct {
        int avgBusy; // avg time the team was busy
        int avgFree; // avg time the team was free
        int avgQueue; // avg queue length
        int gold; // amount of gold collected
        int visits; // amount of visits it got
}team;

team* Department;

int freeStaff[4]; //on update, -1 = that staff is busy
int teams;

pthread_mutex_t checkStaff = PTHREAD_MUTEX_INITIALIZER;


typedef struct {
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
int itemizedBill(team ateam){

        return 1;
}

//calculate department Expenses
// 5 gold pieces of day to staff each team
// print amount of time each team was busy, amount of time each team was free
// average queue length, gross revenue, gold-per-visit (amount of gold / # visits)
// total profits
int deptExp(team ateam){

        return 1;
}


void *costumeDept(void* args){
        pthread_mutex_lock(&checkStaff);
        printf("Teams: %d\n",teams);
        for(int x=0; x<teams; x++) {
                printf("Hey\n" );
        }
        pthread_mutex_unlock(&checkStaff);
}

void MasterQueue(){
        int val = rand50();
        int changedVal = 0;
        while(1) {
                pthread_t i;
                if(!changedVal) {
                        val = rand50();
                }
                if(val) {
                        if(/*piratequeue is empty*/) {
                                val = 0;
                                changedVal = 1;
                        }
                        changedVal = 0;

                        //  pthread_create(&i, NULL, &costumeDept, &pirate.dequeue);
                        //  pthread_join(i, NULL);
                }else if(val == 0) {
                        if(/*ninja queue is empty*/) {
                                val = 1;
                                changedVal = 1;
                        }
                        changedVal = 0;
                        //  pthread_create(&i, NULL, &costumeDept, &ninja.dequeue);
                        //  pthread_join(i, NULL);
                }
                if(/*pirate queue is empty && ninja queue is empty */) {
                        // the day is over
                        break;
                }
        }


}


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


                teams = atoi(argv[1]); //teams
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
                printf("Average arrival time for Ninjas: %d\n", avgATNinja);

                Department = (team*)malloc(teams * sizeof(*Department));

                for(int i = 0; i < teams; i++) {
                        freeStaff[i] = 1;
                }
                //Initialize pirate
                for(int i = 0; i < numPirates; i++) {
                        actor * pirate = malloc(sizeof(actor *) * 20);
                        pirate->type = 1;
                        pirate->tid = i;
                        pirate->avgCosTime = avgTPirate;
                        pirate->avgAT = avgATPirate;

                        int coming_back = rand25(); //need to be a 25% chance of coming back
                        pirate->comeBack = coming_back;
                        // TODO store in linked list
                }
                //Initialize ninja
                for(int i = 0; i < numNinjas; i++) {
                        actor * ninja = malloc(sizeof(actor *) * 20);
                        ninja->type = 0;
                        ninja->tid = i;
                        ninja->avgCosTime = avgTNinja;
                        ninja->avgAT = avgATNinja;

                        int coming_back = drand48(); //need to be a 25% chance of coming back
                        ninja->comeBack = coming_back;
                        // TODO store in linked list
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
