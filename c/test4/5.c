#include <stdio.h>
void sell(int);
void purchase(int);
int getStock();

int main()
{
    purchase(50);
    sell(20);
    printf("Available Stock = %d\n", getStock());
    return 0;
}
