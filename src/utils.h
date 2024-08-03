#pragma once 
#include <time.h>
#include "common.h"

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
