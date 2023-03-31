#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <random>
#include <unistd.h>
using namespace std;
struct commodity
{
    int id;
    double price;
};
struct commodities{
    commodity commoditiesQueue[1];
    int in,out;
    int buffSize;
};


int main(int argc, char *argv[])
{   //If argument count = 1  so only the program name is sent with no arguments the program exits
    if(argc==1){
        printf("ERROR no arguments sent to producer\n");
        exit(1);
    }
    int cId=-1;
    char * commodityName=argv[1];
    double mean=atof(argv[2]);
    double stdDev=atof(argv[3]);
    int sleepTime=atoi(argv[4]);
    int bufferSize=atoi(argv[5]);
    printf("%s\n%f\n%f\n%d\n",commodityName,mean,stdDev,sleepTime);
    default_random_engine generator;
    normal_distribution <double> distribution(mean,stdDev);
    double price;
    if(!strcasecmp(commodityName,"GOLD")){
        cId=4;
    }else if(!strcasecmp(commodityName,"SILVER")){
        cId=9;
    }else if(!strcasecmp(commodityName,"CRUDEOIL")){
        cId=3;
    }else if(!strcasecmp(commodityName,"NATURALGAS")){
        cId=7;
    }else if(!strcasecmp(commodityName,"ALUMINIUM")){
        cId=0;
    }else if(!strcasecmp(commodityName,"COPPER")){
        cId=1;
    }else if(!strcasecmp(commodityName,"NICKEL")){
        cId=8;
    }else if(!strcasecmp(commodityName,"LEAD")){
        cId=5;
    }else if(!strcasecmp(commodityName,"ZINC")){
        cId=10;
    }else if(!strcasecmp(commodityName,"MENTHAOIL")){
        cId=6;
    }else if(!strcasecmp(commodityName,"COTTON")){
        cId=2;
    }
    /*if(cId=-1){
        printf("UNAVAILABLE COMMODITY\n");
        exit(1);
    }*/
    printf("%d\n",cId);
    // ftok to generate unique key
    key_t key = ftok(".", 4007);

    // shmget returns an identifier in shmid
    int shmid = shmget(key, bufferSize*(sizeof(commodities)), 0666 | IPC_CREAT);
    if(shmid ==-1){
        exit(1);
    }
    // shmat to attach to shared memory
    commodities *startCommodities = (commodities *)shmat(shmid, (void *)0, 0);
    //START SEMAPHORES
    while (true)
    {
        price=distribution(generator);
        //price=10*startCommodities->in;
        startCommodities->commoditiesQueue[startCommodities->in].id=cId;
        startCommodities->commoditiesQueue[startCommodities->in].price=price;
        startCommodities->in=(startCommodities->in+1);
        if(startCommodities->in==50){
            startCommodities->in=0;
        }
        sleep(sleepTime);
    }
    
    
    //cout << "Write Data : ";
    //cin >> str;
    //
    //printf("Data written in memory: %s\n", str);
    //printf("%ld",sizeof(commodity));
    // detach from shared memory
    //shmdt(writeAddress);

    return 0;
}
