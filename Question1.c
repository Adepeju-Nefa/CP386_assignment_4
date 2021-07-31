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









int readfile(char* filename, Customer** customers) {

	FILE *file = fopen(filename, "r");
	if(!file)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
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
