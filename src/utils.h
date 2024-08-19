#pragma once 
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "common.h"


void date_now(Date* out){
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    
    out->year = tm_info->tm_year;
    out->year = out->year + 1900;
    out->month = tm_info->tm_mon;
    out->day = tm_info->tm_mday;

}

int date_to_int(Date a){
    int result = 0;
    result = (a.year * 10000) + (a.month * 100) + (a.day);
    return result;
}

bool date_eq(Date a, Date b){
    return a.year == b.year && a.month == b.month && a.day == b.day;
}

bool date_gt(Date a, Date b){
    return date_to_int(a) > date_to_int(b);
}

int date_gte(Date a, Date b){
    return date_to_int(a) >= date_to_int(b);
}

bool date_lt(Date a, Date b){
    return date_to_int(a) < date_to_int(b);
}

bool date_lte(Date a, Date b){
    return date_to_int(a) <= date_to_int(b);
}

int date_cmp(Date a, Date b){
    int x = date_to_int(a);
    int y = date_to_int(b);

    if(x > y){
        return 1;
    }else if(x < y){
        return -1;
    }
    return SUCCESS;
}

void date_to_str(Date a, char* str){
    sprintf(str, "%04d.%02d.%02d", a.year, a.month, a.day);
}

bool date_from_str(char* str, Date* out){
    return sscanf(str, "%04d.%02d.%02d", &out->year, &out->month, &out->day) == 3;
}

bool amount_from_str(char* value, double* out){
    char* end_ptr;
    *out = strtod(value, &end_ptr);
    return *end_ptr == 0;
        
}

bool is_leap_year(int year){
        if ( year%4 != 0){
            return false;
        }else if (year%100 != 0){
            return true;
        }else if (year%400 != 0){
            return false;
        }else{
            return true;
        }
        
}

int days_in_month(int year, int month){
    int days;
    if( month != 2){
        switch (month){         
            case 4:
            case 6:
            case 9:
            case 11:
                days = 30;
                break;
            default :
                days = 31;
                break;
        }
    }else{
        days = is_leap_year(year) ? 29 : 28;
    }
    return days;
}

void date_begin_of_year(Date a, Date* out){
    out->year = a.year;
    out->month = 1;
    out->day = 1;
}

void date_end_of_year(Date a, Date* out){
    out->year = a.year;
    out->month = 12;
    out->day = 31;
}

void date_begin_of_month(Date a, Date* out){
    out->year = a.year;
    out->month = a.month;
    out->day = 1;
}

void date_end_of_month(Date a, Date* out){
    out->year = a.year;
    out->month = a.month;
    out->day = days_in_month(a.year, a.month);
}


void date_add_days(Date* a, int days){
    
    if(days < 0){
        a->day --;
        if(a->day < 1){
            a->month --;
            if(a->month < 1){
                a->month = 12;
                a->year --;
            }
            a->day = days_in_month(a->year, a->month);
        }
        if(days < -1){
            date_add_days(a, days + 1);
        }
    }else if (days > 0){
        a->day ++;
        if(a->day > days_in_month(a->year, a->month)){
            a->day = 1;
            a->month ++;
            if(a->month > 12){
                a->month = 1;
                a->year ++;
            }
        }
        if(days > 1){
            date_add_days(a, days - 1);
        }
        
    }

}

void debug_journal(Journal j){
    
    DateStr dt;
    date_to_str(j.date, dt);
    if (j.amount < 0){
        printf("%s\t\033[31m%10.2lf$\033[0m\t#%s\n", dt, j.amount, j.tag );
    }else if (j.amount > 0){
        printf("%s\t\033[32m%10.2lf$\033[0m\t#%s\n", dt, j.amount, j.tag );
    }
    
}