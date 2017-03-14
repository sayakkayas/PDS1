/*Created by Sayak Chakraborti NetID-schakr11,January 2017 
compile- g++ parcount.cpp -w -std=c++0x -pthread*/
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include<ctime>
#include<chrono>

using namespace std;

//static int counter;
/*Global variables for locks and time calculation*/
std::mutex _lock;
std:: mutex g_mutex;  //guard mutex
std::atomic<bool> start;


std::chrono::high_resolution_clock::time_point t1,t2;
std::chrono::duration<double> time_span;
/*....*/


void make_threads_joinsAndReLaunch(int num_threads,int iterations);


//counter increment function without any synchronisation 
void func_no_sync(int & count,int iter)
{

	while (!start.load());  //spin until start becomes true.This is done so that all threads start at the same time (truely run parallely)

	int i;
	for(i=0;i<iter;i++)
	(count)=(count)+1;
}


//counter increment function using local counter variables for different threads
void func_local(int & count,int iter)
{

	while (!start.load());//spin until start becomes true.This is done so that all threads start at the same time (truely run parallely)

	int i=0;
	count=0;

	for(i=0;i<iter;i++)
	count=count+1;

 
}

//counter increment function with specfic lock mechanism on a mutex,so that at a time only one thread gets to increment counter at a time
void func_lock(int & count,int iter)
{

	while (!start.load());//spin until start becomes true.This is done so that all threads start at the same time (truely run parallely)

	int i;
	for(i=0;i<iter;i++)
   	{
     		_lock.lock();   //acquire lock on the mutex _lock
     		(count)=(count)+1;
     		_lock.unlock(); //release lock on the mutex _lock
   	} 
}

//counter increment function with lock_guard on mutex g_mutex
void func_lockguard(int & count,int iter)
{

	while (!start.load());//spin until start becomes true.This is done so that all threads start at the same time (truely run parallely)

	int i;
	for(i=0;i<iter;i++)
	{
     		std::lock_guard<std::mutex> lock(g_mutex);
     		(count)=(count)+1;
     
   	} 
}

//counter increment function for atomic variable using atomic fetch and add
void func_atomic(std::atomic<int> & count,int iter)
{
	while (!start.load());//spin until start becomes true.This is done so that all threads start at the same time (truely run parallely)
	int i;
	for(i=0;i<iter;i++)
	{
     		count.fetch_add(1, std::memory_order_relaxed);
   	}
}


int main(int argc,char** argv)
{
	int num_threads;
	int iterations;

    num_threads=4;
    iterations=10000;

   
	if(argc!=0)
	{
			if(argc==5)
			{   
          
					if(strcmp(argv[1],"-t")==0)
          		    		{ num_threads=atoi(argv[2]);

              				}

              				if(strcmp(argv[3],"-t")==0)
              				{ num_threads=atoi(argv[4]);

              				}


              				if(strcmp(argv[1],"-i")==0)
              				{ iterations=atoi(argv[2]);

              				}

              				if(strcmp(argv[3],"-i")==0)
              				{ iterations=atoi(argv[4]);

              				}

			}//if argc ==5 close

			if(argc==3)
			{
            

            	 		if(strcmp(argv[1],"-t")==0)
              			{ num_threads=atoi(argv[2]);

              			}

            			if(strcmp(argv[1],"-i")==0)
              			{ iterations=atoi(argv[2]);

              			}

			}//if argc==3 close
			
    }//if argc!=0 close
	
    make_threads_joinsAndReLaunch(num_threads,iterations);
  		return 0;
}

//Supervisor function that makes the threads and allotes them different task for each experiment and relaunches them with initial conditions for different experiments
void make_threads_joinsAndReLaunch(int num_threads,int iterations)
{
     //no synchronization
    
	int counter=0,i;double increments_per_micro=0.0;
	std::thread myThreads[num_threads];      //initializing  threads

	start=false;   //initialize start to false so that each thread waits for every other thread to be created before it can start
       
	for ( i=0; i<num_threads; i++)
	{ myThreads[i]=std::thread(func_no_sync, std::ref(counter),iterations);      //run N number of threads
        
	} 

	start=true;   //all threads start now

	t1=std::chrono::high_resolution_clock::now();   //get current time with fine granularity(start time for threads)

	for ( i=0; i<num_threads; i++)
	myThreads[i].join();                          //wait for all the threads created to complete
    
        t2=std::chrono::high_resolution_clock::now();      //get current time when the threads have terminated

        time_span=std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1); //calculate the difference of the two times to get the time required to execute

          

       cout <<"No synchronization "<< counter << " Time " << time_span.count()<<" secs" << endl;
       increments_per_micro=((double)counter)/((double)time_span.count());
       increments_per_micro=increments_per_micro/1000000.0;
       cout<<" Increments per micro "<<increments_per_micro<<endl; 

     /*---------------------------------------------------------*/
     
     //lock and unlock mechanism
	counter=0;
        start=false; //initialize start to false so that each thread waits for every other thread to be created before it can start
       
        for ( i=0; i<num_threads; i++)
        { myThreads[i]=std::thread(func_lock, std::ref(counter),iterations);  //run N number of threads
        
        } 

        start=true;  //all threads start now
    
        t1=std::chrono::high_resolution_clock::now();   //get current time with fine granularity(start time for threads)

        for ( i=0; i<num_threads; i++)
        myThreads[i].join();                               //wait for all the threads created to complete

        t2=std::chrono::high_resolution_clock::now();    //get current time when the threads have terminated

        time_span=std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1); //calculate the difference of the two times to get the time required to execute

        cout <<"Lock "<< counter << " Time " << time_span.count()<<" secs" << endl;
        increments_per_micro=((double)counter)/((double)time_span.count());
       increments_per_micro=increments_per_micro/1000000.0;
       cout<<" Increments per micro "<< increments_per_micro << endl; 


      /*---------------------------------------------------------*/
      //lock guard
      counter=0;
      start=false; //initialize start to false so that each thread waits for every other thread to be created before it can start
     

      for ( i=0; i<num_threads; i++)
      { myThreads[i]=std::thread(func_lockguard, std::ref(counter),iterations);       //run N number of threads
        
      } 
      start=true;  //all threads start now

      t1=std::chrono::high_resolution_clock::now(); //get current time with fine granularity(start time for threads)

      for ( i=0; i<num_threads; i++)
      myThreads[i].join();                          //wait for all the threads created to complete
 
      t2=std::chrono::high_resolution_clock::now(); //get current time when the threads have terminated

       time_span=std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);   //calculate the difference of the two times to get the time required to execute

     cout <<"lockguard "<< counter << " Time " << time_span.count()<<" secs" << endl;
      increments_per_micro=((double)counter)/((double)time_span.count());
       increments_per_micro=increments_per_micro/1000000.0;
       cout<<" Increments per micro "<<increments_per_micro<<endl; 

  
    /*---------------------------------------------------------*/
      //atomic
      std::atomic<int> counter_atm(0);
     start=false; //initialize start to false so that each thread waits for every other thread to be created before it can start

     for ( i=0; i<num_threads; i++)
      { myThreads[i]=std::thread(func_atomic, std::ref(counter_atm),iterations);  //run N number of threads
        
      } 

      start=true;                  //all threads start now
      
      t1=std::chrono::high_resolution_clock::now(); //get current time with fine granularity(start time for threads)
      
      for ( i=0; i<num_threads; i++)
       myThreads[i].join();       //wait for all the threads created to complete

        t2=std::chrono::high_resolution_clock::now(); //get current time when the threads have terminated

       time_span=std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);//calculate the difference of the two times to get the time required to execute

        cout <<"Atomic " << counter_atm << " Time " << time_span.count()<<" secs" << endl;
        increments_per_micro=((double)counter_atm)/((double)time_span.count());
       increments_per_micro=increments_per_micro/1000000.0;
       cout<<" Increments per micro "<<increments_per_micro<<endl; 


     /*---------------------------------------------------------*/
       counter=0;
       start=false; //initialize start to false so that each thread waits for every other thread to be created before it can start


       int c[num_threads];

      for ( i=0; i<num_threads; i++)
      {   c[i]=0;
         myThreads[i]=std::thread(func_local,std::ref(c[i]),iterations ); //run N number of threads
       
        
      } 
      start=true;  //all threads start now

      t1=std::chrono::high_resolution_clock::now(); //get current time with fine granularity(start time for threads)

      for ( i=0; i<num_threads; i++)
      {myThreads[i].join();                       //wait for all the threads created to complete
      counter=counter+c[i];
      }
     
        t2=std::chrono::high_resolution_clock::now(); //get current time when the threads have terminated

       time_span=std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1); //calculate the difference of the two times to get the time required to execute

        cout <<"Local " << counter << " Time " << time_span.count()<<" secs" << endl;
        increments_per_micro=((double)counter)/((double)time_span.count());
       increments_per_micro=increments_per_micro/1000000.0;
       cout<<" Increments per micro "<<increments_per_micro<<endl; 

     
}

