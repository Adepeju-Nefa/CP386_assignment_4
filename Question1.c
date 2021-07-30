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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
