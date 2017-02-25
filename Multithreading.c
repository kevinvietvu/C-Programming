#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#define NUM_OF_STUDENTS 4
#define NUM_OF_SEATS 2
#define NUM_OF_HELPS 2
#define MAX_SLEEP_TIME 3

int ta_sleep; //boolean if ta is sleeping or not
int waiting_students; //number of waiting students on chairs
int students_left = NUM_OF_STUDENTS; //number of students left in the building
pthread_t ta; //ta thread variable
pthread_t students[NUM_OF_STUDENTS]; //student thread variable
//mutex and semaphore declaration
pthread_mutex_t mutex_lock;
pthread_mutex_t mutex_lock2;
pthread_cond_t cond;
sem_t students_sem;
sem_t ta_sem;

/**
 * Function that student threads run
 */
void *studentFunction(void *param) {
	int studentNumber = param; //student id
	int seed = studentNumber * MAX_SLEEP_TIME * NUM_OF_STUDENTS; // seed for random number generator
	int sleep_time; //random time for sleep
	int numOfHelp = 0; //number of times student has been helped

	while (numOfHelp < NUM_OF_HELPS) {
		sleep_time = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
		printf("	Student %d programming for %d seconds\n", studentNumber,
				sleep_time);
		sleep(sleep_time);

		if (waiting_students == 2) {
			printf("			Student %d will try later\n", studentNumber);
			continue;
		}
		sem_wait(&students_sem);
		pthread_mutex_lock(&mutex_lock);
		if (ta_sleep == 1) {
			pthread_cond_signal(&cond);
			ta_sleep = 0;
		}
		waiting_students++;
		printf("		Student %d takes a seat, # of waiting students = %d\n",
				studentNumber, waiting_students);
		pthread_mutex_unlock(&mutex_lock);
		sem_wait(&ta_sem);
		numOfHelp++;
		printf("Student %d receiving help\n", studentNumber);
	}
	students_left--;
	if (students_left == 0) { pthread_cond_signal(&cond); }
	printf("	student %d exited\n", studentNumber);
	pthread_cancel(students[studentNumber]);

}
/**
 * Function that the ta thread runs
 */
void *taFunction(void *param) {
	while (students_left > 0) {
		if (waiting_students < 1) {
			ta_sleep = 1;
			pthread_cond_wait(&cond, &mutex_lock2);
			if (students_left == 0) { break; }
		}
		sem_post(&students_sem);
		pthread_mutex_lock(&mutex_lock);
		waiting_students--;
		printf("Helping a student for 3 seconds, # of waiting students = %d\n",
				waiting_students);
		pthread_mutex_unlock(&mutex_lock);
		sem_post(&ta_sem);
		sleep(MAX_SLEEP_TIME);
	}
	printf("Ta exited\n");
	pthread_cancel(ta);
}

/**
 * Create the student threads.
 */
void createStudentThreads() {
	int i;
	for (i = 0; i < NUM_OF_STUDENTS; i++) {
		pthread_create(&students[i], 0, studentFunction, (void *) i);
	}
}

/**
 * Create the TA thread.
 */
void createTaThread() {
	pthread_create(&ta, 0, taFunction, 0);
}

int main(void) {
	int i; //counter for students

	//initializing semaphores and mutexes
	sem_init(&students_sem, 0, 2);
	sem_init(&ta_sem, 0, 0);
	pthread_mutex_init(&mutex_lock, 0);
	pthread_mutex_init(&mutex_lock2, 0);
	pthread_cond_init(&cond, NULL);

	printf("CS149 SleepingTa from Kevin Vu\n");

	//creating student threads
	createStudentThreads();

	//creating ta thread
	createTaThread();

	//join threads
	for (i = 0; i < NUM_OF_STUDENTS; i++)
		pthread_join(students[i], NULL);

	pthread_join(ta, NULL);

	//destroy semaphores and mutexes
	sem_destroy(&students_sem);
	sem_destroy(&ta_sem);
	pthread_mutex_destroy(&mutex_lock);
	pthread_mutex_destroy(&mutex_lock2);
	pthread_cond_destroy(&cond);

	return 0;
}
