xint stock=0;
void addStock(int qty){
  stock=stock+qty;
}

void deleteStock(int qty){
    stock=stock-qty;
}

int getStock(){
    return stock;
}
