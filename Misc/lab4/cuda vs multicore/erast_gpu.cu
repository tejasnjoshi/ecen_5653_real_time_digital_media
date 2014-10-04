#include<cutil_inline.h>
#include<cuda_runtime.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#define CUDA_GRID_INVALIDATE
//#define THREAD_GRID_INVALIDATE
//#define SEQUENTIAL_INVALIDATE
//#define SEQUENTIAL_GRID_INVALIDATE


#define NUM_THREADS 16  // NOTE THAT THIS MUST BE DIVISIBLE BY 2!
#define NUM_LOCKS (4*NUM_THREADS)

pthread_t threads[NUM_THREADS];
unsigned int threadID[NUM_THREADS];

typedef struct _threadArgs
{
    unsigned long long int thread_idx;
    unsigned long long int j;
    unsigned long long int p;
} threadArgsType;

threadArgsType threadarg[NUM_THREADS];
pthread_attr_t fifo_sched_attr;
pthread_attr_t orig_sched_attr;
struct sched_param fifo_param;

#define MAX (1000000ULL)
//#define MAX (500ULL)
//#define MAX (10000000ULL)
//#define MAX (10000000000ULL)
#define CODE_LENGTH ((sizeof(unsigned char))*8ULL)
#define SCHED_POLICY SCHED_RR

//unsigned char isprime[(MAX/(CODE_LENGTH))+1];
unsigned char *isprime;
sem_t updateIsPrime[NUM_LOCKS];

#ifdef CUDA_GRID_INVALIDATE
unsigned char* isprime_cuda;
unsigned int* threadID_cuda;
unsigned long long int* p_cuda;
__shared__ int iii;
#endif

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


void set_scheduler(void)
{
    int max_prio, scope, rc;

    print_scheduler();

    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);

    max_prio=sched_get_priority_min(SCHED_POLICY);
    fifo_param.sched_priority=max_prio;    

    if((rc=sched_setscheduler(getpid(), SCHED_POLICY, &fifo_param)) < 0)
        perror("sched_setscheduler");

    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);

    print_scheduler();
}


int chk_isprime(unsigned long long int i)
{
    unsigned long long int idx;
    unsigned int bitpos;

    idx = i/(CODE_LENGTH);
    bitpos = i % (CODE_LENGTH);

    //printf("i=%llu, idx=%llu, bitpos=%u\n", i, idx, bitpos);

    return(((isprime[idx]) & (1<<bitpos))>0);
}

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
        //printf("thread %llu, j=%llu\n", thargs.thread_idx, thargs.j);
    }

    //printf("invalidate_thread %llu FINAL j=%llu\n", thargs.thread_idx, thargs.j);
    pthread_exit(&thargs.j);
}

#if defined(CUDA_GRID_INVALIDATE)
__global__ void invalidate_cuda(unsigned long long int* p_cuda,unsigned char* isprime_cuda,unsigned int* threadID_cuda){
	int j,idx;
	unsigned long long int byteidx;
	unsigned int bitpos;	

	idx=blockIdx.x*blockDim.x+threadIdx.x;
	for(j=2*(*p_cuda); j < MAX+1; j+=NUM_THREADS*(*p_cuda)){
        if((j+(idx*(*p_cuda))) > MAX+1)break;        		
		byteidx = 	(j+(idx*(*p_cuda)))	/(CODE_LENGTH);
		bitpos = 	(j+(idx*(*p_cuda)))	%(CODE_LENGTH);
		isprime_cuda[byteidx] = isprime_cuda[byteidx] & (~(1<<bitpos));		
		//atomicAnd((isprime_cuda+byteidx),(~(1<<bitpos)));
    }	
}

__global__ void find_next_lowest_prime_cuda(unsigned long long int* p_cuda, unsigned char* isprime_cuda){
	int j;
	unsigned long long int byteidx;
	unsigned int bitpos;	
	for(j=(*p_cuda)+1; j<MAX+1; j++){		
		byteidx = j/(CODE_LENGTH);
		bitpos = j % (CODE_LENGTH);
		if((((isprime_cuda[byteidx]) & (1<<bitpos))>0)){
			(*p_cuda)=j; break;
		}		
	}
}

#endif


int main(void)
{
		struct timeval tv_start, tv_end;
        unsigned long long int i, j, final_thread_j;
        unsigned long long int p=2;		
        unsigned int cnt=0;
        unsigned long long int thread_idx=0;		

        printf("max uint = %u\n", (0xFFFFFFFF));
        printf("max long long = %llu\n", (0xFFFFFFFFFFFFFFFFULL));

        set_scheduler();
        if(!((isprime=(unsigned char*)malloc((size_t)(MAX/(CODE_LENGTH))+1)) > 0))
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
		
#ifdef CUDA_GRID_INVALIDATE
		int retval;
		dim3 grid(1,1);
		dim3 block(NUM_THREADS,1);
		// Allocate Memory in GPU
		if(cudaSuccess!=cudaMalloc(&isprime_cuda,(size_t)(MAX/(CODE_LENGTH)+1))){
			perror("CUDA Malloc: isprime_cuda"); exit(-1);
		}
		if(cudaSuccess!=cudaMalloc(&threadID_cuda,(size_t)(NUM_THREADS))){
			perror("CUDA Malloc: threadID_cuda"); 
			cudaFree(isprime_cuda);
			exit(-1);
		}
		if(cudaSuccess!=cudaMalloc(&p_cuda,(size_t)(sizeof(unsigned long long int)))){
			perror("CUDA Malloc: threadID_cuda");
			cudaFree(isprime_cuda);
			cudaFree(threadID_cuda);
			exit(-1);
		}
		// Copy from Host to GPU
		if(cudaSuccess!=cudaMemcpy((void*)isprime_cuda,(const void*)isprime
			,(size_t)((MAX/(CODE_LENGTH))+1),cudaMemcpyHostToDevice)){
			perror("CUDAMemCpy: isprime");
			cudaFree(isprime_cuda); cudaFree(threadID_cuda); cudaFree(p_cuda);
			exit(-1);
		}
		if(cudaSuccess!=cudaMemcpy((void*)threadID_cuda,(const void*)threadID
			,(size_t)(NUM_THREADS),cudaMemcpyHostToDevice)){
			perror("CUDAMemCpy: threadID");
			cudaFree(isprime_cuda); cudaFree(threadID_cuda); cudaFree(p_cuda);
			exit(-1);
		}

		if(cudaSuccess!=cudaMemcpy((void*)p_cuda,(const void*)&p
			,(size_t)(sizeof(unsigned long long int)),cudaMemcpyHostToDevice)){
			perror("CUDAMemCpy: p_cuda");
			cudaFree(isprime_cuda);
			cudaFree(threadID_cuda);
			cudaFree(p_cuda);
			exit(-1);
		}

		cudaThreadSynchronize();
#endif
		gettimeofday(&tv_start,NULL);		
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
                    pthread_create(&threads[thread_idx], (const pthread_attr_t *)0, invalidate_thread, (void *)&threadarg[thread_idx]);

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
#if defined(SEQUENTIAL_GRID_INVALIDATE)
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

#else
#if defined(CUDA_GRID_INVALIDATE)
					
			invalidate_cuda<<<grid,block>>>(p_cuda,isprime_cuda,threadID_cuda);
			cudaThreadSynchronize();
			find_next_lowest_prime_cuda<<<1,1>>>(p_cuda,isprime_cuda);			
			cudaThreadSynchronize();
//printf("\ncudaErrorInvalidValue=%d\ncudaErrorInvalidDevicePointer=%d\ncudaErrorInvalidMemcpyDirection=%d\nSuccess=%d"
//	, cudaErrorInvalidValue,cudaErrorInvalidDevicePointer,cudaErrorInvalidMemcpyDirection,cudaSuccess);
			if(cudaSuccess!=cudaMemcpy((void*)(&p),(const void*)p_cuda
				,(size_t)(sizeof(unsigned long long int)),cudaMemcpyDeviceToHost)){
				perror("CUDAMemCpy: p");
				cudaFree(isprime_cuda);
				cudaFree(threadID_cuda);
				cudaFree(p_cuda);
				exit(-1);
			}
			cudaThreadSynchronize();
			
#endif
#endif
#endif
#endif

#ifndef CUDA_GRID_INVALIDATE
            // find next lowest prime - sequential process
            for(j=p+1; j<MAX+1; j++)
            {
                if(chk_isprime(j)) { p=j; break; }
            }
#endif
//printf("P=%d\n",p);
//break;
        }	// while
		gettimeofday(&tv_end,NULL);
		printf("Time elapsed= %f ms\n",(float)(1000000*tv_end.tv_sec+tv_end.tv_usec-1000000*tv_start.tv_sec-tv_start.tv_usec)/1000.0);
#ifdef CUDA_GRID_INVALIDATE
		if(cudaSuccess!=cudaMemcpy((void*)isprime,(const void*)isprime_cuda
			,(size_t)((MAX/(CODE_LENGTH))+1),cudaMemcpyDeviceToHost)){
			perror("CUDAMemCpy: isprime- Device to host");
			cudaFree(isprime_cuda); cudaFree(threadID_cuda); cudaFree(p_cuda);
			exit(-1);
		}
		cudaThreadSynchronize();
		cudaFree(isprime_cuda);
		cudaFree(threadID_cuda);
		cudaFree(p_cuda);
		cudaThreadExit();		
#endif
printf("\n\n");
        for(i=0; i<MAX+1; i++)
        {
            if(chk_isprime(i))
            { 
                cnt++; 
//                printf("i=%llu\n", i); 
            }
        }		

//printf("\nisprime:%u\t%0x\n",(unsigned char)(*isprime),(unsigned char)(*(isprime+1)));
		
        printf("\nNumber of primes [0..%llu]=%u\n\n", MAX, cnt);
		

        return (i);
}
