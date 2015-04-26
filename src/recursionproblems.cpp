#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>

#include "gwindow.h"
#include "hashmap.h"
#include "hashset.h"
#include "map.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "set.h"
#include "grid.h"

#include "recursionproblems.h"
#include "marbletypes.h"
#include "compression.h"
#include "marbles.h"

using namespace std;

//Prototypes
double weightOnKnees(int row, int col, Vector<Vector<double> >& weights, Grid<double>& weightsSupported);
void floodFill(GBufferedImage& image, int x, int y, int color, int preColor);
Vector<Move> findPossibleMoves(Grid<MarbleType>& board);
void checkMarbleNeighbors(Grid<MarbleType>& board, Vector<Move>& moveList, int startRow, int startCol, int rowOffset, int colOffset);
void determinePossibleDominoes(const Grid<int>& board, Vector< Vector<coord> >& possibleDominoes);
bool canSolveBoard(DominosaDisplay& display, Grid<int>& board, Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, coord& currentSpot);
Vector< Vector<coord> > getProvisionalDominoes(const Grid<int>& board, const coord& currentSpot);
Vector<coord> createDomino(int row1, int col1, int row2, int col2);
bool isSpotEmpty(const HashSet< Vector<int> >& occupiedSpots, const coord currentSpot);
Vector<int> convertCoordToVector(const coord c);
bool isNumberPairFree(const Grid<int>& board, const Vector< Vector<coord> >& currentDominoes, const Vector<coord>& provisionalDomino);
void getDominoValues(const Grid<int>& board, Set<int>& dominoValues, const Vector<coord>& domino);
void addDomino(Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, Vector<coord>& domino);
coord getNextSpot(const coord currentSpot);
void removeDomino(Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, const Vector<coord> domino);
void certifyPairings(DominosaDisplay& display, Vector< Vector<coord> >& currentDominoes);

/*
 * Part 1: Human Pyramid
 */

/*
 * Wrapper function that initializes a Grid for memoization of weights
 * already calculated for faster recall by the recursive function
 */
double weightOnKnees(int row, int col, Vector<Vector<double> >& weights) {
    int humansInPyramidBase = weights.size();
    Grid<double> weightsSupported(humansInPyramidBase, weights[humansInPyramidBase-1].size());
    return weightOnKnees(row, col, weights, weightsSupported);
}

/*
 * Recursive function that determines corresponding weights for each member
 * of the Human Pyramid. It takes the top of the pyramid as a base case,
 * returns previously calculated weights via memoization from the Grid created
 * in the wrapper function, and then employs the appropriate weight
 * algorithm recursively.
 */
double weightOnKnees(int row, int col, Vector<Vector<double> >& weights, Grid<double>& weightsSupported) {
	if(row == 0 && col == 0) return weights[row][col];
	if(weightsSupported[row][col] != 0.0) return weightsSupported[row][col];

	double weightLeftShoulder;
	double weightRightShoulder;
	weightLeftShoulder = (weightsSupported.inBounds(row-1, col-1) && (row-1) - (col-1) >= 0) ? weightOnKnees(row-1, col-1, weights, weightsSupported) : 0;
	weightRightShoulder = (weightsSupported.inBounds(row-1, col) && (row-1) - col >= 0) ? weightOnKnees(row-1, col, weights, weightsSupported) : 0;
	double weight = weights[row][col] + (weightLeftShoulder + weightRightShoulder) / 2.0;
	weightsSupported[row][col] = weight;
	return weight;
}

/*
 * Part 2: Flood Fill
 */

/*
 * Wrapper function for floodFill that passes the current color
 * for the point selected, if in bounds, so to determine
 * the bounds of what to fill
 */
void floodFill(GBufferedImage& image, int x, int y, int color) {
    int preColor = (image.inBounds(x, y)) ? image.getRGB(x, y) : 0;
    floodFill(image, x, y, color, preColor);
}

/*
 * Recursive function that continually runs as long as the 
 * point passed is not of the desired color. Its base case
 * is the time at which the current point is the same color
 * as the desired color, which indicates a bound.
 */
void floodFill(GBufferedImage& image, int x, int y, int color, int preColor) {
	if(preColor == color) {
	} else {
		image.setRGB(x, y, color);
		if(image.inBounds(x-1, y) && preColor == image.getRGB(x-1, y)) floodFill(image, x-1, y, color, preColor);
		if(image.inBounds(x, y-1) && preColor == image.getRGB(x, y-1)) floodFill(image, x, y-1, color, preColor);
		if(image.inBounds(x+1, y) && preColor == image.getRGB(x+1, y)) floodFill(image, x+1, y, color, preColor);
		if(image.inBounds(x, y+1) && preColor == image.getRGB(x, y+1)) floodFill(image, x, y+1, color, preColor);
	}
}


/*
 * Part 3: Marble Board
 * /

/*
 * Recursive function that returns whether the Marble Solitaire game can be solved. It does this
 * by establishing base cases of whether the resulting iteration of the game has one
 * remaining marble, or if the current board has already been explored unsuccessfully.
 * The function invokes a helper function to determine all the possible moves that can be made
 * on the given board, and calculates the corresponding outcome for each.
 */

bool solvePuzzle(Grid<MarbleType>& board, int marblesLeft, Set<uint32_t>& exploredBoards, Vector<Move>& moveHistory) {
	
    if(marblesLeft == 1) return true;
    if(exploredBoards.contains(compressMarbleBoard(board))) return false;
    Vector<Move> moveList = findPossibleMoves(board);
    exploredBoards.add(compressMarbleBoard(board));
    if(exploredBoards.size() % 10000 == 0) {
			cout << "Boards evaluated: " << exploredBoards.size() << "\tDepth: " << moveHistory.size() << endl;
		}
    for(Move move: moveList) {
		makeMove(move, board);
		moveHistory.add(move);
		marblesLeft--;
		if(solvePuzzle(board, marblesLeft, exploredBoards, moveHistory)) {
			return true;
		} else {
			int lastMove = moveHistory.size()-1;
			undoMove(moveHistory[lastMove], board);
			marblesLeft++;
			moveHistory.remove(lastMove);
		}
	}
	return false;
}

/*
 * Helper function that determines all the possible moves for a given
 * Marble Solitaire board, randomly shuffling the results to ensure
 * variability in the game outcome.
 */
Vector<Move> findPossibleMoves(Grid<MarbleType>& board) {
	Vector<Move> moveList;
	for(int i = 0; i < board.numRows(); i++) {
		for(int j = 0; j < board.numCols(); j++) {
			checkMarbleNeighbors(board, moveList, i, j, -1, 0);
			checkMarbleNeighbors(board, moveList, i, j, 1, 0);
			checkMarbleNeighbors(board, moveList, i, j, 0, -1);
			checkMarbleNeighbors(board, moveList, i, j, 0, 1);
		}
	}
	random_shuffle(moveList.begin(), moveList.end());
	return moveList;
}

/*
 * Helper function employed by findPossibleMoves to determine
 * whether a move is valid according to the rules
 */
void checkMarbleNeighbors(Grid<MarbleType>& board, Vector<Move>& moveList, int startRow, int startCol, int rowOffset, int colOffset) {
	int endRow = startRow + rowOffset * 2;
	int endCol = startCol + colOffset * 2;
	if(board.inBounds(startRow, startCol) && board.inBounds(endRow, endCol)) {
		Move move(startRow, startCol, endRow, endCol);
		if(isValidMove(move, board)) moveList.add(move);
	}
}

/*
 * Part 4: Dominosa
 */

/*
 * Wrapper function that initializes requisite data types
 * so to accurately find whether a solution exists for the current
 * Dominosa board.
 */
bool canSolveBoard(DominosaDisplay& display, Grid<int>& board) {
	Vector< Vector<coord> > dominoes;
	HashSet< Vector<int> > occupiedSpots;
	coord currentSpot = {0,0};
	return canSolveBoard(display, board, dominoes, occupiedSpots, currentSpot);
}

/*
 * Recursive function that returns whether the current Dominosa board can
 * be solved. It establishes a base case of whether the maximum number of dominoes
 * have been placed on the board. It recursively loops through each address
 * on the board, evaluates the potential moves, including the rejection of moves
 * that would include overwritting an existing domino placement, and returns true
 * if it reaches base case/end.
 */
bool canSolveBoard(DominosaDisplay& display, Grid<int>& board, Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, coord& currentSpot) {
	int dominoesLeft = currentDominoes.size() - board.numCols();
	if(dominoesLeft == 0) {
		certifyPairings(display, currentDominoes);
		return true;
	}
	while(!isSpotEmpty(occupiedSpots, currentSpot)) currentSpot = getNextSpot(currentSpot);
	Vector< Vector<coord> > provisionalDominoes = getProvisionalDominoes(board, currentSpot);
	
	for(Vector<coord> domino : provisionalDominoes) {
		display.provisonallyPair(domino[0], domino[1]);
		if(isNumberPairFree(board, currentDominoes, domino)) {
			addDomino(currentDominoes, occupiedSpots, domino);
            coord nextSpot = getNextSpot(currentSpot);
            if(canSolveBoard(display, board, currentDominoes, occupiedSpots, nextSpot)) {
				return true;
			} else {
				removeDomino(currentDominoes, occupiedSpots, domino);
			}
		}
		display.vetoProvisionalPairing(domino[0], domino[1]);
		display.eraseProvisionalPairing(domino[0], domino[1]);
	}
	return false;
}

/*
 * Helper function that returns a Vector of board locations and the possible
 * dominoes that can be placed using that location as a starting position.
 */
Vector< Vector<coord> > getProvisionalDominoes(const Grid<int>& board, const coord& currentSpot) {
	Vector< Vector<coord> > provisionalDominoes;
	int row = currentSpot.row;
	int col = currentSpot.col;
	if(board.inBounds(row, col+1)) {
		provisionalDominoes.add(createDomino(row, col, row, col+1));
	}
	if(board.inBounds(row+1, col)) {
		provisionalDominoes.add(createDomino(row+1, col, row, col));
	}
    return provisionalDominoes;
}

/*
 * Simple helper function to create a domino given two sets of
 * coordinates stored as integer. Used for clarity in upstream
 * helper functions.
 */
Vector<coord> createDomino(int row1, int col1, int row2, int col2) {
	Vector<coord> domino;
    domino.add({row1, col1});
    domino.add({row2, col2});
	return domino;
}

/*
 * Helper function that determines if a board address is empty.
 * It does so by checking a set of previously placed board addresses.
 */
bool isSpotEmpty(const HashSet< Vector<int> >& occupiedSpots, const coord currentSpot) {
	Vector<int> currentSpotCoord = convertCoordToVector(currentSpot);
	if(occupiedSpots.contains(currentSpotCoord)) return false;
	return true;
}

/*
 * Due to issues with ADTs and the coord struct that doesn't
 * specify handling, this helper function converts
 * a coord to an ADT-approved data type
 */
Vector<int> convertCoordToVector(const coord c) {
	Vector<int> coordVector;
	coordVector.add(c.row);
	coordVector.add(c.col);
	return coordVector;
}

/*
 * Helper function that returns whether or not a given set of
 * domino values, without respect to order, has been used in the
 * game. If so, it returns so to prevent further development of
 * this board track.
 */
bool isNumberPairFree(const Grid<int>& board, const Vector< Vector<coord> >& currentDominoes, const Vector<coord>& provisionalDomino) {
	Set<int> provisionalDominoValues;
	getDominoValues(board, provisionalDominoValues, provisionalDomino);
	for(Vector<coord> currentDomino : currentDominoes) {
		Set<int> currentDominoValues;
		getDominoValues(board, currentDominoValues, currentDomino);
		if(provisionalDominoValues == currentDominoValues) return false;
	}
	return true;
}

/*
 * Given a specific domino, this helper function add its corresponding
 * value pair to a Set that keeps track of it so to prevent duplicates
 */
void getDominoValues(const Grid<int>& board, Set<int>& dominoValues, const Vector<coord>& domino) {
	for(int i = 0; i < domino.size(); i++) {
		coord c = domino[i];
		dominoValues.add( board[c.row][c.col] );
	}
}

/*
 * Simple helper function that adds an ADT-approved data type to the
 * Vector of board coordinates that keeps track of the current dominoes
 * put in place.
 */
void addDomino(Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, Vector<coord>& domino) {
	Vector<int> dominoCoord1 = convertCoordToVector(domino[0]);
	Vector<int> dominoCoord2 = convertCoordToVector(domino[1]);
	occupiedSpots.add(dominoCoord1);
	occupiedSpots.add(dominoCoord2);
	currentDominoes.add(domino);
}

/*
 * Simple helper function that, given a board address as coord,
 * returns the next spot downstream on the dominosa board.
 */
coord getNextSpot(const coord currentSpot) {
	coord nextSpot;
	if(currentSpot.row == 0) {
		nextSpot = {1, currentSpot.col};
	} else {
		nextSpot = {0, currentSpot.col + 1};
	}
	return nextSpot;
}

/*
 * Helper function used to remove a previously placed domino
 * from the ADTs that track dominos in place.
 */
void removeDomino(Vector< Vector<coord> >& currentDominoes, HashSet< Vector<int> >& occupiedSpots, const Vector<coord> domino) {
	Vector<int> dominoCoord1 = convertCoordToVector(domino[0]);
	Vector<int> dominoCoord2 = convertCoordToVector(domino[1]);
	occupiedSpots.remove(dominoCoord1);
	occupiedSpots.remove(dominoCoord2);
	if(!currentDominoes.isEmpty()) currentDominoes.remove(currentDominoes.size()-1);
}

/*
 * Simple helper function that invokes the UI to validate all dominos
 * in place once the base case is reached
 */
void certifyPairings(DominosaDisplay& display, Vector< Vector<coord> >& currentDominoes) {
	for(int i = currentDominoes.size()-1; i >= 0; i--){
		Vector<coord> domino = currentDominoes[i];
		display.certifyPairing(domino[0], domino[1]);
	}
}
