#include<Arduino.h>
void sevenseg(int a,int b,int c,int d,int 

e,int f,int g)
{
  digitalWrite(2, a);  
  digitalWrite(3, b);  
  digitalWrite(4, c);  
  digitalWrite(5, d);  
  digitalWrite(6, e);  
  digitalWrite(7, f);  
  digitalWrite(8, g);  
}

void setup()
{
  for(int i=2; i<=8; i++)
  {
    pinMode(i, OUTPUT);
  }
}

void loop()
{
  // 1 → AND = NAND
  sevenseg(1,0,0,1,1,1,1);
  delay(1000);

  // 2 → OR = XOR
  sevenseg(0,0,1,0,0,1,0);
  delay(1000);

  // 3 → NOT = XNOR
  sevenseg(0,0,0,0,1,1,0);
  delay(1000);
}
