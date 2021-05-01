#include <stdio.h>

int main()
{
   printf("calculator for +\n");
   printf("----------\n");

   int firstnumber  = 0;
   int secondnumber = 0;
   int result       = 0;

   printf("enter the first number: \n");
   scanf ("%d", &firstnumber);

   printf("enter the second number: \n");
   scanf ("%d", &secondnumber);

   result = firstnumber + secondnumber;

   printf("result: %d + %d = %d\n", firstnumber, secondnumber, result);

   return 0;
}