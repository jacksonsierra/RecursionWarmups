/*
 * CS 106X Recursion Problems
 * This file declares necessary function prototypes, partly so that other files
 * are able to call the recursive functions you will write.
 * You should not modify this file.
 * (If you want to declare your own helper function prototypes,
 *  do that near the top of your .cpp file, not here.)
 */

#ifndef _recursionproblems_h
#define _recursionproblems_h

#include <iostream>

#include "gbufferedimage.h"
#include "gwindow.h"
#include "vector.h"
#include "set.h"

#include "dominosa-graphics.h"
#include "marbletypes.h"

// colors for flood fill
#define COLOR_BLACK      0x000000
#define COLOR_DARK_GRAY  0x595959
#define COLOR_GRAY       0x999999
#define COLOR_LIGHT_GRAY 0xBFBFBF
#define COLOR_WHITE      0xFFFFFF
#define COLOR_RED        0xFF0000
#define COLOR_YELLOW     0xFFFF00
#define COLOR_GREEN      0x00FF00
#define COLOR_CYAN       0x00FFFF
#define COLOR_BLUE       0x0000FF
#define COLOR_MAGENTA    0xFF00FF
#define COLOR_ORANGE     0xFFC800
#define COLOR_PINK       0xFFAFAF

// function prototype declarations
// (If you want to declare your own helper function prototypes, do that near
//  the top of your .cpp file, not here.  You shouldn't modify this file.)

double weightOnKnees(int row, int col, Vector<Vector<double> >& weights);
void floodFill(GBufferedImage& image, int x, int y, int color);
bool solvePuzzle(Grid<MarbleType>& board, int marblesLeft, Set<uint32_t>& exploredBoards,
                 Vector<Move>& moveHistory);
bool canSolveBoard(DominosaDisplay& display, Grid<int>& board);

// provided helpers
int getPixelColor(int x, int y);
void setPixelColor(int x, int y, int color);

// testing function prototype declarations
void test_humanPyramid();
void test_floodFill();
void test_marbleBoard();
void test_dominosa();

#endif
