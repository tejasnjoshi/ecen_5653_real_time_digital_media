#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <semaphore.h>
#include "perflib.c"
#include "perflib.h"

#define NUM_THREADS 8  // NOTE THAT THIS MUST BE DIVISIBLE BY 2!
#define NUM_LOCKS (4*NUM_THREADS)
double DT=0.0, startTOD=0.0, stopTOD=0.0;

pthread_t threads[NUM_THREADS]; //creates a arry of threads
unsigned int threadID[NUM_THREADS]; //creates array of NUM_THREADS to store thread IDs

//structure to pass arguments while creating thread
typedef struct _threadArgs
{
    unsigned long long int thread_idx;   //thread id being sent
    unsigned long long int j;       
    unsigned long long int p;
} threadArgsType;  //initiating stuct as threadArgsType

threadArgsType threadarg[NUM_THREADS];   //array of stucture declared 

//declaring thread attribute variables
pthread_attr_t fifo_sched_attr;  
pthread_attr_t orig_sched_attr;
//declaring scheduler paramerter stucture
struct sched_param fifo_param;

#define MAX (1000000ULL)
//#define MAX (10000000000ULL)
#define CODE_LENGTH ((sizeof(unsigned char))*8ULL)
#define SCHED_POLICY SCHED_RR 


//#define THREAD_GRID_INVALIDATE
#define SEQUENTIAL_INVALIDATE
//#define SEQUENTIAL_GRID_INVALIDATE

//unsigned char isprime[(MAX/(CODE_LENGTH))+1];
unsigned char *isprime;
sem_t updateIsPrime[NUM_LOCKS];


// The routine to print scheduler policy
void print_scheduler(void)
{
    int schedType = sched_getscheduler(getpid());

    switch(schedType)
    {
        case SCHED_FIFO:
            printf("Pthread policy is SCHED_FIFO\n");
            break;
        case SCHED_OTHER:
            printf("Pthread policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf("Pthread policy is SCHED_RR\n");
            break;
        default:
            printf("Pthread policy is UNKNOWN\n");
    }
}


//routine to set the scheduler policy as #defined by SCHED_POLICY variable
void set_scheduler(void)
{
    int max_prio, scope, rc;

    print_scheduler();

//sets up the thread attribute initiation variable
    pthread_attr_init(&fifo_sched_attr);
//sets the inheritance policy
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
//sets the scheduler policy to Round Robin in attributes
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);
//gets the max priority of the RR 
    max_prio=sched_get_priority_min(SCHED_POLICY);
//stores the max prio value in sctruct fifo_param.sched_priority
    fifo_param.sched_priority=max_prio;    
//sets the scheduler policy 
    if((rc=sched_setscheduler(getpid(), SCHED_POLICY, &fifo_param)) < 0)
        perror("sched_setscheduler");

    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);

    print_scheduler();
}

//check if the number is prime
int chk_isprime(unsigned long long int i)
{
    unsigned long long int idx;
    unsigned int bitpos;

    idx = i/(CODE_LENGTH);
    bitpos = i % (CODE_LENGTH);

    //printf("i=%llu, idx=%llu, bitpos=%u\n", i, idx, bitpos);

    return(((isprime[idx]) & (1<<bitpos))>0);
}

//sets a prime number
int set_isprime(unsigned long long int i, unsigned char val)
{
    unsigned long long int idx;
    unsigned int bitpos;

    idx = i/(CODE_LENGTH);
    bitpos = i % (CODE_LENGTH);

    //printf("i=%llu, idx=%llu, bitpos=%u\n", i, idx, bitpos);

    // MUTEX PROTECT THIS TEST AND SET
    if(val > 0)
    {
        sem_wait(&updateIsPrime[idx % NUM_LOCKS]);
        isprime[idx] = isprime[idx] | (1<<bitpos);
        sem_post(&updateIsPrime[idx % NUM_LOCKS]);
    }
    else
    {
        sem_wait(&updateIsPrime[idx % NUM_LOCKS]);
        isprime[idx] = isprime[idx] & (~(1<<bitpos));
        sem_post(&updateIsPrime[idx % NUM_LOCKS]);
    }
    // MUTEX PROTECT THIS TEST AND SET
}


void print_isprime(void)
{
    long long int idx=0;

    printf("idx=%lld\n", (MAX/(CODE_LENGTH)));

    for(idx=(MAX/(CODE_LENGTH)); idx >= 0; idx--)
    {
        //calls the function isprime and send idx value
        printf("idx=%lld, %02X\n", idx, isprime[idx]);
    }
    printf("\n");

}


void *invalidate_thread(void *threadptr)
{
    threadArgsType thargs=*((threadArgsType *)threadptr);

    for(thargs.j=2*(thargs.p); (thargs.j < MAX+1); thargs.j+=NUM_THREADS*(thargs.p))
    {
        if((thargs.j+(thargs.thread_idx*thargs.p)) > MAX+1) break;
        set_isprime(thargs.j+((thargs.thread_idx)*(thargs.p)),0);
       // printf("thread %llu, j=%llu\n", thargs.thread_idx, thargs.j);
    }

   // printf("invalidate_thread %llu FINAL j=%llu\n", thargs.thread_idx, thargs.j);
    pthread_exit(&thargs.j);
}


int main(void)
{
        unsigned long long int i, j, final_thread_j;
        unsigned long long int p=2;
        unsigned int cnt=0;
        unsigned long long int thread_idx=0;

        printf("max uint = %u\n", (0xFFFFFFFF));
        printf("max long long = %llu\n", (0xFFFFFFFFFFFFFFFFULL));
        

        startTOD=readTOD(); 
        set_scheduler();
        if(!((isprime=malloc((size_t)(MAX/(CODE_LENGTH))+1)) > 0))
        {
            perror("malloc");
            exit(-1);
        }

        int idx=0, ridx=0;

        // Divide the packed isprime binary array into a number of locking
        // regions.
        //
        for(idx=0;idx<NUM_LOCKS;idx++)
        {
            if (sem_init (&updateIsPrime[idx], 0, 0))
	    {
	        perror ("sem_init");
	        printf ("Failed to initialize updateIsPrime semaphore %d\n", idx);
	        exit (-1);
	    }
            sem_post(&updateIsPrime[idx]);
        }

        // Now scramble thread indices into thread IDs that are non-consecutive
        // to reduce lock conention
        for(idx=0, ridx=(NUM_THREADS-1); idx<NUM_THREADS; idx+=2, ridx-=2)
        {
            if(((idx % 2) == 0) || (idx == 0))
            {
                threadID[idx]=ridx;
                threadID[idx+1]=idx;
                //printf("** idx=%d, ridx=%d, threadID[%d]=%d, threadID[%d]=%d\n", idx, ridx, idx, threadID[idx], idx+1, threadID[idx+1]);
            }
            else
            {
                threadID[idx]=ridx;
                threadID[idx+1]=idx;
                //printf("** idx=%d, ridx=%d, threadID[%d]=%d, threadID[%d]=%d\n", idx, ridx, idx, threadID[idx], idx+1, threadID[idx+1]);
            }
        }
 
        for(idx=0, ridx=(NUM_THREADS-1); idx<NUM_THREADS; idx++, ridx--)
            printf("idx=%d, ridx=%d, threadID=%d\n", idx, ridx, threadID[idx]);

        // Not prime by definition
        // 0 & 1 not prime, 2 is prime, 3 is prime, assume others prime to start
        isprime[0]=0xFC; 
        for(i=2; i<MAX; i++) { set_isprime(i, 1); }
  
        //for(i=0; i<MAX; i++) { printf("isprime=%d\n", chk_isprime(i)); }
        //print_isprime();

        while( (p*p) <=  MAX)
        {
            //printf("p=%llu\n", p);

            // invalidate all multiples of lowest prime so far
            // 
            // simple to compose into a grid of invalidations
            //

#if defined(SEQUENTIAL_INVALIDATE)

            for(j=2*p; j<MAX+1; j+=p)
            {
                //printf("j=%llu\n", j);
                set_isprime(j,0);
            }

#else

#if defined(THREAD_GRID_INVALIDATE)
#warning THREAD_GRID_INVALIDATE

            // Create threads so as to keep them in different regions of
            // the grid to avoid locking as much as possible.
            //
            for(thread_idx=0; thread_idx < NUM_THREADS; thread_idx++)
            {
                    // This is where we permute the thread mapping
                    // to grid mapping to avoid lock contention
                    threadarg[thread_idx].thread_idx=threadID[thread_idx];
                    threadarg[thread_idx].p=p;
                    threadarg[thread_idx].j=2*p;
                    pthread_create(&threads[thread_idx], (void *)0, invalidate_thread, (void *)&threadarg[thread_idx]);

            }
    
            for(thread_idx=0; thread_idx < NUM_THREADS; thread_idx++)
            {
                    if(pthread_join(threads[thread_idx], (void **)&final_thread_j) < 0)
                    {
                        perror("pthread_join");
                        exit(-1);
                    }
                    else
                    {
                        //printf("join thread %llu FINAL j=%llu\n", thread_idx, final_thread_j);
                    }
            }

#else
#warning SEQUENTIAL_GRID_INVALIDATE

            for(thread_idx=0; thread_idx < NUM_THREADS; thread_idx++)
            {
                for(j=2*p; j<MAX+1; j+=NUM_THREADS*p)
                {
                    if((j+(thread_idx*p)) > MAX+1) break;
                    set_isprime(j+(thread_idx*p),0);
                    //printf("thread %llu, j=%llu\n", thread_idx, j);
                }

                //printf("sequential FINAL j=%llu\n", j);
            }

#endif
#endif

            // find next lowest prime - sequential process
            for(j=p+1; j<MAX+1; j++)
            {
                if(chk_isprime(j)) { p=j; break; }
            }

        }


        for(i=0; i<MAX+1; i++)
        {
            if(chk_isprime(i))
            { 
                cnt++; 
                //printf("i=%llu\n", i); 
            }
        }
	stopTOD=readTOD();
        DT = elapsedTOD(stopTOD, startTOD);
        printf("Elapesd Time=%lf secs\n", DT);
        printf("\nNumber of primes [0..%llu]=%u\n\n", MAX, cnt);

        return (i);
}
