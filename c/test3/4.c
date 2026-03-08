// craete operand as global variable

#include<stdio.h>
 int  x=10, y=2;

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
  printf("addition  is %d /n",add());
  printf("addition  is %d /n ",sub());
  printf("addition  is %d /n",mul());
  printf("addition  is %d /n ",div());

  return 0;
}








