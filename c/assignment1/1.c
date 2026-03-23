#include <stdio.h>
#include <stdlib.h>

// 1) Restrict stock variable to stock.c file using 'static'
static int stock = 50; 
static int capacity = 100;

// Tracking variables for the final report
static int num_sales = 0;
static int items_sold = 0;
static int num_purchases = 0;
static int items_purchased = 0;
static int customers_visited = 0;

void handle_sales() {
    int requested, choice;
    customers_visited++;

    printf("\nEnter number of items to sell: ");
    scanf("%d", &requested);

    // 2) Sales logic with sub-options
    if (stock < requested) {
        printf("Insufficient stock! Available: %d\n", stock);
        printf("1) Sell available stock\n2) Cancel the sales\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            items_sold += stock;
            stock = 0;
            num_sales++;
            printf("Sold remaining items.\n");
        } else {
            printf("Sales cancelled.\n");
        }
    } else {
        stock -= requested;
        items_sold += requested;
        num_sales++;
        printf("Sales successful!\n");
    }
}

void handle_purchase() {
    int requested, choice;
    int space = capacity - stock;

    printf("\nEnter number of items to purchase: ");
    scanf("%d", &requested);

    // 3) Purchase logic with sub-options
    if (space < requested) {
        printf("Insufficient space! Remaining space: %d\n", space);
        printf("1) Purchase items for remaining space\n2) Cancel the purchase\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            items_purchased += space;
            stock += space;
            num_purchases++;
            printf("Purchased %d items to fill capacity.\n", space);
        } else {
            printf("Purchase cancelled.\n");
        }
    } else {
        stock += requested;
        items_purchased += requested;
        num_purchases++;
        printf("Purchase successful!\n");
    }
}

void print_report() {
    // 4) Final report requirement
    printf("\n REPORT\n");
    printf("1) Closing stock: %d\n", stock);
    printf("2) No of sales: %d\n", num_sales);
    printf("3) No of items sold: %d\n", items_sold);
    printf("4) No of purchases: %d\n", num_purchases);
    printf("5) No of items purchased: %d\n", items_purchased);
    printf("6) No of customers visited: %d\n", customers_visited);
}

int main() {
    int option;

    while (1) {
        printf("\n--- Inventory Menu ---\n");
        printf("1. Sales\n2. Purchase\n3. Check Stock\n4. Quit\nSelection: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                handle_sales();
                break;
            case 2:
                handle_purchase();
                break;
            case 3:
                printf("Current Stock Level: %d / %d\n", stock, capacity);
                break;
            case 4:
                print_report();
                exit(0);
            default:
                printf("Invalid selection. Try again.\n");
        }
    }
    return 0;
}
