#include "game.h"

// char int_to_byte(int number){
//   char byte[8];
//   for (int i = 0; i < 8; i++){
//     //char buffer[1];
//     if (number % 2 == 1){
//       byte[i] = '1';
//     } else {
//       byte[i] = '0';
//     }
//     number = number / 2;
//   }
//   return *byte;
// }

int byte_to_int(char byte[8]){
  int number = 0;
  for (int i = 0; i < 8; i++){
    number += byte[i] * pow(2, 7 - i);
  }
  return number;
}

Card* random_card(){
  int card_number = (rand() % 13) + 1;
  int card_color = (rand() % 4) + 1;
  Card* new_card = malloc(sizeof(Card));
  new_card->number = card_number;
  new_card->color =  card_color;
  return new_card;
}

Hand* generate_hand(char cards[80]){
  Hand* new_hand = malloc(sizeof(Hand));
  for (int i = 0; i < 5; i++){
    new_hand->cards[i] = random_card();
  }
  return new_hand;
}

void complete_hand(Hand* hand){
  for (int i = 0; i < 5; i ++){
    if (!hand->cards[i]){
      hand->cards[i] = random_card();
    }
  }
}

void sort_hand(Hand* hand){
  for (int i = 0; i < 4; i++){
    Card* highest_card = malloc(sizeof(Card));
    highest_card->number = 0;
    int position;
    for (int j = i; j < 5; j++)
      if (hand->cards[j]->number > highest_card->number){
        highest_card = hand->cards[j];
        position = j;
      }
      if (position != i){
        hand->cards[position] = hand->cards[i];
        hand->cards[i] = highest_card;
      }
  }
}

int check_royal_flush(Hand* hand){
  sort_hand(hand);
  if (!check_color(hand)){
    return 0;
  }
  if (hand->cards[0]->number != 1){
    return 0;
  }
  for (int i = 1; i < 5; i++){
    if (hand->cards[i]->number != i + 9){
      return 0;
    }
  }
  return 1;
}

int check_straight_flush(Hand* hand){
  if (check_straight(hand) && check_color(hand)){
    return 1;
  }
  return 0;
}

int check_poquer(Hand* hand){
 for (int i = 0; i < 2; i++){
   int count = 0;
   for (int j = 0; j < 5; j++){
     if (hand->cards[i]->number == hand->cards[j]->number){
       count++;
     }
   }
   if (count == 4){
     return 1;
   }
 }
 return 0;
}

int check_full(Hand* hand){
  if (check_three(hand) && check_two(hand)){
    return 1;
  }
  return 0;
}

int check_color(Hand* hand){
  for (int i = 0; i < 4; i++){
    if (hand->cards[i]->color != hand->cards[i + 1]->color){
      return 0;
    }
  }
  return 1;
}

int check_straight(Hand* hand){
  sort_hand(hand);
  for (int i = 0; i < 4; i++){
    if (hand->cards[i + 1]->number - hand->cards[i]->number != 1){
      if (i != 1){
        return 0;
      } else {
        if (hand->cards[i + 1]->number - hand->cards[i]->number != 9){
          return 0;
        }
      }
    }
  }
  return 1;
}

int check_three(Hand* hand){
  for (int i = 0; i < 3; i++){
    int count = 0;
    for (int j = 0; j < 5; j++){
      if (hand->cards[i]->number == hand->cards[j]->number){
        count++;
      }
    }
    if (count == 3){
      return 1;
    }
  }
  return 0;
}

int check_double_two(Hand* hand){
  int pairs = 0;
  for (int i = 0; i < 4; i++){
    int count = 0;
    for (int j = 0; j < 5; j++){
      if (hand->cards[i]->number == hand->cards[j]->number){
        count++;
      }
    }
    if (count == 2){
      pairs++;
    }
  }
  if (pairs > 2){
    return 1;
  }
  return 0;
}

int check_two(Hand* hand){
  for (int i = 0; i < 4; i++){
    int count = 0;
    for (int j = 0; j < 5; j++){
      if (hand->cards[i]->number == hand->cards[j]->number){
        count++;
      }
    }
    if (count == 2){
      return 1;
    }
  }
  return 0;
}

int check_highest(Hand* hand){
  int highest = 0;
  for (int i = 0; i < 5; i++){
    if (hand->cards[i]->number > highest){
      highest = hand->cards[i]->number;
    }
  }
  return highest;
}
