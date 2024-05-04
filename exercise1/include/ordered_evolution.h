#ifndef ORDERED_EVOLUTION_H
#define ORDERED_EVOLUTION_H

void update_cell_ordered(unsigned char *top_adjacent_row, unsigned char *bottom_adjacent_row, unsigned char *local_playground, int xsize, int ysize, int x, int y);
void ordered_evolution(unsigned char *local_playground, int xsize, int my_chunk, int my_offset, int n, int s);

#endif // ORDERED_EVOLUTION_H