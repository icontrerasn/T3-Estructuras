#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utilities.h"

char* decimal_to_binary(int n){
   int b = 8;
   int d, count;
   char *pointer;
   count = 0;
   pointer = (char*)malloc(b+1);
   if (pointer == NULL)
      exit(EXIT_FAILURE);
   for (int c = b-1 ; c >= 0 ; c--){
      d = n >> c;
      if (d & 1){
         *(pointer+count) = 1 + '0';}
      else{
         *(pointer+count) = 0 + '0';}
      count++;
   }
   *(pointer+count) = '\0';
   return pointer;
}


int binary_to_decimal(int n, char* arr){
  int decimalNumber = 0;
  for (int i = 0; i < n; i++){
    int a = arr[i] - '0';
    int m = n-1-i;
    int b = pow(2, m);
    decimalNumber += a*b;
  }
  return decimalNumber;
}
