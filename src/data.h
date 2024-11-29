#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "common.h"

typedef struct{
    Journal* records;
    int count;
} Database;

bool database_read_record(FILE* f, Journal* row){
    DateStr date_str_value;
    if(fscanf(f, "%s\t%lf\t%s", date_str_value, &row->amount, row->tag) == 3){
       return date_from_str(date_str_value, &row->date);
    } else{
        return false;
    }
}

int cmp(const void* a, const void* b) {
   Journal* p = (Journal*)a;
   Journal* q = (Journal*)b;
   return date_cmp(p->date, q->date);
}

void database_sort(Database* d){
    if(d->count > 0){
        qsort(d->records, d->count, sizeof(Journal), cmp);
    }
}

bool database_open(Database* d){
    FILE* f = fopen("journal.tsv", "r");
    
    d->count = 0;
    d->records = NULL;

    if(f != NULL){
        Journal j;
        int count = 0;
        while(database_read_record(f, &j)){
            count ++;
        }
        if(count > 0){
            d->records = (Journal*)malloc(sizeof(Journal) * count);
            rewind(f);
            int i = 0;
            while( i < count && database_read_record(f, &d->records[i])){
                i++;
            }
            d->count = i;
        }
        fclose(f);
        database_sort(d);
    }
    return d->count > 0;
}

void database_free(Database* d){
    if(d->records != NULL){
        free(d->records);
    }
    d->records = NULL;
    d->count = 0;
}

Journal* database_get(Database* d, int i){
    if(i > 0 && i < d->count){
        return &d->records[i];
    } else{
        return NULL;
    }
}

