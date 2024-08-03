#pragma once 
#include <stdio.h>

#define MAX_DATE_SIZE 10
#define MAX_TAG_SIZE  80
#define MAX_AMOUNT_SIZE 10

#define ERROR 1
#define SUCCESS 0

typedef char Date[MAX_DATE_SIZE + 1];
typedef char Tag[MAX_TAG_SIZE + 1];
typedef char Amount[MAX_AMOUNT_SIZE + 1];


typedef struct{
    Date date;
    double amount;
    Tag tag;
} Journal;

void debug_journal(Journal j){
    
    printf("%s\t%10.2lf$\t#%s\n", j.date, j.amount, j.tag );
    
}