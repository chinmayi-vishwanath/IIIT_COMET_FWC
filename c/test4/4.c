#include <stdio.h>
int getStock();
void addStock(int);

void purchase(int qty)
{
    if(getStock()+qty<= 100)
    {
        addStock(qty);
        printf("Purchase successful\n");
    }
    else
    {
        printf("Stock limit exceeded\n");
    }
}
