#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "museumsim.h"

//
// In all of the definitions below, some code has been provided as an example
// to get you started, but you do not have to use it. You may change anything
// in this file except the function signatures.
//
#define VISITORS_PER_GUIDE 10
#define GUIDES_ALLOWED_INSIDE 2

struct shared_data {
	//ticket managment
	pthread_mutex_t ticketMutex;
	int tickets;
	//outside line
	pthread_mutex_t lineMutex;
	sem_t guide;
	sem_t visitor;
	int peopleWaiting;

	pthread_mutex_t genMutex;
	sem_t guidesAriving;
	sem_t guidesLeaving;

	sem_t emptyMuseum;

	int *guidesVisitors;
	int visitorsInside;
	int guidesInside;
	
	int totalGuides;
	bool firstLeaver;
};

static struct shared_data shared;


/**
 * Set up the shared variables for your implementation.
 * 
 * `museum_init` will be called before any threads of the simulation
 * are spawned.
 */
void museum_init(int num_guides, int num_visitors)
{
	pthread_mutex_init(&shared.ticketMutex, NULL);
	pthread_mutex_init(&shared.lineMutex, NULL);
	pthread_mutex_init(&shared.genMutex, NULL);
	sem_init(&shared.guide, 0, 0);
	sem_init(&shared.guidesLeaving, 0, 0);
	sem_init(&shared.guidesAriving, 0, GUIDES_ALLOWED_INSIDE);
	sem_init(&shared.visitor, 0, 0);
	sem_init(&shared.emptyMuseum, 0, 0);
	shared.totalGuides = num_guides;
	shared.peopleWaiting = 0;
	shared.firstLeaver = true;
	shared.guidesInside = 0;
	
	shared.guidesVisitors = malloc(16*num_guides);
	int guides = 0;
	while (guides < num_guides) {
		shared.guidesVisitors[guides] = VISITORS_PER_GUIDE;
		guides++;
	}
	
	shared.tickets = MIN(VISITORS_PER_GUIDE * num_guides, num_visitors);
}


/**
 * Tear down the shared variables for your implementation.
 * 
 * `museum_destroy` will be called after all threads of the simulation
 * are done executing.
 */
void museum_destroy()
{
	pthread_mutex_destroy(&shared.ticketMutex);
	pthread_mutex_destroy(&shared.lineMutex);
	sem_destroy(&shared.guide);
	sem_destroy(&shared.guidesLeaving);
	sem_destroy(&shared.guidesAriving);

}


/**
 * Implements the visitor arrival, touring, and leaving sequence.
 */
void visitor(int id)
{
	visitor_arrives(id);
	//if ticket get ticket get in line and block for guide to allow entry
	pthread_mutex_lock(&shared.lineMutex);
	pthread_mutex_lock(&shared.ticketMutex);
	if(shared.tickets > 0) {	
		shared.peopleWaiting++;
		shared.tickets--;
		pthread_mutex_unlock(&shared.lineMutex);
		pthread_mutex_unlock(&shared.ticketMutex);
		sem_post(&shared.visitor);
		sem_wait(&shared.guide);
	} else {
		pthread_mutex_unlock(&shared.lineMutex);
		pthread_mutex_unlock(&shared.ticketMutex);
		visitor_leaves(id);
		return;
	}

	visitor_tours(id);
	pthread_mutex_lock(&shared.lineMutex);
	shared.visitorsInside--;
    visitor_leaves(id);
    pthread_mutex_unlock(&shared.lineMutex);
    //printf("Attempting to wake guide");

    //wake up guides to do their checks
    sem_post(&shared.emptyMuseum);
    sem_post(&shared.visitor);
	return;
}

/**
 * Implements the guide arrival, entering, admitting, and leaving sequence.
 */
void guide(int id)
{
	guide_arrives(id);
	pthread_mutex_lock(&shared.lineMutex);
    pthread_mutex_lock(&shared.genMutex);
    //has extra guides wait outside
	while (shared.guidesInside > 1) {
        pthread_mutex_unlock(&shared.lineMutex);
        pthread_mutex_unlock(&shared.genMutex);
		sem_wait(&shared.guidesAriving);
        pthread_mutex_lock(&shared.lineMutex);
        pthread_mutex_lock(&shared.genMutex);
	}
    //printf("There are %d guides inside I can enter\n", shared.guidesInside);
    
    //makes sure a single guide wont end up waiting for someone to leave with
    //ie. with 3 guides #1 and #2 leave, dropping total to 1, so the odd man out knows to leave alone
	if(shared.totalGuides == 1) {
		//printf("I %d am the last guide of the day\n", id);
		shared.firstLeaver = false;
	}
	guide_enters(id);
	shared.guidesInside++;
	pthread_mutex_unlock(&shared.lineMutex);
    pthread_mutex_unlock(&shared.genMutex);

	//this will bug if two guides have the same id
	//possibly incorrect implementation, may technically count as waiting for memory to change
	while(shared.guidesVisitors[id] > 0) {
		sem_wait(&shared.visitor);
		pthread_mutex_lock(&shared.lineMutex);
        pthread_mutex_lock(&shared.ticketMutex);
		if(shared.peopleWaiting > 0) {
			shared.guidesVisitors[id]--;
			shared.visitorsInside++;
			shared.peopleWaiting--;
			//printf("Guide %d admitted someone, they have %d people left and there are %d people left in line. ", id, shared.guidesVisitors[id], shared.peopleWaiting);
			guide_admits(id);
			sem_post(&shared.guide);
		}

		//if no more tickets and no one waiting we go wait outside for our partner
		if(shared.peopleWaiting == 0 && shared.tickets == 0) {
			//printf("Guide %d found empty ticket box and no waiters\n", id);
            pthread_mutex_unlock(&shared.lineMutex);
			pthread_mutex_unlock(&shared.ticketMutex);
			break;
		}
        pthread_mutex_unlock(&shared.lineMutex);
		pthread_mutex_unlock(&shared.ticketMutex);
	}

    //Coordinates two guides leaving together
	if(!shared.firstLeaver) {
		//printf("Guide %d is second\n", id);
        pthread_mutex_lock(&shared.genMutex);
		shared.firstLeaver = !shared.firstLeaver;
		pthread_mutex_unlock(&shared.genMutex);
        //printf("Guide %d is waiting for last visitor\n", id);
        while (shared.visitorsInside != 0) {
            //printf("%d visitors inside, %d cant leave", shared.visitorsInside, id);
            sem_wait(&shared.emptyMuseum);
        }
		pthread_mutex_lock(&shared.lineMutex);
		shared.guidesInside--;
        shared.guidesInside--;
		shared.totalGuides--;
        shared.totalGuides--;
		pthread_mutex_unlock(&shared.lineMutex);
        sem_post(&shared.guidesLeaving);
        guide_leaves(id);
		sem_post(&shared.guidesAriving);
	} else {
        //printf("Guide %d is first\n", id);
        sem_post(&shared.visitor);
        pthread_mutex_lock(&shared.genMutex);
		shared.firstLeaver = !shared.firstLeaver;
		pthread_mutex_unlock(&shared.genMutex);
		sem_wait(&shared.guidesLeaving);
        guide_leaves(id);
        sem_post(&shared.guidesAriving);
	}
}