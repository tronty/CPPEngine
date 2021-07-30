#include "pellet.h"

void pellets_init(PelletHolder *pelletHolder)
{
	for (int i = 0; i < NUM_PELLETS; i++)
	{
		pelletHolder->pellets[i].eaten = false;
	}

	pelletHolder->numLeft = NUM_PELLETS;
	pelletHolder->totalNum = NUM_PELLETS;
}

int pellet_points(Pellet *pellet)
{
	switch (pellet->type)
	{
		case SmallPellet: return 10;
		case LargePellet: return 50;
	}

	printf("error\n");
	exit(1);
}

int pellet_nop_frames(Pellet *pellet)
{
	switch (pellet->type)
	{
		case SmallPellet: return 1;
		case LargePellet: return 3;
	}

	printf("error\n");
	exit(1);
}
