#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <signal.h>
#include <fcntl.h>
#include <csignal>
#include <random>
#include <cstring>
#include <sys/sem.h>
using namespace std;
int shmid;
int mutex_id;
int n;
int e; 
void destroyShared(int x){
    int destroyed = shmctl(shmid,IPC_RMID,0);
    if(destroyed == -1){
        printf("error in destroying the shared memory\n");
    }
}
/*void destroySema(int x){
    int destroyed = semctl(mutex_id, 0, IPC_RMID);
    if(destroyed == -1){
        printf("error in destroying mutex semaphore\n");
    }
    destroyed = semctl(n, 0, IPC_RMID);
    if(destroyed == -1){
        printf("error in destroying n semaphore\n");
    }
    destroyed = semctl(e, 0, IPC_RMID);
    if(destroyed == -1){
        printf("error in destroying e semaphore\n");
    }
}*/
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("ERROR max buffer not given as an input\n");
        exit(1);
    }
    int N = atoi(argv[1]);
    union semun
    {
        int val;
        struct semid_ds *buf;
        ushort array[1];
    } sem_attr;
    int k = 29;
    key_t key_shm = ftok(".", k * 20);
    key_t key_mutex = ftok(".", k * 30);
    key_t key_e = ftok(".", k * 40);
    key_t key_n = ftok(".", k * 50);
    int mutex_id = semget(key_mutex, 1, IPC_CREAT | 0666);
    int n = semget(key_n, 1, IPC_CREAT | 0666); // Can Consume
    int e = semget(key_e, 1, IPC_CREAT | 0666); // Can produce
    if (mutex_id == -1)
    {
        perror("semget mutex at con");
        exit(1);
    }
    else
    {
        sem_attr.val = 1;
        if (semctl(mutex_id, 0, SETVAL, sem_attr) == -1)
        {
            perror("semctl error");
            exit(1);
        }
    }
    if (n == -1)
    {
        perror("semget is full at con");
        exit(1);
    }
    else
    {
        sem_attr.val = 0;
        if (semctl(n, 0, SETVAL, sem_attr) == -1)
        {
            perror("semctl error");
            exit(1);
        }
    }
    if (e == -1)
    {
        perror("semget isEmpty at con");
        exit(1);
    }
    else
    {
        sem_attr.val = N;
        if (semctl(e, 0, SETVAL, sem_attr) == -1)
        {
            perror("semctl error");
            exit(1);
        }
    }

    struct sembuf semaphore_op[1];
    // key_t key = ftok(".",2016);
    shmid = shmget(key_shm, (N + 2) * sizeof(double), 0666 | IPC_CREAT);
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
    double *tempptr;
    char temp[128];
    const char *commNames[11] = {"ALUMINIUM", "COPPER", "COTTON", "CRUDEOIL", "GOLD", "LEAD", "MENTHAOIL", "NATURALGAS", "NICKEL", "SILVER", "ZINC"};
    double prices[11][5];
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            prices[i][j] = 0;
        }
    }
    int currentId;
    double avg = 0;
    double currentPrice;
    int avgFlags[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double replacedPrice;
    int commodityPriceIndicies[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int writePriceIndex;
    int i, j;
    int index;     // Bta3 el loop
    int prevIndex; // Bta3 el loop bardo
    semaphore_op[0].sem_num = 0;
    semaphore_op[0].sem_op = 0;
    semaphore_op[0].sem_flg = 0;
    // Start of Semaphore
    signal(SIGINT, destroyShared);
    //signal(SIGINT, destroySema);
    while (true)
    {
        semaphore_op[0].sem_op = -1;
        if (semop(n, semaphore_op, 1) == -1)
        {
            perror("semaphore operation failed\n");
            exit(1);
        }
        if (semop(mutex_id, semaphore_op, 1) == -1)
        {
            perror("semaphore operation failed\n");
            exit(1);
        }
        //ENtering cs

        
        i = *ptr;
        j = *(ptr + sizeof(double));
        double read = *(ptr + sizeof(double));
        double price = *(ptr + 2 * sizeof(double) + (int)read);
        char currentPriceString[20];
        double currentPrice;
        int currentId;
        sprintf(temp, "%.4f", price);
        sprintf(currentPriceString, "%.2f", price);
        //printf("temp=%s\n",temp);
        currentPrice = atof(currentPriceString);
        char c_id[2];
        c_id[0] = temp[strlen(temp) - 2];
        c_id[1] = temp[strlen(temp) - 1];
        *(ptr + sizeof(double)) = 1.00 * ((int)(*(ptr + sizeof(double)) + sizeof(double)) % (int)(N * sizeof(double))); // updating reading index
        // Exiting the critical section
        semaphore_op[0].sem_op = 1;
        if (semop(mutex_id, semaphore_op, 1) == -1)
        {
            perror("semaphore operation failed\n");
            exit(1);
        }
        if (semop(e, semaphore_op, 1) == -1)
        {
            perror("semaphore operation failed\n");
            exit(1);
        }
        //////////////////////Decoding Commodity ID/////////////////////////////////////////
        if (c_id[0] == '1' && c_id[1] == '1')
        {
            currentId = 0;
            writePriceIndex = commodityPriceIndicies[currentId];
            replacedPrice = prices[currentId][writePriceIndex];
            if (currentPrice > replacedPrice)
            {
                avgFlags[currentId] = 1;
            }
            else
            {
                avgFlags[currentId] = 0;
            }
            prices[currentId][writePriceIndex] = currentPrice;
            writePriceIndex = writePriceIndex + 1;
            if (writePriceIndex == 5)
            {
                writePriceIndex = 0;
            }
            commodityPriceIndicies[currentId] = writePriceIndex;
        }
        else if (c_id[0] == '1' && c_id[1] == '9')
        {
            currentId = 1;
            writePriceIndex = commodityPriceIndicies[currentId];
            replacedPrice = prices[currentId][writePriceIndex];
            if (currentPrice > replacedPrice)
            {
                avgFlags[currentId] = 1;
            }
            else
            {
                avgFlags[currentId] = 0;
            }
            prices[currentId][writePriceIndex] = currentPrice;
            writePriceIndex = writePriceIndex + 1;
            if (writePriceIndex == 5)
            {
                writePriceIndex = 0;
            }
            commodityPriceIndicies[currentId] = writePriceIndex;
        }
        else if (c_id[1] == '0')
        {
            if (c_id[0] == '1')
            { // ID 10 ZINC
                currentId = 10;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }
            else if (c_id[0] == '2')
            {
                currentId = 2;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }

            else if (c_id[0] == '3')
            {
                currentId = 3;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }

            else if (c_id[0] == '4')
            {
                currentId = 4;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }
            else if (c_id[0] == '5')
            {
                currentId = 5;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }

            else if (c_id[0] == '6')
            {
                currentId = 6;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }

            else if (c_id[0] == '7')
            {
                currentId = 7;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }
            else if (c_id[0] == '8')
            {
                currentId = 8;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }
            else if (c_id[0] == '9')
            {
                currentId = 9;
                writePriceIndex = commodityPriceIndicies[currentId];
                replacedPrice = prices[currentId][writePriceIndex];
                if (currentPrice > replacedPrice)
                {
                    avgFlags[currentId] = 1;
                }
                else
                {
                    avgFlags[currentId] = 0;
                }
                prices[currentId][writePriceIndex] = currentPrice;
                writePriceIndex = writePriceIndex + 1;
                if (writePriceIndex == 5)
                {
                    writePriceIndex = 0;
                }
                commodityPriceIndicies[currentId] = writePriceIndex;
            }
        }
        printf("\033[0m");
        printf("+-------------------------------------+\n");
        printf("| Currency \t|   Price   | AvgPrice |\n");
        for (int i = 0; i < 11; i++)
        {
            avg = 0;
            // Printing commodity names
            printf("\033[0m");
            if (strlen(commNames[i]) < 5)
            {
                printf("| %s \t\t", commNames[i]);
            }
            else
            {
                printf("| %s \t", commNames[i]);
            }
            // Getting current index
            index = commodityPriceIndicies[i];
            if (index == 0)
            {
                index = 4;
            }
            else
            {
                index = index - 1;
            }
            // Getting previous index
            prevIndex = index - 1;
            if (prevIndex == 0)
            {
                prevIndex = 4;
            }
            else
            {
                prevIndex = prevIndex - 1;
            }
            // Printing blue
            if (prices[i][index] == 0)
            {
                printf("|");
                printf("\033[0;34m");
                printf("   %.02f", prices[i][index]);
                printf("\033[0m");
                printf("    ");
            }
            else if (prices[i][index] > prices[i][prevIndex])
            { // Printing green
                printf("|");
                printf("\033[1;32m");
                if(prices[i][index]<=9.99){
                    printf("   %.02f", prices[i][index]);
                }
                else if(prices[i][index]<=99.99){
                    printf("  %.02f", prices[i][index]);
                }else if(prices[i][index]<=999.99){
                    printf(" %.02f", prices[i][index]);
                }else{
                    printf("%.02f", prices[i][index]);
                }
                cout << "\u2912";
                printf("\033[0m");
                printf("   ");
            }
            else
            { // Printing red
                printf("|");
                printf("\033[1;31m");
                if(prices[i][index]<=9.99){
                    printf("   %.02f", prices[i][index]);
                }
                else if(prices[i][index]<=99.99){
                    printf("  %.02f", prices[i][index]);
                }else if(prices[i][index]<=999.99){
                    printf(" %.02f", prices[i][index]);
                }else{
                    printf("%.02f", prices[i][index]);
                }
                cout << "\u2913";
                printf("\033[0m");
                printf("   ");
            }
            // Getting average
            for (int j = 0; j < 5; j++)
            {
                avg += prices[i][j];
            }
            avg = avg / 5;
            if (avg == 0)
            { // Printing blue
                printf("|");
                printf("\033[0;34m");
                printf("   %.02f", avg);
                printf("\033[0m");
                printf("   |\n");
            }
            else if (avgFlags[i])
            { // Printing green
                printf("|");
                printf("\033[1;32m");
                if(avg<=9.99){
                    printf("   %.02f", avg);
                }else if(avg<=99.99){
                    printf("  %.02f", avg);
                }else if(avg<=999.99){
                    printf(" %.02f", avg);
                }else{
                    printf("%.02f", avg);
                }
                cout << "\u2912";
                printf("\033[0m");
                printf("  |\n");
            }
            else
            { // Printing red
                printf("|");
                printf("\033[1;31m");
                if(avg<=9.99){
                    printf("   %.02f", avg);
                }else if(avg<=99.99){
                    printf("  %.02f", avg);
                }else if(avg<=999.99){
                    printf(" %.02f", avg);
                }else{
                    printf("%.02f", avg);
                }
                cout << "\u2913";
                printf("\033[0m");
                printf("  |\n");
            }
        }
        printf("\e[1;1H\e[2J");
        
    }
}

// printf("\033[1;31m"); red
// printf(“\033[0m”);    default
// printf("\033[1;32m"); green
// U+2912                up arrow
// U+2913                down arrow
// printf("\033[0;34m"); blue