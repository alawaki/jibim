#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int get_amount(int* sum){
    
    
    return 0;
}

/*%04d.%02d.%02d\t%.2lf\t%s\n*/

int main() {
    srand(time(NULL));

    char *tags_expense[9] = {"sode_bank", "sanp", "gym", "cafe", "market", "uni", "net", "food", "gf"};
    char *tag_e;
    char *tags_income[4] = {"asadiyan", "hamid", "abi", "other"};
    char *tag_i;

    double amount;

    int num_tag = 0;

    int sum = 700.00;
    int chg;

    int month;
    int day;

    FILE *file = fopen("journal.tsv", "a");
    if (file == NULL) {
        printf("Error opening file");
        return 1;

    }

    for(month = 1; month <= 12; month++) {
        for (day = 1; day <= 8; day++) {

            chg = rand() % 199 - 99;
            amount = (double)chg;
            if (amount > 0){
                printf("%d", sum);
                sum = sum + (amount * 10);
                printf("+%.2lf=%d\n", amount * 10, sum);
            }else if (amount < 0 ){
                printf("%d", sum);
                sum = sum + (amount * 10);
                printf("%.2lf=%d\n", amount * 10, sum);
                
                if (sum < 0 ){
                    sum = sum + 700;
                    double s = 700;
                    fprintf(file, "2023.%02d.%02d\t%.2lf\t%s\n", month, day, s, tags_income[0]);
                    printf("%d=\n", sum);
                }
            }  

            if(amount < 0){
                num_tag= rand() % (8 - 1 + 1) + 1;

                tag_e = tags_expense[num_tag];

                fprintf(file, "2023.%02d.%02d\t%.2lf\t%s\n", month, day, amount * 10, tag_e);    

            }else if(amount > 0){
                num_tag= rand() % (4 - 0 + 1) + 0;
    
                tag_i = tags_income[num_tag];

                fprintf(file, "2023.%02d.%02d\t%.2lf\t%s\n", month, day, amount * 10, tag_i);

            }else{
            }
        }
    }

    fclose(file);

    return 0;
}