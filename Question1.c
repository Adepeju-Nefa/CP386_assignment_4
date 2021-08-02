/*
 -------------------------------------
 File:    Question1.c
 Project: Assignment 4
 file description: Using banker's algorithm to solve deadlock problems
 -------------------------------------
 Authors:  Adepeju Olowonefa and Lovette Oyewole
 ID:      193049300,  190888960
 Email:   olow9300@mylaurier.ca, oyew8960@mylaurier.ca
 github: https://github.com/Adepeju-Nefa/CP386_assignment_4
 Version;  2021-07-29
 -------------------------------------
 */


//import 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

//define constants
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define FILE_NAME "sample4_in.txt"

typedef struct customer
{
	int resource_1;
	int resource_2;
	int resource_3;
	int resource_4;
} Customer;

//define functions
int read_File(char* fileName, Customer** customer);
int safety_Algorithm(int customer_count);
void request_Resource(int tid, int num1, int num2, int num3, int num4, int customer_count);
void release_Resource(int tid, int num1, int num2, int num3, int num4);
void show_values(int customerCount);
void run(int customer_count);
void *runThread(void *t);

int available_array[5]; 

int safe_sequence[5];
//define the maximum numbrer of customers, the allocated resources, remaining resources
Customer* max_customer = NULL;    
Customer* customer_allocation = NULL;  
Customer* customerneed = NULL;   

int i;
int main(int argc, char *argv[])
{
	char *file_name = "sample4_in.txt";

	if(argc!=5) 
	{
		printf("Invalid input, Enter 5 values\n");	
		return -1;
	}
	else
	{
		//load characters into array
		for (i=1; i<argc; i++) 
		{
			available_array[i]=atoi(argv[i]);
			
		}
	}

	int customerCount = read_File(file_name, &max_customer);

	//define size of customer allocation
	customer_allocation = (Customer*) malloc(sizeof(Customer)*customerCount);
	customerneed = (Customer*) malloc(sizeof(Customer)*customerCount);
	for(i =0; i <customerCount;i++)
	{
		// initialize all the customer allocation to zero
		customer_allocation[i].resource_1 = 0;
		customer_allocation[i].resource_2 = 0;
		customer_allocation[i].resource_3 = 0;
		customer_allocation[i].resource_4 = 0;

		//initial allocation is 0 so, need = max (for now)
		customerneed[i].resource_1 = max_customer[i].resource_1;
		customerneed[i].resource_2 = max_customer[i].resource_2;
		customerneed[i].resource_3 = max_customer[i].resource_3;
		customerneed[i].resource_4 = max_customer[i].resource_4;
	}
	// print number of customers
	printf("Number of Customers: %d\n",customerCount);
	//print number of resources available
	printf("Currently available resources: %d %d %d %d\n", available_array[1],available_array[2],available_array[3],available_array[4]);
	//print out all the max resources
	printf("Maximum Resources from file:\n");

	//loop to print out all resources in file
	for (i=0; i<customerCount; i++) 
	{
		printf("%d, %d, %d, %d\n", max_customer[i].resource_1,max_customer[i].resource_2,max_customer[i].resource_3,max_customer[i].resource_4);
	}
	

	//initialize
	char line[100];
	char cmd[2];
	int tid =-1;
	int num1 =-1;
	int num2 =-1;
	int num3 =-1;
	int num4 =-1;

	
	do {
        
		printf("Enter Command: ");
		fgets(line,100,stdin);


		char *ptr = strtok(line, " ");

		strcpy(cmd,ptr);

		int j =0;
		while(ptr!=NULL)
		{
			switch(j){
				case 1:
					tid = atoi(ptr);
					break;
				case 2:
					num1 = atoi(ptr);
					break;
				case 3:
					num2 = atoi(ptr);
					break;
				case 4:
					num3 = atoi(ptr);
					break;
				default:
					num4 = atoi(ptr);
			}
			
			j++;
			ptr = strtok(NULL," ");
		}
		
		if (strstr(cmd,"RQ")!=NULL)
		{
		//if command is RQ call out the trelease resources function and
		// print out values
			printf("State is safe, and request is satisfied\n");
			
			
			request_Resource(tid,num1,num2,num3,num4,customerCount);
			

		}
		//if command is RL call out the trelease resources function and
		// print out values
		else if(strstr(cmd,"RL")!=NULL)
		{
		
			release_Resource(tid,num1,num2,num3,num4);
		}
		// if command is Status, call the show values function
		// print the customer status
		else if(strstr(cmd,"Status")!=NULL)
		{
			
			show_values(customerCount);

		}
		else if(strstr(cmd,"RUN")!=NULL)
		{
			printf("run function\n");
			/*
			do something
			*/

			printf("You have typed: %s\n\n", cmd);
			run(customerCount);
		}		
		else if(strstr(cmd,"Exit")!=NULL)
		{
			printf("End of program!\n");
			break;
		}
		else
		{
			printf("invalid command try inputting 'RQ,RL,Status or RUN' [Case Sensitive]\n");
		}
		

    } while (1);

}



//function to read from file
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

// code to run the program
void run(int customerCount){
    int k=safety_Algorithm(customerCount);
	if (k==0)
	{
		printf("check thread\n");
		return;
	}
	else{

		for (i=0;i<customerCount;i++){ 
			int runnable = safe_sequence[i];

			pthread_t tid;
			pthread_attr_t newThread;
			pthread_attr_init(&newThread);

			pthread_create(&tid, &newThread, runThread, (void *)&runnable);


			pthread_join(tid, NULL);
		}
	}
	

	return;

}

//code to request resources from 

void request_Resource(int tid, int num1, int num2, int num3, int num4, int customerCount)
{
	if (num1<=customerneed[tid].resource_1 && num2<=customerneed[tid].resource_2 &&
	num3<=customerneed[tid].resource_3 && num4<=customerneed[tid].resource_4)	
	{
		if(num1 <= available_array[1] && num2 <= available_array[2] && 
		num3 <= available_array[3] && num4 <= available_array[4])
		{

			available_array[1] -= num1;
			available_array[2] -= num2;
			available_array[3] -= num3;
			available_array[4] -= num4;


			customer_allocation[tid].resource_1 += num1;
			customer_allocation[tid].resource_2 += num2;
			customer_allocation[tid].resource_3 += num3;
			customer_allocation[tid].resource_4 += num4;

			customerneed[tid].resource_1 -= num1;
			customerneed[tid].resource_2 -= num2;
			customerneed[tid].resource_3 -= num3;
			customerneed[tid].resource_4 -= num4;

			int is_Safe = safety_Algorithm(customerCount);

			if (is_Safe != 0)
			{
                printf("request completed sucessfully\n");
				
			}
			else
			{
				available_array[1] += num1;
				available_array[2] += num2;
				available_array[3] += num3;
				available_array[4] += num4;

				customer_allocation[tid].resource_1 -= num1;
				customer_allocation[tid].resource_2 -= num2;
				customer_allocation[tid].resource_3 -= num3;
				customer_allocation[tid].resource_4 -= num4;

				customerneed[tid].resource_1 += num1;
				customerneed[tid].resource_2 += num2;
				customerneed[tid].resource_3 += num3;
				customerneed[tid].resource_4 += num4;	

				printf("Insuffiecient resources\n");
			}
			

		}
		else
		{
			printf("Cannot request more than the available resources\n");
		}
		
	}
	else
	{
		printf("Cannot request more than the needed resources\n");
	}
	

	return;
}



void release_Resource(int tid, int num1, int num2, int num3, int num4)
{
	if (num1<=customer_allocation[tid].resource_1 && num2<=customer_allocation[tid].resource_2 &&
	num3<=customer_allocation[tid].resource_3 && num4<=customer_allocation[tid].resource_4)
	{
		available_array[1] += num1;
		available_array[2] += num2;
		available_array[3] += num3;
		available_array[4] += num4;

		customer_allocation[tid].resource_1-= num1;
		customer_allocation[tid].resource_2-= num2;
		customer_allocation[tid].resource_3-= num3;
		customer_allocation[tid].resource_4-= num4;

		customerneed[tid].resource_1+= num1;
		customerneed[tid].resource_2+= num2;
		customerneed[tid].resource_3+= num3;
		customerneed[tid].resource_4+= num4;
		
	}
	else
	{
		printf("Cannot release more than allocated resources\n");
	}
	
	return;
}

// funtion to riun the thread
void *runThread(void *t){

    int *tid = (int*)t;
	printf("- -> Customer/Thread %d\n", *tid);
	printf("	Allocated resources: ");
    printf("%d ",customer_allocation[*tid].resource_1);
	printf("%d ",customer_allocation[*tid].resource_2);
	printf("%d ",customer_allocation[*tid].resource_3);
	printf("%d\n",customer_allocation[*tid].resource_4);

	printf("	Needed: ");
    printf("%d ",customerneed[*tid].resource_1);
	printf("%d ",customerneed[*tid].resource_2);
	printf("%d ",customerneed[*tid].resource_3);
	printf("%d\n",customerneed[*tid].resource_4);

	printf("	available:\n");
    printf("%d ",available_array[1]);
	printf("%d ",available_array[2]);
	printf("%d ",available_array[3]);
	printf("%d\n",available_array[4]);

	printf("	Thread has started:\n");
	printf("	Thread has finished:\n");
	printf("	Thread is releasing resources:\n");
	printf("	New Available:\n");
      printf("%d ",available_array[1] + customer_allocation[*tid].resource_1);
      printf("%d ",available_array[2] + customer_allocation[*tid].resource_2);
      printf("%d ",available_array[3] + customer_allocation[*tid].resource_3);
      printf("%d ",available_array[4] + customer_allocation[*tid].resource_4);
    
	return NULL;
}


void show_values(int customerCount)

{
	printf("Currently available resources: %d %d %d %d\n", available_array[1],available_array[2],available_array[3],available_array[4]);
	printf("Maximum Resources from file:\n");

	for (i=0; i<customerCount; i++) //print customer items
	{
		printf("%d, %d, %d, %d\n", max_customer[i].resource_1,max_customer[i].resource_2,max_customer[i].resource_3,max_customer[i].resource_4);
	}

	printf("current alloc\n");
	for (i=0; i<customerCount; i++) 
	{
		printf("%d, %d, %d, %d\n", customer_allocation[i].resource_1,customer_allocation[i].resource_2,customer_allocation[i].resource_3,customer_allocation[i].resource_4);
	}

	printf("still needed\n");
	for (i=0; i<customerCount; i++) 
	{
		printf("%d, %d, %d, %d\n", customerneed[i].resource_1,customerneed[i].resource_2,customerneed[i].resource_3,customerneed[i].resource_4);
	}
	return;
}

int safety_Algorithm(int customerCount)

{
	
	int finish[5] = {1,1,1,1,1};

	
	int available_copy[5];
	Customer* alloc_copy = NULL;

	Customer* needed_copy = NULL;
	alloc_copy = (Customer*) malloc(sizeof(Customer)*customerCount);
	needed_copy = (Customer*) malloc(sizeof(Customer)*customerCount);
	int customerCount_Copy = customerCount;

	for(i=1;i<5;i++)
		available_copy[i] = available_array[i];



	

	for(i =0; i <customerCount;i++)
	{
		
		//alloc_copy[i].cusID = customermax[i].cusID;
		alloc_copy[i].resource_1 = customer_allocation[i].resource_1;
		alloc_copy[i].resource_2 = customer_allocation[i].resource_2;
		alloc_copy[i].resource_3 = customer_allocation[i].resource_3;
		alloc_copy[i].resource_4 = customer_allocation[i].resource_4;
		
		//needed_copy[i].cusID = customermax[i].cusID;
		needed_copy[i].resource_1 = customerneed[i].resource_1;
		needed_copy[i].resource_2 = customerneed[i].resource_2;
		needed_copy[i].resource_3 = customerneed[i].resource_3;
		needed_copy[i].resource_4 = customerneed[i].resource_4;
	}
	int safe = 0;//false
	


	return safe;
}