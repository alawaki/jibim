#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "utils.h"

typedef enum {
    COMMAND_UNKNOWN,
    COMMAND_INCOME,
    COMMAND_EXPENSE,
    COMMAND_SUMMARY
} Command;



typedef struct {
    int expense_flag;
    int income_flag;
    int tag_flag;
    AmountStr income_value;
    AmountStr expense_value;
    TagStr tag_value;
} Option;

void print_usage() {
    printf("Usage:\n");
    printf(" - To record expense: jibim -e [amount] -t [tag]\n");
    printf(" - To record income: jibim -i [amount] -t [tag]\n");
}

int get_args(int argc, char** argv, Option* o) {
    int opt;
    o->income_flag = 0;
    o->expense_flag = 0;
    o->tag_flag = 0;
    do {
        opt = getopt(argc, argv, "i:t:e:s:");
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

int append(char* amount_value, char* tag_value, double mul){
    Date dt;
    double amount;  

    date_now(&dt);

    if (!amount_from_str(amount_value, &amount)){
        return ERROR;
    }
    
    if(amount <= 0.0){
        printf("Amount[%.2lf] can not be less than zero!\n", amount);
        return ERROR;
    }

    FILE* f = fopen("journal.tsv", "a");
    if ( f != NULL){
        fprintf(f, "%04d.%02d.%02d\t%.2lf\t%s\n", dt.year, dt.month, dt.day, mul * amount, tag_value);
        fclose(f);
    }

    return SUCCESS;
}

int record_expense(Option o){
    return append(o.expense_value, o.tag_value, -1.0);
}

int record_income(Option o){
    return append(o.income_value, o.tag_value, 1.0);
}

int today_expenses(){
    Journal row;
    Date today;
    DateStr date;
    double sum_e =0;
    double sum_i =0;

    date_now(&today);

    FILE* f = fopen("journal.tsv", "r");
    if ( f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);
        if (date_eq(row.date, today) == true){
            if(row.amount < 0 || row.amount > 0){
                //debug_journal(row);
                if (row.amount < 0){
                    sum_e = sum_e + row.amount;
                }else
                    sum_i = sum_i + row.amount;
            }
        }
    }

    fclose(f);

    
    printf("Today:\033[31m%15.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
   
}

int last_seven_day_expense(){
    Journal row;
    Date today;
    Date seven_days_ago;
    DateStr date;
    double sum_e =0;
    double sum_i =0;

    FILE* f = fopen("journal.tsv", "r");
    if ( f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    date_now(&today);
    date_add_days(today, -7, &seven_days_ago);

    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);    
        if (date_gte(row.date, seven_days_ago)){
            //debug_journal(row);
            if(row.amount < 0){
                sum_e = sum_e + row.amount;
            }else
                sum_i = sum_i + row.amount;
        }
            
    }   

    fclose(f);

    printf("Last week:\033[31m%11.2lf$\033[0m\033[32m%18.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
}

int last_month_expense(){
    Journal row;
    Date today;
    Date first_ofthe_month;
    Date end_ofthe_month;
    DateStr date;
    double sum_e = 0;
    double sum_i = 0;

    FILE* f= fopen("journal.tsv", "r");
    if (f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    date_now(&today);
    date_add_month(today, -1, &first_ofthe_month);
    date_end_of_month(first_ofthe_month, &end_ofthe_month);
    date_begin_of_month(first_ofthe_month, &first_ofthe_month);
    
    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);    
        if (date_gte(row.date, first_ofthe_month) && date_lte(row.date, end_ofthe_month)){
            //debug_journal(row);
            if(row.amount < 0){
                sum_e = sum_e + row.amount;
            }else
                sum_i = sum_i + row.amount;
        }    
    }   

    fclose(f);

    printf("Last month:\033[31m%13.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
}

int last_three_month_expense(){
    Journal row;
    Date today;
    Date three_month_ago;
    DateStr date;
    double sum_e = 0;
    double sum_i = 0;

    FILE* f= fopen("journal.tsv", "r");
    if (f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    date_now(&today);
    date_add_month(today, -3, &three_month_ago);

    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);    
        if (date_gte(row.date, three_month_ago)){
            //debug_journal(row);
            if(row.amount < 0){
                sum_e = sum_e + row.amount;
            }else
                sum_i = sum_i + row.amount;
        }    
    }   

    fclose(f);

    printf("Last 3 month:\033[31m%12.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;

}

int last_six_month_expense(){
    Journal row;
    Date today;
    Date six_month_ago;
    DateStr date;
    double sum_e = 0;
    double sum_i = 0;

    FILE* f = fopen("journal.tsv", "r");
    if (f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    date_now(&today);
    date_add_month(today, -6, &six_month_ago);

    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);    
        if (date_gte(row.date, six_month_ago)){
            //debug_journal(row);
            if (row.amount < 0){
                sum_e = sum_e + row.amount;
            }else
                sum_i = sum_i + row.amount;
        }    
    }

    fclose(f);

    printf("Last 6 month:\033[31m%12.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;    
}

int last_year_expense(){
    Journal row;
    Date today;
    Date one_year_ago;
    DateStr date;
    double sum_e = 0;
    double sum_i = 0;

    FILE* f = fopen("journal.tsv", "r");
    if( f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }

    date_now(&today);
    today.year --;
    date_begin_of_year(today, &one_year_ago);
    date_end_of_year(today, &today);

    while(fscanf(f, "%s\t%lf\t%s", date, &row.amount, row.tag) == 3){        
        date_from_str(date, &row.date);    
        if (date_eq(row.date, one_year_ago) && date_lte(row.date, today)){
            //debug_journal(row);
            if(row.amount < 0 ){
                sum_e = sum_e + row.amount;
            }else
                sum_i = sum_i + row.amount;
        }    
    }

    fclose(f);

    printf("Last year:\033[31m%11.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
}

int print_summary(){
    Journal row;
    double sum_e = 0;
    double sum_i = 0;
    DateStr date;
    int max_rows_to_print = 5;

    FILE* f = fopen("journal.tsv", "r");
    if ( f == NULL){
        printf("Nothing to summrize!\n");
        return ERROR;
    }
    
    while(fscanf(f, "%s\t%lf\t%s",date, &row.amount, row.tag) == 3){
        if (row.amount < 0){
            sum_e = sum_e + row.amount;
        }else
            sum_i = sum_i + row.amount;       
        max_rows_to_print--;
        if(max_rows_to_print >= 0){
            date_from_str(date, &row.date);
            debug_journal(row);       
        }
    }

    fclose(f);
    printf("\nDate                 expense         income      \n");
    printf("---------------------------------------------\n");
    printf("Current:\033[31m%17.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    today_expenses();
    last_seven_day_expense();
    last_month_expense();
    last_three_month_expense();
    last_six_month_expense();
    last_year_expense();

    return SUCCESS;
}

int main(int argc, char** argv){
    Option o;
    Journal j;
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
        case COMMAND_INCOME:
            return record_income(o);
        case COMMAND_SUMMARY:
            return print_summary();
      
    } 
    return SUCCESS;
}