#include "grid.h"
#include "dominosa-graphics.h"

#ifndef DOMINOSA_H
#define DOMINOSA_H

void welcome();
int getIntegerInRange(string prompt, int low, int high);
void populateBoard(Grid<int>& board, int low, int high);

#endif // DOMINOSA_H
