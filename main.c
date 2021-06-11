//#include "api.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
typedef struct mail{
    int id;
    char from[32];
    char to[32];
    char subject[256];
    char content[100000];
}mail;

typedef struct query{
    int id;
    double reward;
    enum query_type{ expression_match, find_similar, group_analyse }type;
    union query_data{
        struct{ char expression[2048]; } expression_match_data;
        struct{ int mid ; double threshold; } find_similar_data;
        struct{ int len; int mids[512]; } groups_analyse_data;
    } data;
} query;

int n_mails, n_queries;
mail *mails;
query *queries;

//void api.init(int *n_mails, int *n_queries, mail **mails, query **queries);
//void api.answer(int query_id, int answer[], int answer_length);

bool alpha_numeric(char c){
    if ((c >= '0') && (c <= '9')) return true;
    if ((c >= 'a') && (c <= 'z')) return true;
    if ((c >= 'A') && (c <= 'Z')) return true;
    return false;
}

int transform(char c){
    if ((c >= '0') && (c <= '9')) return c;
    if ((c >= 'a') && (c <= 'z')) return c;
    if ((c >= 'A') && (c <= 'Z')) return c+32;
    return 0;
}

void complete_hash (char content[100000], long long int hash_array[100000], int *len){
    long long int hash = 0;
    for (int i=0 ; i<100000 ; i++){
        if (alpha_numeric(content[i])){
            hash *= 128;
            hash = (hash + transform(content[i])) % 2021060687879487;
        }else{
            bool repeated = false;
            for (int j=0 ; j<*len ; j++){
                if (hash == hash_array[j]) repeated = true;
            }
            if ((!repeated) && (hash != 0)){
                hash_array[*len] = hash;
                *len = *len + 1;
            }
            hash = 0;
            if (content[i] == 0) break;
        }
    }
    return;
}

double ratio(long long int hash_array1[100000], long long int hash_array2[100000], int len1, int len2){
    double same = 0;
    for (int i=0 ; i<len1 ; i++){
        for (int j=0 ; j<len2 ; j++){
            if (hash_array1[i] == hash_array2[j]) same++ ;
        }
    }
    return same / (len1 + len2 - same);
}

void findsimilar(query q, mail mails[], int n_mails, int answer[], int *answer_length){
    double threshold = q.data.find_similar_data.threshold;
    int mid = q.data.find_similar_data.mid;
    for (int i=0 ; i<n_mails ; i++){
        if (i != mid){
            int len1 = 0, len2 = 0;
            long long int hash_array1[100000] = {0}, hash_array2[100000] = {0};
            complete_hash (mails[i].content,hash_array1,&len1);
            complete_hash (mails[mid].content,hash_array2,&len2);
            if (ratio(hash_array1,hash_array2,len1,len2) > threshold){
                answer[*answer_length] = i;
                *answer_length = *answer_length + 1;
            }
        }
    }
    return;
}

void test(){
    char a[100000] = {0}, b[100000] = {0};
    strcpy(a, "I am smart.is \\\stupid");
    strcpy(b, "i is smArt is stu\\\pid");
    int len1 = 0, len2 = 0;
    long long int hash_array1[100000] = {0}, hash_array2[100000] = {0};
    complete_hash (a,hash_array1,&len1);
    complete_hash (b,hash_array2,&len2);
    for (int i=0 ; i<len1 ; i++){
        printf ("%lld ",hash_array1[i]);
    }
    printf ("\n");
    for (int i=0 ; i<len2 ; i++){
        printf ("%lld ",hash_array2[i]);
    }
    printf ("\n");
    printf ("%lf\n", ratio(hash_array1,hash_array2,len1,len2));
}

int main(){
    //api.init(&n_mails, &n_queries, &mails, &queries);
    test();
    for (int i=0 ; i<n_queries ; i++){
        if (queries[i].type == expression_match){
            int answer[10];
            //api.answer(i, a, 10);
        }else if (queries[i].type == find_similar){
            int answer[n_mails];
            int answer_length = 0;
            findsimilar(queries[i],mails,n_mails,answer,&answer_length);
            //api.answer(i, answer, answer_length);
        }else{
            int answer[2];
            //api.answer(i, answer, 2);
        }
    }
}
