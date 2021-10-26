#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH 33
#define BOARD_HEIGHT 16
#define BOARD_SPACES (BOARD_WIDTH * BOARD_HEIGHT)
#define NUM_MINES 70

void place_random_mine(uint32_t mines[]);
void draw_board(uint32_t view[], uint32_t mines[], uint8_t draw_mines);
uint32_t input_guess();
void floodfill(uint32_t index, uint32_t view[], uint32_t mines[]);
uint8_t count_nearby_mines(uint32_t index, uint32_t mines[]);

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	const uint32_t fields = BOARD_SPACES / 32 + (BOARD_SPACES % 32 > 0);
	uint32_t mines[fields], view[fields];
	for (int i = 0; i < fields; ++i)
		mines[i] = view[i] = 0;
	for (int i = 1; i < NUM_MINES; ++i)
		place_random_mine(mines);
	
	int dead = 0;
	while (!dead)
	{
		draw_board(view, mines, 0);
		floodfill(input_guess(), view, mines);
		
		
		for (int i = 0; i < fields; ++i)
			if (mines[i] & view[i])
			{
				printf("\nYou ran into a mine.\n");
				dead = 1;
				break;
			}
	}

	draw_board(view, mines, 1);
	return 0;
}

void place_random_mine(uint32_t mines[])
{
	uint32_t index;
	do {
		index = rand() % BOARD_SPACES;
	} while (mines[index / 32] & (1 << (index % 32)));
	mines[index / 32] |= (1 << (index % 32));
}

void draw_board(uint32_t view[], uint32_t mines[], uint8_t draw_mines)
{
	char row[BOARD_WIDTH + 1];
	row[BOARD_WIDTH] = '\0';

	printf("   ");
	for (uint32_t i = 0; i < (BOARD_WIDTH / 10) + 1; ++i)
		printf("%-10.0u", i);
	printf("\n   ");	
	for (uint32_t i = 0; i < BOARD_WIDTH; ++i)
		printf("%u", i % 10);
	printf("\n   ");
	for (uint32_t i = 0; i < BOARD_WIDTH; ++i)
		printf("-");
	printf("\n");

	uint32_t index = 0;
	for (uint32_t y = 0; y < BOARD_HEIGHT; ++y)
	{
		for (uint32_t x = 0; x < BOARD_WIDTH; ++x)
		{
			if (draw_mines && mines[index / 32] & (1 << (index % 32)))
				row[x] = '*';
			else if (view[index / 32] & (1 << (index % 32)))
			{
				uint8_t c = count_nearby_mines(index, mines);
				row[x] = (c > 0) ? ('0' + c) : ('-');
			}
			else
				row[x] = '.';

			++index;
		}
		printf("%2u|%s\n", y, row);
	}
}

uint32_t input_guess()
{
	char str[10];
	uint32_t x, y;
	
	do {
		printf("Guess a location.\nx: ");
		gets(str);
		x = atoi(str);
		printf("y: ");
		gets(str);
		y = atoi(str);
	} while (x < 0 && x >= BOARD_WIDTH && y < 0 && y >= BOARD_HEIGHT);

	return x + y * BOARD_WIDTH;
}

uint8_t count_nearby_mines(uint32_t index, uint32_t mines[])
{
	uint8_t count = 0;
	for (int y = -1; y <= 1; ++y)
		for (int x = -1; x <= 1; ++x)
		{
			uint32_t i = index + x + y * BOARD_WIDTH;
			if ((i % BOARD_WIDTH) - (index % BOARD_WIDTH) == x)
				if (i >= 0 && i < BOARD_SPACES)
					count += (mines[i / 32] & (1 << (i % 32))) != 0;
		}
	return count;
}

void floodfill(uint32_t index, uint32_t view[], uint32_t mines[])
{
	if (index >= 0 && index < BOARD_SPACES)
		if (~view[index / 32] & (1 << (index % 32)))
		{
			view[index / 32] |= (1 << (index % 32));
			if (count_nearby_mines(index, mines) == 0)
				for (int y = -1; y <= 1; ++y)
					for (int x = -1; x <= 1; ++x)
					{
						uint32_t i = index + x + y * BOARD_WIDTH;
						if ((i % BOARD_WIDTH) - (index % BOARD_WIDTH) == x)
							if (i >= 0 && i < BOARD_SPACES)
								floodfill(i, view, mines);
					}
		}
}