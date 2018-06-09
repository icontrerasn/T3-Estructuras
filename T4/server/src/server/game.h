#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct card{
  int color;
  int number;
  int available;
};
typedef struct card Card;

struct hand{
  Card* cards[5];
};
typedef struct hand Hand;

struct deck{
  Card* cards[52];
};
typedef struct deck Deck;

struct game{
  int socket[2];
  int player_1_pot;
  int player_2_pot;
  Deck* deck;
};
typedef struct game Game;

char int_to_byte(int number);

int byte_to_int(char byte[8]);

Card* random_card();

Hand* generate_hand(char cards[80]);

Deck* generate_deck();

void set_deck_available(Deck* deck);

Game* create_game();

void complete_hand(Hand* hand);

int check_royal_flush(Hand* hand);

int check_straight_flush(Hand* hand);

int check_poquer(Hand* hand);

int check_full(Hand* hand);

int check_color(Hand* hand);

int check_straight(Hand* hand);

int check_three(Hand* hand);

int check_double_two(Hand* hand);

int check_two(Hand* hands);

int view_points(Hand* hand);

int determine_winner(Hand* hand_1, Hand* hand_2);
