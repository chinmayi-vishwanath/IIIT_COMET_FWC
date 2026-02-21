#include<Arduino.h>
int A,B,C;
int a,b,c,d,e,f,g;
int F;
void display(int a, int b, int c, int d, int e, int f, int g)
{
	digitalWrite(5,a);
	digitalWrite(6,b);
	digitalWrite(7,c);
	digitalWrite(8,d);
	digitalWrite(9,e);
	digitalWrite(10,f);
	digitalWrite(11,g);
}
void setup()
{
        pinMode(2,INPUT);
        pinMode(3,INPUT);
        pinMode(4,INPUT);
        pinMode(5,OUTPUT);
        pinMode(6,OUTPUT);
        pinMode(7,OUTPUT);
        pinMode(8,OUTPUT);
        pinMode(9,OUTPUT);
        pinMode(10,OUTPUT);
        pinMode(11,OUTPUT);
}
void loop()
{

	A=digitalRead(2); //msb //inputs of expression in quetion ABC
	B=digitalRead(3);
        C=digitalRead(4);//lsb
        //the expression of F from truth table
        //F = (!A&&!B&&C)||(!A&&B&&C)||(A&&!B&&C)||(A&&B&&C);
        //the minimized expression of F using kmap is
        F=C;
        if(F==1)
        {
		display(1,0,0,1,1,1,1); //displays 1 on common anode 7seg
        }
        else
		display(0,0,0,0,0,0,1); //displays 0 on 7seg
}
