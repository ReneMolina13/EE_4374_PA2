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
long double *factorials;
// Holds pthread attributes
pthread_attr_t attr;
// Holds pointer to threads
pthread_t *tid;


void *catalanThread(void *param)
{
	// Get start and ending number from parameter
	int *parameter = (int *) param;
	int thread_num = *(parameter + THREAD_NUM);
	int start_num = *(parameter + THREAD_START);
	int end_num = *(parameter + THREAD_END);

	
	/* TESTING */
	printf("Thread %i\n", thread_num);
	
	
	
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
	
	long double catalan;
	for (int n = start_num; n <= end_num; n++) {
		
		// temp_result = (2n)! / (n+1)!
		long double temp_result = n+2;
		for (int i = n+3; i <= 2*n; i++) {
			temp_result *= i;
		}
		
		// c(n) = temp_result / n!
		catalan = temp_result / factorials[n];
		fprintf(catalan_file, "n = %d, c(n) = %.0Lf\n", n, catalan);
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
	int num_factorials = num_catalan + 1;
	factorials = malloc(num_factorials * sizeof(long double));
	
	// Calculate all factorial values needed
	*factorials = 1;
	*(factorials + 1) = 1;
	for (int i = 2; i < num_factorials; i++)
		*(factorials + i) = *(factorials + i-1) * i;
	
	// Divide up the work for the threads
	int divided_work = num_catalan / num_threads;
	int remaining_work = num_catalan % num_threads;
	int work_assigned = 0;
	
	// Create threads
	switch (num_threads) {
	case 4:
		int thread4_start_end[3];
		thread4_start_end[THREAD_NUM] = 4;
		thread4_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread4_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread4_start_end);
		
	case 3:
		int thread3_start_end[3];
		thread3_start_end[THREAD_NUM] = 3;
		thread3_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread3_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread3_start_end);
		
	case 2:
		int thread2_start_end[3];
		thread2_start_end[THREAD_NUM] = 2;
		thread2_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work;
		thread2_start_end[THREAD_END] = work_assigned;
		pthread_create(tid++, &attr, catalanThread, (void *) thread2_start_end);
		
	case 1:
		int thread1_start_end[3];
		thread1_start_end[THREAD_NUM] = 1;
		thread1_start_end[THREAD_START] = work_assigned + 1;
		work_assigned += divided_work + remaining_work;
		thread1_start_end[THREAD_END] = work_assigned;
		pthread_create(tid, &attr, catalanThread, (void *) thread1_start_end);
		break;
		
	default:
		printf("Incorrect number of threads\n");
		return -1;
	}
	
	// Wait for all threads to terminate
	for (int i = 0; i < num_threads; i++)
		pthread_join(*(tid - i), NULL);
	tid -= (num_threads - 1);
	
	// Deallocate Memory
	free(factorials);
	free (tid);
	
	// Tell user where to find results and terminate
	printf("Catalan numbers can be found in files catalan1.dat - catalan%i.dat\n", num_threads);
	return 0;
}