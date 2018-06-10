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

Card* random_card(Deck* deck){
  while (1){
    int random = (rand() % 51);
    Card* random_card = deck->cards[random];
    if (random_card->available){
      random_card->available = 0;
      return random_card;
    }
  }
}

Hand* generate_hand(Deck* deck){
  Hand* new_hand = malloc(sizeof(Hand));
  for (int i = 0; i < 5; i++){
    new_hand->cards[i] = random_card(deck);
  }
  return new_hand;
}

Deck* generate_deck(){
  Deck* deck = malloc(sizeof(Deck));
  int card_number = 0;
  for (int i = 1; i < 13; i++){
    for (int j = 1; j < 5; j++){
      Card* card = malloc(sizeof(Card));
      card->number = i;
      card->color = j;
      card->available = 1;
      deck->cards[card_number] = card;
      card_number++;
    }
  }
  return deck;
}

void set_deck_available(Deck* deck){
  for (int i = 0; i < 52; i++){
    deck->cards[i]->available = 1;
  }
}

Game* create_game(){
  Game* game = malloc(sizeof(Game));
  game->socket[0] = -1;
  game->socket[1] = -1;
  game->player_1_pot = 1000;
  game->player_2_pot = 1000;
  game->deck = generate_deck();
  return game;
}

void complete_hand(Hand* hand, Deck* deck){
  for (int i = 0; i < 5; i ++){
    if (!hand->cards[i]){
      hand->cards[i] = random_card(deck);
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
  return 22;
}

int check_straight_flush(Hand* hand){
  if (check_straight(hand) && check_color(hand)){
    return 1;
  }
  return 21;
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
     return 20;
   }
 }
 return 0;
}

int check_full(Hand* hand){
  if (check_three(hand) && check_two(hand)){
    return 19;
  }
  return 0;
}

int check_color(Hand* hand){
  for (int i = 0; i < 4; i++){
    if (hand->cards[i]->color != hand->cards[i + 1]->color){
      return 0;
    }
  }
  return 18;
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
  return 17;
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
      return 16;
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
    return 15;
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
      return 14;
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

int view_points(Hand* hand){
  int points;
  if (check_royal_flush(hand)){
    points = 22;
  } else if (check_straight_flush(hand)){
    points = 21;
  } else if (check_poquer(hand)){
    points = 20;
  } else if (check_full(hand)){
    points = 19;
  } else if (check_color(hand)){
    points = 18;
  } else if (check_straight(hand)){
    points = 17;
  } else if (check_three(hand)){
    points = 16;
  } else if (check_double_two(hand)){
    points = 15;
  } else if (check_two(hand)){
    points = 14;
  } else {
    points = check_highest(hand);
  }
  return points;
}

int determine_winner(Hand* hand_1, Hand* hand_2){
  int points_player_1 = view_points(hand_1);
  int points_player_2 = view_points(hand_2);
  if (points_player_1 > points_player_2){
    return 1;
  } else if (points_player_2 > points_player_1){
    return 2;
  } else if (points_player_1 == points_player_2){
    return 0;
  }
  return -1;
}
