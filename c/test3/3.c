#include<stdio.h>

int main(){

 int x=4;
   switch(x){
      default:
          printf("default");
          break;

      case 1:
      case 2:
      case 3:
          printf("low range");
         break;

     case 4:
        printf("Four");

    }

 return 0;
} 
