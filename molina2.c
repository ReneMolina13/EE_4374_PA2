/*
 * Programming Assignment 2 - Intro to Threads
 * Rene Molina
 * EE 4374 - Operating Systems
 */


#include <stdio.h>		// Standard I/O library
#include <stdlib.h>		// Standard library: atoi(), malloc
#include <pthread.h>	// pthreads library
#include <unistd.h>		// UNIX system call library
#include <signal.h>		// Signal handler library


// Indices used to assign work to threads
#define THREAD_NUM		0
#define THREAD_START	1
#define THREAD_END		2


// Holds array of all factorials needed
unsigned long long int *factorials;
// Holds pthread attributes
pthread_attr_t attr;
// Holds pointer to threads
pthread_t *tid;


void *catalanThread(void *param)
{
	// Get start and ending number from parameter
	int *thread_num_start_end = (int *) param;
	int thread_num = thread_num_start_end[THREAD_NUM];
	int start_num = thread_num_start_end[THREAD_START];
	int end_num = thread_num_start_end[THREAD_END];
	
	// Open file for this thread
	FILE *catalan_file;
	switch (thread_num) {
	case 1:
		catalan_file = fopen("catalan1.dat", "w");
		break;
	case 2:
		catalan_file = fopen("catalan2.dat", "w");
		break;
	case 3:
		catalan_file = fopen("catalan3.dat", "w");
		break;
	case 4:
		catalan_file = fopen("catalan4.dat", "w");
		break;
	default:
		printf("Invalid thread number\n");
		return (void *) -1;
	}
	
	// Calculate and print Catalan numbers for this thread
	// c(n) = (2n)! / ((n+1)! * n!)
	
	unsigned long long int catalan, numerator;
	long double denominator;
	
	for (int n = start_num; n <= end_num; n++) {
		numerator = *(factorials + (2*n));
		denominator = (long double) ((*(factorials + (n+1))) * (*(factorials + n)));
		catalan = (unsigned long long int) (numerator / denominator);
		fprintf(catalan_file, "n = %d, c(n) = %lld\n", n, catalan);
	}
	
	// Close file and exit thread
	fclose(catalan_file);
	pthread_exit(0);
}


int main(int argc, char **argv)
{
	// Check for correct number of arguments
	if (argc != 3) {
		printf("Not enough armuments entered\n");
		printf("1st argument should be number of Catalan numbers to generate\n");
		printf("2nd argument should be number of threads to use (4 or less)\n");
		return -1;
	}
	
	// Extract number of Catalan numbers and number of threads
	int num_catalan = atoi(*(argv + 1));
	int num_threads = atoi(*(argv + 2));
	
	// Perform Error Checking
	
	// Too many or too few catalan numbers
	if (num_catalan < 1) {
		num_catalan = 1;
		printf("Too Few Catalan Numbers: adjusted to 1\n");
	}
	else if (num_catalan > 10) {
		num_catalan = 10;
		printf("Too Many Catalan Numbers (will cause overflow): adjusted to 10\n");
	}
	
	// Too many or too few threads
	if (num_threads < 0) {
		num_threads = 1;
		printf("Too Few Threads: adjusted to 1\n");
	}
	else if (num_threads > 4) {
		num_threads = 4;
		printf("Too Many Threads: adjusted to 4\n");
	}
	
	// Setup variables needed for threads
	pthread_attr_init(&attr);
	tid = malloc(num_threads * sizeof(pthread_t));
	
	// Create an array of all factorials needed
	int num_factorials = (2 * num_catalan) + 1;
	factorials = malloc(num_factorials * sizeof(unsigned long long int));
	
	// Calculate all factorial values needed
	*factorials = 1;
	*(factorials + 1) = 1;
	for (int i = 2; i < num_factorials; i++) {
		*(factorials + i) = *(factorials + i-1) * i;
		printf("index = %d, value = %lld\n", i, *(factorials + i));
	}
	
	// Divide up the work for the threads
	int divided_work = num_catalan / num_threads;
	int remaining_work = num_catalan % num_threads;
	int thread_num_start_end[3] = {0, 0, 0};
	int work_assigned = 0;
	
	// Create threads
	switch (num_threads) {
	case 4:
		thread_num_start_end[THREAD_NUM] = 4;
		thread_num_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread_num_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread_num_start_end);
		
	case 3:
		thread_num_start_end[THREAD_NUM] = 3;
		thread_num_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread_num_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread_num_start_end);
		
	case 2:
		thread_num_start_end[THREAD_NUM] = 2;
		thread_num_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread_num_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread_num_start_end);
	
	case 1:
		thread_num_start_end[THREAD_NUM] = 1;
		thread_num_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work + remaining_work;
		thread_num_start_end[THREAD_END] = work_assigned;
		pthread_create(tid, &attr, catalanThread, (void *) thread_num_start_end);
		break;
		
	default:
		printf("Incorrect number of threads\n");
		return -1;
	}
	
	// Wait for all threads to terminate
	for (int i = 0; i < num_threads; i++)
		pthread_join(*(tid - i), NULL);
	
	// Deallocate Memory
	free(factorials);
	free (tid);
	
	// Tell user where to find results and terminate
	printf("Catalan numbers can be found in files catalan1.dat - catalan%i.dat\n", num_threads);
	return 0;
}