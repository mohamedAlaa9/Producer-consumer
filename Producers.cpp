#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string>
#include <random>
#include <math.h>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <csignal>
#include <unistd.h>
#include <time.h>
#include <ctime>
#include <sys/time.h>
using namespace std;
struct tm *currentTime()
{

    time_t currTime = time(0);          //Time right now
    struct tm *localPointer = localtime(&currTime);
    
    return localPointer;
}

int main(int argc, char *argv[])
{
    if (argc !=6)
    {
        printf("Invalid number of arguments \n");
        exit(1);
    }
    char *commodityName = argv[1];           
    double mean = atof(argv[2]);    // Mean of the distribution
    double stdev = (atof(argv[3])); // Standard deviation of the distribution
    double sleepInterval = atof(argv[4]);     // Sleep interval
    int N = atoi(argv[5]);          //buff size
    //Printing time attrs
    struct timeval tv;
    struct timezone tz;
    struct tm *current_t;
    string ms;                      //da string lel milliseconds
    char id[2];
    int cid;
    int k = 29;
    key_t key_shm = ftok(".", k*20);
    key_t key_mutex = ftok(".", k*30);
    key_t key_e = ftok(".", k*40);
    key_t key_n = ftok(".", k*50);
    int mutex_id = semget(key_mutex,1,0666);
    int n = semget(key_n,1,0666); //Can Consume
    int e = semget(key_e,1,0666); //Can produce
    if(mutex_id == -1) {
        perror("semget mutex at con");
        exit(1);
    }
    if(n == -1) {
        perror("semget is full at con");
        exit(1);
    }
    if(e == -1) {
        perror("semget isEmpty at con");
        exit(1);
    }
    // GOlD,CRUDEOIL, NATURALGAS, ALUMINIUM, COPPER, NICKEL,LEAD, ZINC, MENTHAOIL,COTTON,SILVER
    if(!strcasecmp(commodityName,"GOLD")){
        cid=4;
    }else if(!strcasecmp(commodityName,"SILVER")){
        cid=9;
    }else if(!strcasecmp(commodityName,"CRUDEOIL")){
        cid=3;
    }else if(!strcasecmp(commodityName,"NATURALGAS")){
        cid=7;
    }else if(!strcasecmp(commodityName,"ALUMINIUM")){
        cid=11;
    }else if(!strcasecmp(commodityName,"COPPER")){
        cid=19;
    }else if(!strcasecmp(commodityName,"NICKEL")){
        cid=8;
    }else if(!strcasecmp(commodityName,"LEAD")){
        cid=5;
    }else if(!strcasecmp(commodityName,"ZINC")){
        cid=10;
    }else if(!strcasecmp(commodityName,"MENTHAOIL")){
        cid=6;
    }else if(!strcasecmp(commodityName,"COTTON")){
        cid=2;
    }
    sprintf(id, "%d", cid);                         //string containing id
    default_random_engine generator;
    normal_distribution<double> distribution(mean, stdev);
    double price;
    int shmid = shmget(key_shm, (N + 2) * sizeof(double), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("error in shmget");
        exit(0);
    }
    double *ptr = (double *)shmat(shmid, (void *)0, 0);
    if (ptr == (double *)-1)
    {
        perror("error in shmat");
        exit(0);
    }
    struct sembuf semaphore_op[1];
    while (true)
    {
        price = distribution(generator);
        //Print log for getting the current price
        gettimeofday(&tv, &tz);
        current_t = localtime(&tv.tv_sec);
        ms = to_string((int)tv.tv_usec / 1000);                 //usec(Micro)/1000 btgeb el milli
        current_t = currentTime();                              //Lazem azawed 1900 3shan btal3ly 122
        printf("\033[;31m[%d/%d/%d %d:%d:%d.%s] %s: Generating a new value %.02f\n\033[0m", current_t->tm_mday, 1 + current_t->tm_mon, current_t->tm_year + 1900, current_t->tm_hour, current_t->tm_min, current_t->tm_sec, ms.c_str(), commodityName, price);
        semaphore_op[0].sem_op = -1;
        if(semop(e,semaphore_op,1) == -1) {
            perror("semaphore operation failed\n");
            exit(1);
        }
        gettimeofday(&tv, &tz);
        current_t = localtime(&tv.tv_sec);
        ms = to_string((int)tv.tv_usec / 1000);                 
        current_t = currentTime();                              
        printf("\033[;31m[%d/%d/%d %d:%d:%d.%s] %s: Trying to get mutex on shared buffer \n\033[0m", current_t->tm_mday, 1 + current_t->tm_mon, current_t->tm_year + 1900, current_t->tm_hour, current_t->tm_min, current_t->tm_sec, ms.c_str(), commodityName);
        if(semop(mutex_id,semaphore_op,1) == -1) {
            perror("semaphore operation failed\n");
            exit(1);
        }
        //CRITICAL SECTION
        int i = *ptr; // Start address of shared memory
        char temp[128];
        sprintf(temp, "%.2f", price);
        //printf("temp before id %s\n",temp);
        if ((cid==10)  || (cid==11)  ||(cid==19) ){
            strncat(temp, id, 2);
        }
        else
            strncat(temp, id, 1); // Concatenate the id of the commodity to the last decimal place of the price
        price = atof(temp);
        *(ptr + (sizeof(double) * 2) + i) = price; // Writing in the memory
        gettimeofday(&tv, &tz);
        current_t = localtime(&tv.tv_sec);
        ms = to_string((int)tv.tv_usec / 1000);                
        current_t = currentTime();                             
        printf("\033[;31m[%d/%d/%d %d:%d:%d.%s] %s: placing %.02f on shared buffer\n\033[0m", current_t->tm_mday, 1 + current_t->tm_mon, current_t->tm_year + 1900, current_t->tm_hour, current_t->tm_min, current_t->tm_sec, ms.c_str(), commodityName, price);
        *(ptr) = 1.00*((int)(*(ptr)+sizeof(double)) % (int)(N*sizeof(double)));         //Incrementing the writing offset
        semaphore_op[0].sem_op = 1;
        //EXITING CRITICAL SECTION
        if(semop(mutex_id,semaphore_op,1) == -1) {
            perror("semaphore operation failed\n");
            exit(1);
        }
        if(semop(n,semaphore_op,1) == -1) {
            perror("semaphore operation failed\n");
            exit(1);
        }    
        gettimeofday(&tv, &tz);
        current_t = localtime(&tv.tv_sec);
        ms = to_string((int)tv.tv_usec / 1000);                
        current_t = currentTime();                             
        printf("\033[;31m[%d/%d/%d %d:%d:%d.%s] %s: sleeping for %.02f ms\n\033[0m", current_t->tm_mday, 1 + current_t->tm_mon, current_t->tm_year + 1900, current_t->tm_hour, current_t->tm_min, current_t->tm_sec, ms.c_str(), commodityName,sleepInterval);
        usleep(sleepInterval*1000);
        printf("\e[1;1H\e[2J");
    }
    
}