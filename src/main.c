#include <stdio.h>

const int MAX_JOURNAL_COUNT = 100;

typedef struct{
    double amount;
    char tag[100];
} Journal;

int input_journal(Journal *j, int count){
    printf("%d - Enter the amount:", count);
    scanf("%lf", &j->amount);
    if(j->amount != 0){
        printf("%d - Enter tag:", count);
        scanf("%s", &j->tag);
        return 1;
    }else{
        return 0;
    }
}

int save_journal(Journal *start, int count){
    FILE* f = fopen("journal.data", "wb");
    if (f != NULL){
        fwrite(start, sizeof(Journal), count, f);
        fclose(f);
    }

}

int read_journal(Journal *start){
    int count = 0;
    FILE* f = fopen("journal.data", "rb");
    if ( f!= NULL){
        while(fread(&start[count], sizeof(Journal), 1, f) > 0){
            count++;
        }
        fclose(f);
    }
    return count;
}

int main(){
    int keep_going = 1;
    Journal js[MAX_JOURNAL_COUNT];
    int count = read_journal(js);

    while(keep_going != 0){
        keep_going = input_journal(&js[count], count+1);
        count += keep_going;
        save_journal(&js[0], count);
    }
    return 0;
}
