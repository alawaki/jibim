#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "utils.h"
#include "data.h"

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

int get_expense(Database* d, int i){
    Date today;
    Date first_ofthe_month;
    Date end_ofthe_month;
    double sum_e = 0;

    date_now(&today);
    date_add_month(today, -i, &first_ofthe_month);
    date_end_of_month(first_ofthe_month, &end_ofthe_month);
    date_begin_of_month(first_ofthe_month, &first_ofthe_month);

    
    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, first_ofthe_month) && date_lte(d->records[i].date, end_ofthe_month) == true){
            if(d->records[i].amount < 0){
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }
    return sum_e;
} 

int today_expenses(Database* d){
    Date today;
    double sum_e =0;
    double sum_i =0;

    date_now(&today);
    
    for(int i = 0; i < d->count; i++){
        if(date_eq(d->records[i].date, today) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Today:\033[31m%17.2lf$\033[0m\033[32m%13.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;   
}

int last_seven_day_expense(Database* d){
    Date today;
    Date seven_days_ago;
    double sum_e =0;
    double sum_i =0;

    date_now(&today);
    date_add_days(today, -7, &seven_days_ago);
    
    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, seven_days_ago) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Last week:\033[31m%13.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
}

int last_month_expense(Database* d){
    Date today;
    Date first_ofthe_month;
    Date end_ofthe_month;
    double sum_e = 0;
    double sum_i = 0;

    date_now(&today);
    date_add_month(today, -1, &first_ofthe_month);
    date_end_of_month(first_ofthe_month, &end_ofthe_month);
    date_begin_of_month(first_ofthe_month, &first_ofthe_month);
    
    
    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, first_ofthe_month) && date_lte(d->records[i].date, end_ofthe_month) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Last month:\033[31m%13.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;
}

int last_three_month_expense(Database* d){
    Date today;
    Date three_month_ago;
    double sum_e = 0;
    double sum_i = 0;

    date_now(&today);
    date_add_month(today, -3, &three_month_ago);

    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, three_month_ago) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Last 3 month:\033[31m%12.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);

    return SUCCESS;

}

int last_six_month_expense(Database* d){
    Date today;
    Date six_month_ago;
    double sum_e = 0;
    double sum_i = 0;

    date_now(&today);
    date_add_month(today, -6, &six_month_ago);

    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, six_month_ago) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Last 6 month:\033[31m%12.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);
    
    return SUCCESS;    
}

int last_year_expense(Database* d){
    Date today;
    Date one_year_ago;
    double sum_e = 0;
    double sum_i = 0;

    date_now(&today);
    today.year --;
    date_begin_of_year(today, &one_year_ago);
    date_end_of_year(today, &today);

    
    for(int i = 0; i < d->count; i++){
        if(date_gte(d->records[i].date, one_year_ago) && date_lte(d->records[i].date, today) == true){
            if(d->records[i].amount > 0){
                sum_i = sum_i + d->records[i].amount;
            }else {
                sum_e = sum_e + d->records[i].amount;
            }    
        }
    }

    printf("Last year:\033[31m%15.2lf$\033[0m\033[32m%15.2lf$\033[0m\n\n", sum_e, sum_i);

    return SUCCESS;
}

void print_chart_expense(Database* d){
    Date today;
    int data[6];
    int month;
    double sum = 0;
    date_now(&today);
    month = today.month;
    
    for(int i = 0; i <= 5; i++){
        sum  = get_expense(d, month);
        printf("((s:%lf))\n ", sum);
        data[i-1] = (int)sum * -1;
        if(month < 1 ){
            today.year --;
            month = 12;
        }
        printf("m:%d,i:%d,d:%d\t", month, i, data[i-1]);
        month--;
    }
    
    for(int i = 0; i <=5; i++){
        printf("i(%d):d(%d)\n", i, data[i-1]);
    }
    int max = 0;
    char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    for (int i = 5; i >= 0; i--){
        printf("%d,[%d]", i, data[i-1]);
        if(data[i-1] > max){
            max = data[i-1];
            printf("%d,(%d)\t", i, max);
        }
    }
    
    for (int level = max; level > 0; level -= 500){
        for (int j = 0; j < 6; j++){
            if (data[j] >= level){
                printf(" █  ");
            } else {
                printf("    ");
            }
        }
        printf("\n");
    }

    printf("-----------------------------------------------\n");

    for (int q = 0; q < month; q++){
        printf("%s ", months[q]);
    }
    printf("\n");
}

int print_summary(Database* d){
    double sum_e = 0;
    double sum_i = 0;
    double jibim = 0;
    DateStr date;

    int i = d->count - 1;
    printf("%d\n", d->count);
    for(int j = 0; j < 5; j++){
        debug_journal(&d->records[i]);    
        i--;
    }

    for(int j = 0; j < d->count; j++){
        if(d->records[j].amount > 0){
            sum_i = sum_i + d->records[j].amount;
        }else {
            sum_e = sum_e + d->records[j].amount;
        }
    }

    jibim = sum_e + sum_i;

    printf("\nTotal:\033[31m%19.2lf$\033[0m\033[32m%15.2lf$\033[0m\n", sum_e, sum_i);
    printf("Jibim:\033[32m%18.2lf$\033[0m\n", jibim);
    printf("\nDate             Expense         Income      \n");
    printf("---------------------------------------------\n");
    
    today_expenses(d);
    last_seven_day_expense(d);
    last_month_expense(d);
    last_three_month_expense(d);
    last_six_month_expense(d);
    last_year_expense(d);
    
    print_chart_expense(d);
    
    return 0;
}

int main(int argc, char** argv){
    Option o;
    int result;
    result = get_args(argc, argv, &o);
    if (result != 0){
        print_usage();
        return result;
    }
    
    Database d;
    database_open(&d);

    Command cmd = get_command(o);
    switch(cmd){
        case COMMAND_UNKNOWN:
            print_usage();
            result = ERROR;
            break;
        case COMMAND_EXPENSE:
            result = record_expense(o);
            break;
        case COMMAND_INCOME:
            result = record_income(o);
            break;
        case COMMAND_SUMMARY:
            result = print_summary(&d);
            break;
      
    } 
    database_free(&d); 
    return result;
}