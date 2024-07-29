#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

const int MAX_JOURNAL_COUNT = 100;

#define MAX_DATE_SIZE 10
#define MAX_TAG_SIZE  80
#define MAX_AMOUNT_SIZE 10

#define ERROR 1
#define SUCCESS 0

typedef char Date[MAX_DATE_SIZE + 1];
typedef char Tag[MAX_TAG_SIZE + 1];
typedef char Amount[MAX_AMOUNT_SIZE + 1];

typedef enum {
    COMMAND_UNKNOWN,
    COMMAND_INCOME,
    COMMAND_EXPENSE,
    COMMAND_SUMMARY
} Command;

typedef struct{
    Date date;
    double amount;
    Tag tag;
} Journal;


typedef struct {
    int expense_flag;
    int income_flag;
    int tag_flag;
    Amount income_value;
    Amount expense_value;
    Tag tag_value;
} Option;

int convert_str_to_double(char* value, double* out){
    char* end_ptr;
    *out = strtod(value, &end_ptr);
    if(*end_ptr == 0){
        return SUCCESS;
    }else {
        printf("Can not convert [%s] to amount! \n", value);
        return ERROR;
    }
}

void get_date(Date* out) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(*out, 11, "%Y.%m.%d", tm_info);
}


void print_usage() {
    printf("Usage:\n");
    printf(" - To record expense: jibim -e [amount] -t [tag]\n");
    printf(" - To record income: jibim -i [amount] -t [tag]\n");
}


int get_args(int argc, char** argv, Option* o) {
    int opt;
    do {
        opt = getopt(argc, argv, "i:t:e:");
        switch(opt) {
            case 'i':
                o->income_flag = 1;
                strncpy(o->income_value, optarg, MAX_AMOUNT_SIZE);
                break;
            case 't':
                o->tag_flag = 1;
                strncpy(o->tag_value, optarg, MAX_TAG_SIZE);
                break;
            case 'e':
                o->expense_flag = 1;
                strncpy(o->expense_value, optarg, 10);
                break;
            case -1:
                break;
            default:
                
                return EINVAL;
        }
    } while(opt != -1);
    return SUCCESS;
}

Command get_command(Option o){
    if (o.income_flag == 1 && o.expense_flag == 1){
        printf("Can not have income and expense together!\n");
        return COMMAND_UNKNOWN;
    }else if((o.income_flag == 1 || o.expense_flag == 1) && o.tag_flag != 1){
        printf("PLease specify a tag using -t!\n");
        return COMMAND_UNKNOWN;
    }else if (o.income_flag){
        return COMMAND_INCOME;
    }else if(o.expense_flag){
        return COMMAND_EXPENSE;
    }
    
    return COMMAND_SUMMARY;
}

int record_expense(Option o){
    double amount;  

    if (convert_str_to_double(o.expense_value, &amount) != SUCCESS){
        return ERROR;
    }
    Date date;
    get_date(&date);

    FILE* f = fopen("journal.tsv", "a");
    if ( f != NULL){
        fprintf(f, "%s\t%.2lf\t%s\n", date, amount, o.tag_value);
        fclose(f);
    }

    return SUCCESS;
}


int main(int argc, char** argv){
    Option o;
    int result;
    result = get_args(argc, argv, &o);
    if (result != 0){
        print_usage();
        return result;
    }
    Command cmd = get_command(o);
    switch(cmd){
        case COMMAND_UNKNOWN:
            print_usage();
            return ERROR;

        case COMMAND_EXPENSE:
            return record_expense(o);
            
    } 
    
    return SUCCESS;
}