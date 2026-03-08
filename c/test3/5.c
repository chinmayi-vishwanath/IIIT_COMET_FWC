#include<stdio.h>
 int  x=10, y=2;
 int result;


 int add(){
    result= x+y;
 }

 int sub(){
    result=x-y;
 }

 int mul(){
    result=x*y;
 }

 int div(){
    result=x/y;
 }

int main(){
  add();
  printf("addition  is %d ",result);

  sub();
  printf("subtraction  is %d ",result);

  mul();
  printf("mul is %d",result);

  div();
  printf("division   is %d ",result);
  return 0;
}
