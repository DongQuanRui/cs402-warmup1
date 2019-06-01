#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "cs402.h"
#include "my402list.h"

int *storage;
int *backup_storage;

typedef struct transcation_info{
    char type[1];
    int time;
    double amount;
    char description[40];
}transcation;

void create_storage(int size){
    free(storage);
    backup_storage = (int *)malloc((size/2) * sizeof(int));
    for (int i=0; i<size/2; i++) {
        backup_storage[i] = storage[i];
    }
    storage = (int *)malloc(size * sizeof(int));
    for (int i=0; i<size/2; i++) {
        storage[i] = backup_storage[i];
    }
    free(backup_storage);
}

int search(int timestamp, int size){
    int rt = 0;
    for (int i=0; i<size; i++) {
        if (storage[i] == timestamp) {
            rt = 1;
        }
    }
    return rt;
}
////////////////////////////////////////////////////////////////////////////////////////
static
void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
/* (*pp_elem1) must be closer to First() than (*pp_elem2) */
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(pList, elem1);
    My402ListElem *elem2next=My402ListNext(pList, elem2);
    
    My402ListUnlink(pList, elem1);
    My402ListUnlink(pList, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(pList, obj2);
        *pp_elem1 = My402ListFirst(pList);
    } else {
        (void)My402ListInsertAfter(pList, obj2, elem1prev);
        *pp_elem1 = My402ListNext(pList, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(pList, obj1);
        *pp_elem2 = My402ListLast(pList);
    } else {
        (void)My402ListInsertBefore(pList, obj1, elem2next);
        *pp_elem2 = My402ListPrev(pList, elem2next);
    }
}

static
void BubbleSortForwardList(My402List *pList, int num_items)
{
    My402ListElem *elem=NULL;
    int i=0;
    
    if (My402ListLength(pList) != num_items) {
        fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", num_items);
        exit(1);
    }
    for (i=0; i < num_items; i++) {
        int j=0, something_swapped=FALSE;
        My402ListElem *next_elem=NULL;
        
        for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++) {
            int cur_val=((transcation *)(elem->obj))->time, next_val=0;
            
            next_elem=My402ListNext(pList, elem);
            next_val = ((transcation *)(next_elem->obj))->time;
            
            if (cur_val > next_val) {
                BubbleForward(pList, &elem, &next_elem);
                something_swapped = TRUE;
            }
        }
        if (!something_swapped) break;
    }
}

char *transform(char *str_dou){
    char temp_str_dou[20];
    memset(temp_str_dou, 0, 20);
    int dou_len = strlen(str_dou);
    int move_len = 0;
    if (dou_len >= 6) {
        move_len = 6;
        strncpy(temp_str_dou, str_dou, 20);
    }else{
        move_len = dou_len;
        strncpy(temp_str_dou, str_dou, 20);
    }
    
    while (move_len < dou_len) {
        for (int i=0; i<move_len; i++) {
            temp_str_dou[dou_len-i] = str_dou[dou_len-i-1];
        }
        temp_str_dou[dou_len-move_len] = ',';
        for (int i=0; i<dou_len-move_len; i++) {
            temp_str_dou[i] = str_dou[i];
        }
        move_len += 3;
    }
    strncpy(str_dou, temp_str_dou, strlen(temp_str_dou));
    str_dou[strlen(temp_str_dou)] = '\0';
    
    return str_dou;
}

void print_func(My402List *list, int line_processed){
    printf("+-----------------+--------------------------+----------------+----------------+\n");
    printf("|       Date      | Description              |         Amount |        Balance |\n");
    printf("+-----------------+--------------------------+----------------+----------------+\n");

    double balance = 0;
    My402ListElem *ret;
    ret = My402ListFirst(list);

    //time block
    time_t rawtime = (time_t)(((transcation *)(ret->obj))->time);
    char *buffer;
    buffer = ctime(&rawtime);
    char *space_ptr;

    space_ptr = strrchr(buffer, ' ');
    space_ptr++;
    for (int i=0; i<5; i++) {
        buffer[11+i] = space_ptr[i];
    }
    buffer[15] = '\0';

    printf("| %s |", buffer);


    //description block
    int description_len = strlen(((transcation *)(ret->obj))->description);
    if (description_len <= 24) {
        ((transcation *)(ret->obj))->description[description_len-1] = '\0';
        printf(" %s", ((transcation *)(ret->obj))->description);
        for (int i=0; i<26-description_len; i++) {
            printf(" ");
        }
        printf("|");
    }else{
        char temp[24];
        strncpy(temp, ((transcation *)(ret->obj))->description, 24);
        printf(" %s |", temp);
    }
    
    //amount transform
    //////////////////////start//////////////////////
    double dou = ((transcation *)(ret->obj))->amount;
    if (dou < 0) {
        dou = -dou;
    }
    
    char *pstr_dou;
    char str_dou[20];
    sprintf(str_dou, "%.2f", dou);
    
    pstr_dou = transform(str_dou);
    
    int amount_len = strlen(pstr_dou);
    
    balance = ((transcation *)(ret->obj))->amount;
    ////////////////////// end //////////////////////
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //need to see deposit or withdrawal
    if (((transcation *)(ret->obj))->amount > 0) {
        //print amount
        for (int i=0; i<14-amount_len; i++) {
            printf(" ");
        }
        printf("%s  |", pstr_dou);

        //print balance
        for (int i=0; i<14-amount_len; i++) {
            printf(" ");
        }
        printf("%s  |\n", pstr_dou);
        
    }else{
        //print amount
        printf(" (");
        for (int i=0; i<12-amount_len; i++) {
            printf(" ");
        }
        printf("%s) |", pstr_dou);

        printf(" (");
        for (int i=0; i<12-amount_len; i++) {
            printf(" ");
        }
        printf("%s) |\n", pstr_dou);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    //print the rest of lines
    for (int j=0; j<line_processed-1; j++) {

        ret = My402ListNext(list, ret);

        //////////////////////////  time block  //////////////////////////
        time_t rawtime = (time_t)(((transcation *)(ret->obj))->time);
        char *buffer;
        buffer = ctime(&rawtime);
        char *space_ptr;

        space_ptr = strrchr(buffer, ' ');
        space_ptr++;
        for (int i=0; i<5; i++) {
            buffer[11+i] = space_ptr[i];
        }
        buffer[15] = '\0';

        printf("| %s |", buffer);
        //////////////////////////end of time block//////////////////////////
        
        //description block
        int description_len = strlen(((transcation *)(ret->obj))->description);

        //the length of description is less than 24, then print out
        if (description_len <= 24) {
            ((transcation *)(ret->obj))->description[description_len-1] = '\0';
            printf(" %s", ((transcation *)(ret->obj))->description);
            for (int i=0; i<26-description_len; i++) {
                printf(" ");
            }
            printf("|");
        }else{
            char temp[24];
            strncpy(temp, ((transcation *)(ret->obj))->description, 24);
            printf(" %s |", temp);
        }

        //amount transform
        //////////////////////start//////////////////////
        double dou = ((transcation *)(ret->obj))->amount;
        if (dou < 0) {
            dou = -dou;
        }
        char *pstr_dou;
        char str_dou[20];
        sprintf(str_dou, "%.2f", dou);
        
        pstr_dou = transform(str_dou);
        
        int amount_len = strlen(pstr_dou);
        ////////////////////// end //////////////////////
        
        //balance transform
        balance += ((transcation *)(ret->obj))->amount;
        //////////////////////start//////////////////////
        double bal_dou = 0;
        if (balance < 0) {
            bal_dou = -balance;
        }else{
            bal_dou = balance;
        }
        
        char *pbal_str_dou;
        char bal_str_dou[20];
        sprintf(bal_str_dou, "%.2f", bal_dou);
        
        pbal_str_dou = transform(bal_str_dou);
        
        int bal_len = strlen(pbal_str_dou);
        ////////////////////// end //////////////////////
        
        
        //print amount and balance
        if (((transcation *)(ret->obj))->amount > 0) {
            for (int i=0; i<14-amount_len; i++) {
                printf(" ");
            }
            printf("%s  |", pstr_dou);
            
            if (balance < -10000000){
                fprintf(stderr, "Error: the absolute value of balance exceeds 10,000,000");
                exit(1);
            }else if(balance > 10000000) {
                fprintf(stderr, "Error: the absolute value of balance exceeds 10,000,000");
                exit(1);
            }else if (balance < 0) {
                printf(" (");
                for (int i=0; i<12-bal_len; i++) {
                    printf(" ");
                }
                printf("%s) |\n", pbal_str_dou);
            }else{
                for (int i=0; i<14-bal_len; i++) {
                    printf(" ");
                }
                printf("%s  |\n", pbal_str_dou);
            }
        }else{
            printf(" (");
            for (int i=0; i<12-amount_len; i++) {
                printf(" ");
            }
            printf("%s) |", pstr_dou);
            
            if (balance < -10000000){
                fprintf(stderr, "Error: the absolute value of balance exceeds 10,000,000");
                exit(1);
            }else if(balance > 10000000) {
                fprintf(stderr, "Error: the absolute value of balance exceeds 10,000,000");
                exit(1);
            }else if (balance < 0) {
                printf(" (");
                for (int i=0; i<12-bal_len; i++) {
                    printf(" ");
                }
                printf("%s) |\n", pbal_str_dou);
            }else{
                for (int i=0; i<14-bal_len; i++) {
                    printf(" ");
                }
                printf("%s  |\n", pbal_str_dou);
            }
        }
    }
    printf("+-----------------+--------------------------+----------------+----------------+\n");
}

////////////////////////////////////////////////////////////////////////////////////////
void file_process(char *file_name, int open_type){
    FILE *fp;
    if (open_type == 1) {
        fp = stdin;
    }else if(open_type == 2){
        fp = fopen(file_name, "r");
    }

    char stream[1026];
    int t = 0;
    double money = 0.0;
    char type[3];
    
    int size = 20;
    storage = (int *)malloc(size * sizeof(int));
    int line_processed = 0;
    
    My402List list;
    memset(&list, 0, sizeof(My402List));
    (void)My402ListInit(&list);
    
    while (!feof(fp)) {
        memset(stream, 0, sizeof(stream));
        if (fgets(stream, sizeof(stream), fp) == NULL) {
            break;
        }
        
        if (strlen(stream) >= 1024) {
            fprintf(stderr, "Error: The length of line exceed limit!\n");
            exit(1);
        }
        
        transcation *info = (transcation *)malloc(sizeof(transcation));
        memset(info, 0, sizeof(transcation));
        
        //1st iteration
        char *start_ptr = stream;
        char *tab_ptr = strchr(start_ptr, '\t');
        if (tab_ptr != NULL) {
            *tab_ptr++ = '\0';
        }
        strncpy(type, start_ptr, 1);
        strncpy(info->type, type, 1);
        
        //1st print out error
        if (*type != 43 && *type != 45) {
            fprintf(stderr, "Error: Transcation type error\n");
            exit(1);
        }
        
        //2nd iteration
        start_ptr = tab_ptr;
        tab_ptr = strchr(start_ptr, '\t');
        if (tab_ptr != NULL) {
            *tab_ptr++ = '\0';
        }
        //timestamp
        t = atoi(start_ptr);
        //
        info->time = t;
        //
        time_t date = (time_t)t;
        
        time_t now;
        time(&now);
        int int_now = (int)now;
        
        if(search(t, size)){
            fprintf(stderr, "Error: Repeated timestamp found!\n");
            exit(1);
        }
        
        if (t > int_now || t < 0) {
            fprintf(stderr, "Error: Timestamp error!\n");
            exit(1);
        }else{
            //search in storage
            if (line_processed < size) {
                storage[line_processed] = t;
                line_processed++;
            }else{
                size = size + 20;
                create_storage(size);
                storage[line_processed] = t;
                line_processed++;
            }
            
            char *temp_time = ctime(&date);
            int str_len = strlen(temp_time);
            temp_time[str_len-1] = '\0';
        }
        
        //3rd iteration
        start_ptr = tab_ptr;
        tab_ptr = strchr(start_ptr, '\t');
        if(tab_ptr == NULL){
            fprintf(stderr, "Error: Info missing\n");
            exit(1);
        }
        if (tab_ptr != NULL) {
            *tab_ptr++ = '\0';
        }
        //see if money amount error
        char *temp_ptr = start_ptr;
        char *p_ptr = strchr(start_ptr, '.');
        start_ptr = ++p_ptr;
        if (strlen(start_ptr)>2) {
            fprintf(stderr, "Error: Money amount error!\n");
            exit(1);
        }else{
            start_ptr = temp_ptr;
            money = atof(start_ptr);
            if (money >= 10000000 || money <= -10000000) {
                fprintf(stderr, "Error: Money amount error!\n");
            }
            //
            if (!strcmp(info->type, "+")) {
                info->amount = money;
            }else{
                info->amount = -money;
            }
        }
        
        //last iteration
        start_ptr = tab_ptr;
        tab_ptr = strchr(start_ptr, '\0');
        if(tab_ptr == NULL){
            fprintf(stderr, "Error: Info format error\n");
            exit(1);
        }
        strncpy(info->description, start_ptr, sizeof(info->description));
        tab_ptr = strchr(start_ptr, '\t');
        if (tab_ptr != NULL) {
            fprintf(stderr, "Error: Too many fields\n");
            exit(1);
        }
        
        //append this structure to linked list
        My402ListAppend(&list, info);
        //keep this memory
        info = NULL;
        //and then free the pointer
        free(info);
    }
    free(storage);
    fclose(fp);
    
    //bubblesort in ascending orders
    BubbleSortForwardList(&list, line_processed);
    
    //call print out function
    print_func(&list, line_processed);
}

int main(int argc, char *argv[])
{
    if(argc == 2){
        file_process(argv[1], 1);
    }else if(argc == 3){
        file_process(argv[2], 2);
    }
    return 0;
}

