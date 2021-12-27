/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <cmath>
using namespace std;

class Board
{
    public:
        // size of the yinsh board (number of hexagons)
        int size;

        // number of rows starting from 0 to (2 * N)
        int num_rows;

        // number of rings with each player at the start
        int num_rings;

        // number of markers needed in a row to remove a ring
        int num_resolution;

        // number of rings to be removed to win the game
        int num_rings_win;

        // orientation of the markers on the board where board[row][position] = 0 or 1 or 2
        vector< vector< int > > board;

        // number of rings of Players placed on the board
        int ring_counter[2];

        // place or move phase of the board
        int phase;

        // mapping of ring number to it's position
        vector< pair< int, int > > ring_map;

        // number of rings won by player 1
        int num_rings_removed1;

        // number of rings won by player 2
        int num_rings_removed2;

        // number of markers of player 1
        int num_markers_1;

        // number of markers of player 2
        int num_markers_2;

    public:
        /*
         * Constructor for Board
         *
         * @param size is the number of hexagons.
         * @param num_rings is the number of rings available to each player.
         * @param num_resolution is the number of markers needed in a row for a ring removal.
         * @param num_ring_win is the number of rings required to be remove in order to win.
        */
        Board(int size, int num_rings, int num_resolution, int num_rings_win);

        /*
         * define low, high position for a row number
         *
         * @param row is the row number in range 0 to num_rows
        */
        pair< int, int > low_high(int row);

        /*
         * define mapping of hexagon, position to row, position
         *
         * @param hexagon is the hexagon number
         * @param hex_position is the position number on the hexagon
        */
        pair< int, int > for_map(int hexagon, int hex_position);

        /*
         * define mapping of row, position to hexagon, position
         *
         * @param row is the row number
         * @param position is the position number on the row
        */
        pair< int, int > back_map(int row, int position);

        /*
         * Place a ring on the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row - row number where ring is to be placed.
         * @param position - position on row where ring is to be placed.
        */
        void place_ring(int player, int row, int position);

        /*
         * Backtrack a place ring move.
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row - row number from where ring is to be removed.
         * @param position - position on row from where ring is to be removed.
        */
        void backtrack_place_ring(int player, int row, int position);

        /*
         * Remove a ring from the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row - row number from where ring is to be removed.
         * @param position - position number from where ring is to be removed.
        */
        void remove_ring(int player, int row, int position);

        /*
         * Backtrack from the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row - row number where ring is to be placed.
         * @param position - position number where ring is to be placed.
        */
        void backtrack_remove_ring(int player, int ring_number, int row, int position);

        /*
         * Move a ring on the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row1 - row number from where ring is to be moved.
         * @param position1 - position on row from where ring is to be moved.
         * @param row2 - row number where ring is to be placed.
         * @param position2 - position on row where ring is to be placed.
        */
        void move_ring(int player, int row1, int position1, int row2, int position2);

        /*
         * Backtrack a ring on the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row2 - row number from where ring is to be moved.
         * @param position2 - position on row from where ring is to be moved.
         * @param row1 - row number where ring is to be placed.
         * @param position1 - position on row where ring is to be placed.
        */
        void backtrack_move_ring(int player, int row1, int position1, int row2, int position2);

        /*
         * Remove a row of markers from the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row1 - row number of starting marker.
         * @param position1 - position on row of starting marker.
         * @param row2 - row number of ening marker.
         * @param position2 - position on row of ening marker.
        */
        void remove_markers(int player, int row1, int position1, int row2, int position2);

        /*
         * Retain a row of markers on the board
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row1 - row number of starting marker.
         * @param position1 - position on row of starting marker.
         * @param row2 - row number of ening marker.
         * @param position2 - position on row of ening marker.
        */
        void backtrack_remove_markers(int player, int row1, int position1, int row2, int position2);

        /*
         * Generate valid ring moves for a ring
         *
         * @param player - 1 for player 1 & 2 for player 2.
         * @param row1 - row number from where ring is to be moved.
         * @param position1 - position on row from where ring is to be moved.
         * @param row2 - row number where ring is to be placed.
         * @param position2 - position on row where ring is to be placed.
        */
        vector< pair< int, int > > generate_valid_move_ring(int row, int position);

        /*
         * Get a pair of vector representing set of possible removal of num_resolution markers for player & opponent
         *
        */
        pair< vector< vector< int > > , vector< vector< int > > > generate_remove_markers(int player, int row1, int position1, int row2, int position2);

        /*
         * Check if two lines intersect on the board
        */
        bool intersect(int row1s, int position1s, int row1e, int position1e, int row2s, int position2s, int row2e, int position2e);

        /*
         * Return list of all possible removal of maximal markers
        */
        vector< vector< vector< int > > > unique_non_intersecting_remove_markers(vector< vector< int > > remove_markers_player);

        /*
         * Get the phase of the game
         *
        */
        int get_phase();

        /*
         *
         * Return the heuristic for a board position
         *
        */
        double heuristic(int player);

        /*
         * Print the board.
        */
        void print_board();
};

#endif  /* BOARD_H */
