#include <stdio.h>
int getStock();
void deleteStock(int);

void sell(int qty)
{
    if(getStock()>=qty)
    {
        deleteStock(qty);
        printf("Sale successful\n");
    }
    else
    {
       printf("not succesfully\n");
    }
}
