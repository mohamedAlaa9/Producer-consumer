#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <vector>
#include <cstring>
using namespace std;
struct commodity
{
    int id;
    double price;
};
struct commodities
{
    commodity commoditiesQueue[1];
    int in, out;
    int buffSize;
    commodities(): in(0),out(0){};
};
int main(int argc,char * argv[])
{
    if(argc==1){
        printf("ERROR max buffer not given as an input\n");
        exit(1);
    }
    char* commNames[11]
        = { "ALUMINIUM", "COPPER", "COTTON", "CRUDEOIL","GOLD","LEAD","MENTHAOIL","NATURALGAS","NICKEL","SILVER","ZINC" };
    int bufferSize=atoi(argv[1]);
    commodities *readCommodities;
    // ftok to generate unique key
    double prices[11][5];
    for(int i=0;i<11;i++){
        for(int j=0;j<5;j++){
            prices[i][j]=0;
        }
    }
    double goldPrices[5]={0,0,0,0,0};
    double silverPrices[5]={0,0,0,0,0};
    double crudeOilPrices[5]={0,0,0,0,0};
    double naturalGasPrices[5]={0,0,0,0,0};
    double aluminiumPrices[5]={0,0,0,0,0};
    double copperPrices[5]={0,0,0,0,0};
    double nickelPrices[5]={0,0,0,0,0};
    double leadPrices[5]={0,0,0,0,0};
    double zincPrices[5]={0,0,0,0,0};
    double menthaOilPrices[5]={0,0,0,0,0};
    double cottonPrices[5]={0,0,0,0,0};
    int commodityPriceIndicies[11]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int writePriceIndex;
    int currentId;
    double avg=0;
    double currentPrice;


    key_t key = ftok(".", 4007);

    // shmget returns an identifier in shmid
    int shmid = shmget(key, bufferSize*(sizeof(commodities)), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        exit(1);
    }
    // shmat to attach to shared memory
    commodities *startCommodities = (commodities *)shmat(shmid, (void *)0, 0);
    startCommodities->buffSize=bufferSize;
    while (true)
    {
        if (startCommodities->out >= startCommodities->in)
        {
            //printf("%d,%d",startCommodities->in,startCommodities->out);
            continue;
        }
        
        currentId=startCommodities->commoditiesQueue[startCommodities->out].id;
        currentPrice=startCommodities->commoditiesQueue[startCommodities->out].price;
        if(currentId==0){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //aluminiumPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
            for(int j=0;j<5;j++){
                avg+=aluminiumPrices[j];     
            }
            
        }else if(currentId==1){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //copperPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==2){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //cottonPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==3){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //crudeOilPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==4){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //goldPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==5){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //leadPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==6){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //menthaOilPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==7){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //naturalGasPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==8){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //nickelPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==9){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //silverPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }else if(currentId==10){
            writePriceIndex=commodityPriceIndicies[currentId];
            writePriceIndex=(writePriceIndex+1)%5;
            //zincPrices[writePriceIndex]=currentPrice;
            prices[currentId][writePriceIndex]=currentPrice;
            commodityPriceIndicies[currentId]=writePriceIndex;
        }
        printf("+-------------------------------------+\n");
        printf("| Currency \t| Price | AvgPrice |\n");
        for (int i = 0; i < 11; i++)
        {
            avg=0;
            printf("%ld ",strlen(commNames[i]));
            printf("| %s \t",commNames[i]);
            printf("| %.02f \t",prices[i][commodityPriceIndicies[i]]);
            for(int j=0;j<5;j++){
                avg+=prices[i][j];
            }
            printf("| %f   \n",avg/5);
        }

        printf("%d,%d",startCommodities->in,startCommodities->out);
        printf("cid= %d\n", startCommodities->commoditiesQueue[startCommodities->out].id);
        printf("Current price %f\n", startCommodities->commoditiesQueue[startCommodities->out].price);
        startCommodities->out = (startCommodities->out + 1) ;
        if(startCommodities->out==50){
            startCommodities->out=0;
        }
    }

    /*for(int i=0;i<11;i++){
        //readCommodity=startCommodity+(i*sizeof(commodity));
        //printf("readcommodity = %d",readCommodity);
        if(readCommodity->prices[0]<=0  || readCommodity->id<0 || readCommodity->id>11 )
        {
            readCommodity=readCommodity+sizeof(commodity);
            continue;
        }
        printf("cid= %d\n",readCommodity->id);
        for(int j=0;j<5;j++){
            printf("%f\t",readCommodity->prices[j]);
        }
        printf("\n");
        readCommodity=readCommodity+sizeof(commodity);
    }*/
    // detach from shared memory
    // shmdt(str);

    // destroy the shared memory
    // shmctl(shmid,IPC_RMID,NULL);

    return 0;
}
