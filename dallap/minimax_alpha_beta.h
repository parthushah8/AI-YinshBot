/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#ifndef MINIMAX_ALPHA_BETA_H
#define MINIMAX_ALPHA_BETA_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <limits>
#include <string.h>
#include <unistd.h>
#include "Board.h"
using namespace std;

#define POS_INF (numeric_limits<double>::infinity())
#define NEG_INF (-numeric_limits<double>::infinity())

void makeCombiUtil(vector<vector<int> >& ans, vector<int>& tmp, int n, int left, int k);

vector<vector<int> > makeCombi(int n, int k);

vector < vector < int > > subset(int player, vector< pair< int, int > > ring_map, int k);

pair< string, double > minimax_decision(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, string previous_string);

double max_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, double alpha, double beta);

double min_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, double alpha, double beta);

#endif  /* MINIMAX_ALPHA_BETA_H */
