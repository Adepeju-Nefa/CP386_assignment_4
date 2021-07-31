/*
 -------------------------------------
 File:    Question1.c
 Project: Assignment 4
 file description: Using banker's algorithm to solve deadlock problems
 -------------------------------------
 Authors:  Adepeju Olowonefa and Lovette Oyewole
 ID:      193049300,  
 Email:   olow9300@mylaurier.ca, 
 github: https://github.com/Adepeju-Nefa/CP386_assignment_4
 Version;  2021-07-29
 -------------------------------------
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define FILE_NAME "sample4_in.txt"

typedef struct customer
{
	//int cusID;
	int resource_1;
	int resource_2;
	int resource_3;
	int resource_4;
} Customer;
int read_File(char* fileName, Customer** customer);
int safety_Algorithm(int customerCount);
void request_Resource(int threadID, int item1, int item2, int item3, int item4, int customerCount);
void release_Resource(int threadID, int item1, int item2, int item3, int item4);
void outputValues(int customerCount);
void run(int customerCount);
void *runThread(void *t);

int available[5]; //available array

int safeSeq[5];
Customer* customermax = NULL;    // max number of resources needed
Customer* customeralloc = NULL;  // currently allocated resources
Customer* customerneed = NULL;   // remaining resources (maxneeded-currently allocated)
int i;







int read_File(char* filename, Customer** customers) {

	FILE *file = fopen(filename, "r");
	if(!file)
	{
		printf("There was an error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(file), &st);
	char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
	fileContent[0]='\0';

	while(!feof(file))
	{
		char line[100];
		if(fgets(line,100,file)!=NULL)
		{
			strncat(fileContent,line,strlen(line));
		}
	}
	fclose(file);

	char* command = NULL;
	int customer_Count = 0;


	char* fileCopy = (char*)malloc((strlen(fileContent)+1)*sizeof(char));
	strcpy(fileCopy,fileContent);
	command = strtok(fileCopy,"\r\n");
	while(command!=NULL)
	{
		customer_Count++;
		command = strtok(NULL,"\r\n");
	}
	
	*customers = (Customer*) malloc(sizeof(Customer)*customer_Count);

	char* lines[customer_Count];
	command = NULL;
	int i=0;
	command = strtok(fileContent,"\r\n");
	while(command!=NULL)
	{
		lines[i] = malloc(sizeof(command)*sizeof(char));
		strcpy(lines[i],command);
		i++;
		command = strtok(NULL,"\r\n");
	}

	for(int k=0; k<customer_Count; k++)
	{
		char* token = NULL;
		int j = 0;
		token =  strtok(lines[k],",");
		while(token!=NULL)
		{
			if(j==0){
				(*customers)[k].resource_1 = atoi(token);
				}
			if(j==1){
				(*customers)[k].resource_2 = atoi(token);
				}
			if(j==2){
				(*customers)[k].resource_3 = atoi(token);
				}
			if(j==3){
				(*customers)[k].resource_4 = atoi(token);
				}
			j++;
			token = strtok(NULL,",");
		}
	}
	return customer_Count;
}


void run(int customerCount){
    int k=safetyAlgorithm(customerCount);
	if (k==0)
	{
		printf("UNSAFE: Please check thread before continuing\n");
		return;
	}
	else{

		for (i=0;i<customerCount;i++){ //create and execute threads
			int runnable = safeSeq[i];

			pthread_t threadID;
			pthread_attr_t newThread;
			pthread_attr_init(&newThread);

			pthread_create(&threadID, &newThread, runThread, (void *)&runnable);


			pthread_join(threadID, NULL);
		}
	}
	

	return;

}


void request_Resource(int threadID, int item1, int item2, int item3, int item4, int customerCount)
{
	if (item1<=customerneed[threadID].resource_1 && item2<=customerneed[threadID].resource_2 &&
	item3<=customerneed[threadID].resource_3 && item4<=customerneed[threadID].resource_4)	
	{
		if(item1 <= available[1] && item2 <= available[2] && 
		item3 <= available[3] && item4 <= available[4])
		{

			available[1] -= item1;
			available[2] -= item2;
			available[3] -= item3;
			available[4] -= item4;


			customeralloc[threadID].resource_1+= item1;
			customeralloc[threadID].resource_2+= item2;
			customeralloc[threadID].resource_3+= item3;
			customeralloc[threadID].resource_4+= item4;

			customerneed[threadID].resource_1-= item1;
			customerneed[threadID].resource_2-= item2;
			customerneed[threadID].resource_3-= item3;
			customerneed[threadID].resource_4-= item4;

			int safe = safetyAlgorithm(customerCount);

			if (safe == 0)
			{
				available[1] += item1;
				available[2] += item2;
				available[3] += item3;
				available[4] += item4;

				customeralloc[threadID].resource_1-= item1;
				customeralloc[threadID].resource_2-= item2;
				customeralloc[threadID].resource_3-= item3;
				customeralloc[threadID].resource_4-= item4;

				customerneed[threadID].resource_1+= item1;
				customerneed[threadID].resource_2+= item2;
				customerneed[threadID].resource_3+= item3;
				customerneed[threadID].resource_4+= item4;	
				printf("insuffiecient resources, need to wait\n");
			}
			else
			{
				printf("request completed sucessfully\n");
			}
			

		}
		else
		{
			printf("can not request more than available resources\n");
		}
		
	}
	else
	{
		printf("can not request more than needed resources\n");
	}
	

	return;
}




void release_Resource(int threadID, int item1, int item2, int item3, int item4)
{
	if (item1<=customeralloc[threadID].resource_1 && item2<=customeralloc[threadID].resource_2 &&
	item3<=customeralloc[threadID].resource_3 && item4<=customeralloc[threadID].resource_4)
	{
		available[1] += item1;
		available[2] += item2;
		available[3] += item3;
		available[4] += item4;

		customeralloc[threadID].resource_1-= item1;
		customeralloc[threadID].resource_2-= item2;
		customeralloc[threadID].resource_3-= item3;
		customeralloc[threadID].resource_4-= item4;

		customerneed[threadID].resource_1+= item1;
		customerneed[threadID].resource_2+= item2;
		customerneed[threadID].resource_3+= item3;
		customerneed[threadID].resource_4+= item4;
		printf("Resources released sucessfully\n");
	}
	else
	{
		printf("Cannot release more than allocated resources\n");
	}
	
	return;
}

void *runThread(void *t){

    int *tid = (int*)t;
	printf("- -> Customer/Thread %d\n", *tid);
	printf("	Allocated resources: ");
    printf("%d ",customeralloc[*tid].resource_1);
	printf("%d ",customeralloc[*tid].resource_2);
	printf("%d ",customeralloc[*tid].resource_3);
	printf("%d\n",customeralloc[*tid].resource_4);

	printf("	Needed: ");
    printf("%d ",customerneed[*tid].resource_1);
	printf("%d ",customerneed[*tid].resource_2);
	printf("%d ",customerneed[*tid].resource_3);
	printf("%d\n",customerneed[*tid].resource_4);

	printf("	Available:\n");
    printf("%d ",available[1]);
	printf("%d ",available[2]);
	printf("%d ",available[3]);
	printf("%d\n",available[4]);

	printf("	Thread has started:\n");
	printf("	Thread has finished:\n");
	printf("	Thread is releasing resources:\n");
	printf("	New Available:\n");
      printf("%d ",available[1] + customeralloc[*tid].resource_1);
      printf("%d ",available[2] + customeralloc[*tid].resource_2);
      printf("%d ",available[3] + customeralloc[*tid].resource_3);
      printf("%d ",available[4] + customeralloc[*tid].resource_4);
    
	return NULL;
}
