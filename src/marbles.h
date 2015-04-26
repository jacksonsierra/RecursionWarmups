#include "grid.h"
#include "set.h"

#include "marblegraphics.h"
#include "marbletypes.h"

#ifndef MARBLES_H
#define MARBLES_H

int humanPlay(Grid<MarbleType>& board, int marblesRemaining, MarbleGraphics& mg);
void computerPlay(Grid<MarbleType>& board, int marblesRemaining, MarbleGraphics& mg);

int initializeBoard(Grid<MarbleType>& board);
int readBoardFromFile(Grid<MarbleType>& board, ifstream& file);
int setUpDefaultBoard(Grid<MarbleType>& board);

void makeMove(Move move, Grid<MarbleType>& board);
void undoMove(Move move, Grid<MarbleType>& board);
bool isValidMove(Move move, const Grid<MarbleType>& board);

bool solvePuzzle(Grid<MarbleType>& board, int marblesLeft, Set<uint32_t>& exploredBoards, Vector<Move>& moveHistory);

static const int kPauseDuration = 30;
static const int kNumMarblesStart = 32;

#endif // MARBLES_H
