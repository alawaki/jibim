#pragma once 
#include <stdio.h>

#define MAX_DATE_SIZE 10
#define MAX_TAG_SIZE  80
#define MAX_AMOUNT_SIZE 10

#define ERROR 1
#define SUCCESS 0

typedef char DateStr[MAX_DATE_SIZE + 1];
typedef char TagStr[MAX_TAG_SIZE + 1];
typedef char AmountStr[MAX_AMOUNT_SIZE + 1];

typedef struct{
    int year;
    int month;
    int day;
} Date;

typedef struct{
    Date date;
    double amount;
    TagStr tag;
} Journal;
