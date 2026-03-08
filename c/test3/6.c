#include<stdio.h>
int x,y;
 int add(){
    return x+y;
 }

 int sub(){
    return x-y;
 }

 int mul(){
    return x*y;
 }

 int div(){
    return x/y;
 }

int main(){
  printf("addition  is %d /n",add(2,3));
  printf("addition  is %d /n ",sub(3,5));
  printf("addition  is %d /n",mul(4,5));
  printf("addition  is %d /n ",div(5,7));

  return 0;
}
