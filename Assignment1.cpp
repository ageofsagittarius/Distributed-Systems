// Ritik Mehra
// IIT2019038

#include<iostream>
#include<pthread.h>
#include<cstdlib>
using namespace std;

// Structure to store the initial and final index for each thread instance
struct str{
	int init;
	int fin;
};

int sum = 0;
int num_of_thread = 16;
double *arr;

// Mutex that allows only one thread the access to update global sum value
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Method that the thread executes in its body.
void* sum_find(void *st)
{
	int curr_sum = 0;

	//conversion of void * to the desired limit * type
	struct str *lim = (str *)st;
	
	//sum calculation for the range defined in the structure, i.e. [init, fin)
	for (int i = lim->init; i < lim->fin; i++)
		curr_sum += arr[i];
	
	//Acquiring the lock to update the global sum value
	pthread_mutex_lock(&mutex);      
	
	//Update the global sum only after acquiring the lock
	sum += curr_sum;
	
	//Relinquish the acquired lock
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

int main()
{
	// vars to store the starting and ending time in clock cycles
	clock_t st, end;
	
	int n = 1024;
	
	arr = new double[n];
	
	pthread_t thread[num_of_thread];
	
	for (int i = 0; i < n; i++)
		arr[i] = i;
	
	// starting time for thread executions
	st = clock();
	
	for(int i = 0; i < num_of_thread; i++){
	    // initial and final indexes for the current value of i
		int init = i*(n/num_of_thread);
		int fin = (i+1)*(n/num_of_thread);
		
		//allocating the space for the structure from the heap
		struct str *lim = (str *)malloc(sizeof(str));
		lim->init = init;
		lim->fin = fin;
		
		//creation of thread[i] with lim as the argument to sum_find
		pthread_create(&thread[i], NULL, sum_find, lim);
	}
	
	// Joining the thread on completion to prevent the main thread exiting before the user threads 
	for (int i = 0; i< num_of_thread; i++){
		pthread_join(thread[i], NULL);
	}
		
	//ending time for thread executions
	end = clock();
	
	double formula_sum = (double)n*(n-1)/2;
	
	cout << "N:" << n << "\n";
	cout << "Thread Sum:" << sum << "\n";
	cout << "Formula Sum:" << formula_sum << "\n";
	
	// Total execution time in secs
	cout << "Time:" << (float)(end - st) / CLOCKS_PER_SEC << "s" << endl;
	
	delete[] arr;
	
	return 0;
}


/*  System Configuration:
        Ryzen 8 cores, 16 threads
        8 GB Physical Memory
        Ubuntu
        g++ version 9.3.0
*/

/*  Execution Time in sec (calculated using clock() function and is averaged over 5 readings)
        n = 1 Thread, t = 0.000178s
        n = 2 Threads, t = 0.000424s
        n = 4 Threads, t = 0.000680s
        n = 8 Threads, t = 0.001112s
        n = 16 Threads, t = 0.001391s
*/

/*
    The trend is almost linear as it can be seen clearly that when we increase the number of parallel threads, the
    execution time also increases linearly.
    On increasing the number of threads, the number of simultaneous computations also increasesand hence the overall 
    time should decrease linearly, but it turns out to be the opposite.

    This can be due to the large overhead of creating and executing the many threads. The overheard is 
    significant since the computation time is very small compared to the overall overhead. On increasing the number 
    of threads, overhead increases.
*/
