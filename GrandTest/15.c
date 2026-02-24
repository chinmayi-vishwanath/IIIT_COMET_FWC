#include <stdio.h>

int main() {
    char ch;
    int i;

    printf("Enter character: ");
    scanf("%c", &ch);

    printf("Decimal: %d\n", ch);
    printf("Hex: %x\n", ch);

    printf("Binary: ");
    for(i = 7; i >= 0; i--) {
        printf("%d", (ch >> i) & 1);
    }

return 0;
}
