//  The fitting room
//  Third COSC 6360 assignment for fall 2020 with pthreads, mutexes and condition variables
// 
//  To compile:  g++ -std=c++0x -fpermissive -pthread main.cpp -o main
//	To run: 	 ./main < sample_input.txt
//  
//  First name: Mahsa 
//  Last name:  RezaeiFiruzkuhi
//  December 2020

#include <iostream>
#include <string>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h> 
#include <vector>
using namespace std;
void error(char *msg) {
    perror(msg);
    exit(1);
}
// Static shared vaiables to show the status of all patrons 
static int nFreeCubicles = 0;   	// shared variable (static) for keeping track of Number of free cubicles
static int nArrivedPatrons = 0; 	// shared variable (static) for keeping track of Number of patrons who use or have used the fitting room
static int nWaitingPatrons = 0;		// shared variable (static) for keeping track of Number of patrons who had to wait for fitting room
static pthread_mutex_t fittingRoom = PTHREAD_MUTEX_INITIALIZER;		// creating a pthread lock and must be static
static pthread_cond_t  free_cubicle  = PTHREAD_COND_INITIALIZER;	// creating a condition variable for waiting until a cubicle is free

struct PatronDetails {				// Patron struct
    string patronName;  			// patron first name
    int patronDelay;				// Number of seconds elapsed since the arrival of the previous patron
    int nCloths_multiply_tryOntime; // The number of items brought in X a standard try-on time
};
void *Patron_function(void *arg) {
	// passing arguments to a thread (patron) function with packing them in a single struct (Patron_struct) and declaring as void
	struct PatronDetails *Patron_struct;     
    Patron_struct = (struct PatronDetails *) arg;
    pthread_mutex_lock(&fittingRoom); 					// To Request fittingRoom mutex
	cout << Patron_struct->patronName << " arrives at the fitting room."<< endl;
    nArrivedPatrons++;									// Increment number of patron arrivals
    while (nFreeCubicles == 0){							// If there is no free cubicle
        nWaitingPatrons++;								// Increment number of patrons that had to wait
        pthread_cond_wait(&free_cubicle, &fittingRoom); // Waiting on free_cubicle condition variable 
    }
	cout << Patron_struct->patronName << " gets a cubicle."<< endl;
    nFreeCubicles--;									// Updating the number of free cubicles
    pthread_mutex_unlock(&fittingRoom); 				// Release fittingRoom mutex
    sleep(Patron_struct->nCloths_multiply_tryOntime); 	// Waiting for the patron to try on all the cloths
    pthread_mutex_lock(&fittingRoom); 					// Request fitingRoom mutex
	cout << Patron_struct->patronName << " is done."<< endl;
    nFreeCubicles++;									// Updating the number of free cubicles
    pthread_cond_signal(&free_cubicle); 				// Signal a cubicle is free on the condition variable free_cubicle
    pthread_mutex_unlock(&fittingRoom); 				// Release fittingRoom mutex
    pthread_exit((void*) 0);							// Terminate self thread
}
int main(int argc, char *argv[]) {
	
	int nCubicles = 0;
	int tryOnTime;
	int Number_of_Patrons;
	int patronDelay;
	int nCloths;
	string patronName;
    pthread_mutex_init(&fittingRoom, NULL); 				// To create a mutex named "fittingRoom" and initialize it 
	cin >> nCubicles;
	cin >> tryOnTime;
	nFreeCubicles = nCubicles;								// Initializing number of free cubicles 
	cout << "\n--This fitting room has " << nCubicles <<" cubicle(s)" << endl;
	cout << "--It takes " << tryOnTime <<" time units to try on one item" << endl;
    vector <PatronDetails> Patrons_datail_vector;		
    while (cin >> patronName >> patronDelay >> nCloths)		// Reading Patrons details from the input file line by line (IO redirection) 
	{ 
		struct PatronDetails Patron_struct;					// For each input line describing a patron
		Patron_struct.patronName = patronName;				// Reading a patron first name, a time delay, and a number of clothing to try on
        Patron_struct.patronDelay = patronDelay;
        Patron_struct.nCloths_multiply_tryOntime = nCloths * tryOnTime;
        Patrons_datail_vector.push_back(Patron_struct);     // Saving the patron informations in the "Patrons_datail_vector" 
    }
    Number_of_Patrons = Patrons_datail_vector.size();
    pthread_t Patrons_threadID[Number_of_Patrons];  		// Declaring a thread ID for each patron thread and save them in the array "Patrons_threadID" 
	for (int t = 0; t < Number_of_Patrons; t++ ) 
	{
        sleep(Patrons_datail_vector[t].patronDelay); 		// Wait until arrival time of each patron to the fitting room queue (Sleep for inter-arrival delay)
		if (pthread_create(&Patrons_threadID[t], NULL, Patron_function, (void *)&Patrons_datail_vector[t])){  // Creating/starting a patron thread
            error((char *)"Error on creating thread");
        }
    }
    for (int t = 0; t < Number_of_Patrons; t++ ) {   		// Waiting until all patron threads have terminated
        pthread_join(Patrons_threadID[t], NULL); 			// Must keep track of the thread id’s of all the threads
    }
	cout << "\n\nSUMMARY:"<< endl;
    cout << nArrivedPatrons <<" Patron(s) used the fitting room."<< endl;
	cout << nArrivedPatrons - nWaitingPatrons <<" Patron(s) did not have to wait."<< endl;
	cout << nWaitingPatrons <<" Patron(s) had to wait."<< endl;
    return 0;
}
