#include<stdio.h>
int main(){
    int a=10;
    int *ptr=&a;
    int **ptr2=&ptr;
    
    printf("%u\n",**ptr2);
    printf("%u\n",*ptr);




    return 0;
}