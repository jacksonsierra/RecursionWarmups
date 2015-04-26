/*
 * CS 106X Recursion Problems
 * This client program contains a text menu for running your
 * assignment's various recursion problems.
 * You don't need to modify this file.
 */

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "console.h"
#include "filelib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "ginteractors.h"
#include "gobjects.h"
#include "grid.h"
#include "gwindow.h"
#include "map.h"
#include "hashmap.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"

#include "recursionproblems.h"
#include "marbles.h"
#include "dominosa.h"

using namespace std;

// constants for min/max weight in human pyramid
const int MIN_WEIGHT = 50;
const int MAX_WEIGHT = 150;

// uncomment the line below to 'rig' the random number generator
// for the Human Pyramid problem
#define HUMAN_PYRAMID_RANDOM_SEED 106

// constants for graphical window sizes
const int FLOOD_WINDOW_WIDTH = 500;
const int FLOOD_WINDOW_HEIGHT = 400;
const int FLOOD_FILL_NUM_SHAPES = 100;

// uncomment the line below to get the same shapes every time for flood fill
#define FLOOD_FILL_RANDOM_SEED 42

// private globals to help implement flood fill pixel functionality
static GBufferedImage* floodFillPixels = NULL;
static GWindow* floodFillWindow;

int main() {
    cout << "CS 106X Recursion Problems" << endl;
    while (true) {
        cout << endl;
        cout << "Choose a problem:" << endl;
        cout << "1) Human Pyramid" << endl;
        cout << "2) Flood Fill" << endl;
        cout << "3) Marble Board" << endl;
        cout << "4) Dominosa" << endl;
        int choice = getInteger("Enter your choice (or 0 to quit): ");
        cout << endl;
        if (choice == 0)      { break; }
        else if (choice == 1) { test_humanPyramid(); }
        else if (choice == 2) { test_floodFill(); }
        else if (choice == 3) { test_marbleBoard(); }
        else if (choice == 4) { test_dominosa(); }
    }

    cout << "Exiting." << endl;
    return 0;
}


/*
 * Runs and tests your humanPyramid function.
 */
void test_humanPyramid() {
    int cols = getInteger("How many people are on the bottom row? ");

    // possibly rig the random generator's output
#ifdef HUMAN_PYRAMID_RANDOM_SEED
    setRandomSeed(HUMAN_PYRAMID_RANDOM_SEED);
#endif // HUMAN_PYRAMID_RANDOM_SEED

    // populate vector of weights
    Vector<Vector<double> > weights;
    for (int row = 0; row < cols; row++) {
        Vector<double> currentRow;
        for (int col = 0; col <= row; col++) {
            double weight = randomReal(MIN_WEIGHT, MAX_WEIGHT);
            currentRow.add(weight);
        }
        weights.add(currentRow);
    }

    // print weights
    cout << "Each person's own weight:" << endl;
    cout << fixed << setprecision(2);
    for (int row = 0; row < weights.size(); row++) {
        for (int col = 0; col < weights[row].size(); col++) {
            cout << weights[row][col] << " ";
        }
        cout << endl;
    }
    cout << endl;

    // print weight on knees for each person in pyramid
    cout << "Weight on each person's knees:" << endl;
    for (int row = 0; row < weights.size(); row++) {
        for (int col = 0; col < weights[row].size(); col++) {
            double result = weightOnKnees(row, col, weights);
            cout << result << " ";
        }
        cout << endl;
    }
    cout << resetiosflags(ios::fixed | ios::floatfield);
}

/*
 * Runs and tests your floodFill function.
 */
void test_floodFill() {
    GObject::setAntiAliasing(false);
    floodFillWindow = new GWindow(FLOOD_WINDOW_WIDTH, FLOOD_WINDOW_HEIGHT);
    floodFillWindow->setWindowTitle("CS 106X Flood Fill");
    // floodFillWindow->center();
    // floodFillWindow->setRepaintImmediately(false);

    Map<string, int> colorMap;
    colorMap["Red"]    = 0x8c1515;   // Stanford red
    colorMap["Yellow"] = 0xeeee00;   // yellow
    colorMap["Blue"]   = 0x0000cc;   // blue
    colorMap["Green"]  = 0x00cc00;   // green
    colorMap["Purple"] = 0xcc00cc;   // purple
    colorMap["Orange"] = 0xff8800;   // orange
    Vector<string> colorVector = colorMap.keys();

    GLabel* fillLabel = new GLabel("Fill color:");
    GChooser* colorList = new GChooser();
    for (string key : colorMap) {
        colorList->addItem(key);
    }
    floodFillWindow->addToRegion(fillLabel, "SOUTH");
    floodFillWindow->addToRegion(colorList, "SOUTH");

    // use buffered image to store individual pixels
    if (floodFillPixels) {
        delete floodFillPixels;
        floodFillPixels = NULL;
    }
    floodFillPixels = new GBufferedImage(
                /* x */ 0,
                /* y */ 0,
                /* width */ FLOOD_WINDOW_WIDTH,
                /* height */ FLOOD_WINDOW_HEIGHT,
                /* rgb fill */ 0xffffff);

    // draw several random shapes
#ifdef FLOOD_FILL_RANDOM_SEED
    setRandomSeed(FLOOD_FILL_RANDOM_SEED);
#endif // FLOOD_FILL_RANDOM_SEED

    for (int i = 0; i < FLOOD_FILL_NUM_SHAPES; i++) {
        double x = randomInteger(0, FLOOD_WINDOW_WIDTH  - 100);
        double y = randomInteger(0, FLOOD_WINDOW_HEIGHT - 100);
        double w = randomInteger(20, 100);
        double h = randomInteger(20, 100);
        int color = colorMap[colorVector[randomInteger(0, colorVector.size() - 1)]];
        floodFillPixels->fillRegion(x, y, w, h, color);
    }
    floodFillWindow->add(floodFillPixels);

    // main event loop to process events as they happen
    while (true) {
        GEvent e = waitForEvent(MOUSE_EVENT | WINDOW_EVENT);
        if (e.getEventClass() == MOUSE_EVENT) {
            if (e.getEventType() != MOUSE_CLICKED) { continue; }
            colorList->setEnabled(false);
            GMouseEvent mouseEvent(e);
            string colorStr = colorList->getSelectedItem();
            int color = colorMap[colorStr];
            int mx = (int) mouseEvent.getX();
            int my = (int) mouseEvent.getY();
            cout << "Flood fill at (x=" << dec << mx << ", y=" << my << ")"
                 << " with color " << hex << setw(6) << setfill('0') << color
                 << dec << endl;
            floodFill(*floodFillPixels, mx, my, color);
            colorList->setEnabled(true);
            // floodFillWindow->repaint();
        } else if (e.getEventClass() == WINDOW_EVENT) {
            if (e.getEventType() == WINDOW_CLOSED) {
                // make sure that it was the flood fill window that got closed
                if (!floodFillWindow->isOpen() || !floodFillWindow->isVisible()) {
                    break;
                }
            }
        }
    }
    cout << resetiosflags(ios::fixed | ios::floatfield);
}

/*
 * Returns the color of the given x/y pixel onscreen.
 * If the x/y coordinates are out of range, throws an Error.
 */
int getPixelColor(int x, int y) {
    return floodFillPixels->getRGB(x, y);
}

/*
 * Sets the color of the given x/y pixel onscreen to the given color.
 * If the x/y coordinates or color are out of range, throws an Error.
 */
void setPixelColor(int x, int y, int color) {
    floodFillPixels->setRGB(x, y, color);
}

void test_marbleBoard() {
    cout << "Welcome to Marble Solitaire!" << endl;
    MarbleGraphics mg;
    Grid<MarbleType> board(7,7);

    do {
        int marblesRemaining = initializeBoard(board);
        mg.drawBoard(board);
        marblesRemaining = humanPlay(board, marblesRemaining, mg);
        //Only activate computer's turn if human hasn't won
        if (marblesRemaining == 1){
            cout << "Congrats! You have won. :-)" << endl;
        }
        else {
            computerPlay(board, marblesRemaining, mg);
        }
    } while (getLine("Game over! Play again? [y/n] ") == "y");

}

void test_dominosa() {
    DominosaDisplay display;
    welcome();
    while (true) {
        int numColumns = getIntegerInRange("How many columns? [0 to exit]: ", 9, 25);
        if (numColumns == 0) break;
        Grid<int> board(2, numColumns);
        populateBoard(board, 1, ceil(2 * sqrt((double) numColumns)));
        display.drawBoard(board);
        if (canSolveBoard(display, board)) {
            cout << "The board can be solved, and one such solution is drawn above." << endl;
        } else {
            cout << "This board you see can't be solved." << endl;
        }
    }
    HashMap<Vector<string>, Vector<string>> s;

    cout << "Okay, thanks for watching, and come back soon." << endl;
    cout << "Click the mouse anywhere in the window to exit." << endl;
}
