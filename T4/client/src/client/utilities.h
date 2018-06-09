#pragma once

char* decimal_to_binary(int n);
int binary_to_decimal(int n, char* arr);

typedef struct tupla Tupla;

struct tupla{
  int message_id;
  int* payload;
};
