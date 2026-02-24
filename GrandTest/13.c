
#include <stdio.h>

int main() {
    int num=13;
    printf("Enter number: ");

    if((num >> 31) & 1)
        printf("Negative");
    else
        printf("Positive");

    return 0;
}
