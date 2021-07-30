#include "fruit.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "fps.h"

int fruit_points(Fruit fruit)
{
	switch (fruit)
	{
		case Cherry:     return 100;
		case Strawberry: return 300;
		case Peach:      return 500;
		case Apple:      return 700;
		case Grapes:     return 1000;
		case Galaxian:   return 2000;
		case Bell:       return 3000;
		case Key:        return 5000;
	}

	printf("invalid fruit\naborting\n");
	exit(1);
}

Fruit fruit_for_level(int level)
{
	if (level < 1)
	{
		printf("invalid level: %d\n", level);
		printf("aborting\n");
		exit(1);
	}

	switch (level)
	{
		case 1:           return Cherry;
		case 2:           return Strawberry;
		case 3:  case 4:  return Peach;
		case 5:  case 6:  return Apple;
		case 7:  case 8:  return Grapes;
		case 9:  case 10: return Galaxian;
		case 11: case 12: return Bell;
		default:          return Key;
	}
}

void reset_fruit(GameFruit *gameFruit)
{
	gameFruit->fruit = Cherry;
	gameFruit->fruitMode = NotDisplaying;
	gameFruit->startedAt = 0;
	gameFruit->displayTime = 0;

	gameFruit->eaten = false;
	gameFruit->eatenAt = 0;

	gameFruit->x = 13;
	gameFruit->y = 17;
}

void regen_fruit(GameFruit *gameFruit, int level)
{
	gameFruit->fruit = fruit_for_level(level);
	gameFruit->displayTime = rand_fruit_visible_time();
	gameFruit->startedAt = ticks_game();
}

int rand_fruit_visible_time(void)
{
	// chosen by a random number generator
	// guarenteed to be random
	return 9500;
}
