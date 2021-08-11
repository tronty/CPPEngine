#include "boardloader.h"

#include "imageloader.h"

void load_board(Board *board, PelletHolder *pelletHolder, const char* file)
{
	FILE *fp = fopen(file, "r");

	if (fp == NULL)
	{
		printf("File %s cannot be found.\n", file);
		printf("Aborting.\n");
		exit(1);
	}

	char c = 'a';
	int x = 0;
	int y = 0;

	int pelletI = 0;

	bool walkable = false;

	while (((c = fgetc(fp)) != EOF))
	{
		if (c == '\n' || c == 13) continue; //continue if a newline or carriage-return

		//	NOTE:
		//
		// Some of these characters look identical (═ and =, for example).
		// They are different, and used to distinguish between different orientations of the same tile on the board.
		// Be wary of this when editing this code.

		SDL_Surface *image = NULL;

		switch (c)
		{
			case '0': image = double_corner_image(BottomLeft);  break;
			case '1': image = double_corner_image(BottomRight); break;
			case '2': image = double_corner_image(TopLeft);     break;
			case '3': image = double_corner_image(TopRight);    break;

			case '4': image = single_corner_image(BottomLeft);  break;
			case '5': image = single_corner_image(BottomRight); break;
			case '6': image = single_corner_image(TopLeft);     break;
			case '7': image = single_corner_image(TopRight);    break;

			case '8': image = tleft_image(TopRight);            break;
			case '9': image = tleft_image(TopLeft);             break;
			case 'a': image = tleft_image(BottomLeft);          break;
			case 'b': image = tleft_image(BottomRight);         break;

			case 'c': image = tright_image(TopLeft);            break;
			case 'd': image = tright_image(BottomRight);        break;
			case 'e': image = tright_image(BottomLeft);         break;
			case 'f': image = tright_image(TopRight);           break;

			case 'g': image = hallway_image(Up);                break;
			case 'h': image = hallway_image(Down);              break;
			case 'i': image = hallway_image(Left);              break;
			case 'j': image = hallway_image(Right);             break;

			case 'k': image = middle_image(Left);               break;
			case 'l': image = middle_image(Right);              break;
			case 'm': image = middle_image(Up);                 break;
			case 'n': image = middle_image(Down);               break;

			case 'o': image = pen_corner_image(TopLeft);        break;
			case 'p': image = pen_corner_image(TopRight);       break;
			case 'q': image = pen_corner_image(BottomLeft);     break;
			case 'r': image = pen_corner_image(BottomRight);    break;

			case 's': image = pen_side_image(Up);               break;
			case 't': image = pen_side_image(Down);             break;
			case 'u': image = pen_side_image(Left);             break;
			case 'v': image = pen_side_image(Right);            break;

			case 'w': image = pen_gate_image();                 break;

			case '.': /* pellet */
				pelletHolder->pellets[pelletI].image = small_pellet_image();
				pelletHolder->pellets[pelletI].x = x;
				pelletHolder->pellets[pelletI].y = y;
				pelletHolder->pellets[pelletI].eaten = false;
				pelletHolder->pellets[pelletI].type = SmallPellet;
				pelletI++;

				walkable = true;
				break;
			case '*': /* large pellet */
				pelletHolder->pellets[pelletI].image = large_pellet_image();
				pelletHolder->pellets[pelletI].x = x;
				pelletHolder->pellets[pelletI].y = y;
				pelletHolder->pellets[pelletI].eaten = false;
				pelletHolder->pellets[pelletI].type = LargePellet;
				pelletI++;

				walkable = true;
				break;
			case ',': // empty, walkable square
				walkable = true;
				break;
			case ' ': //empty, nonwalkable square
				walkable = false;
				break;
			default:
				printf("Error loading character: %c (int: %d) at coordinate (%d, %d) in boardfile %s\n", c, c, x, y, file);
				printf("aborting\n");
				exit(1);
				break;
		}

		board->boardSquares[x][y].image = image;
		board->boardSquares[x][y].walkable = walkable;

		image = NULL;
		walkable = false;
		x++;

		if (x == BOARD_LENGTH)
		{
			x = 0;
			y++;
		}
	}

	fclose(fp);
}
