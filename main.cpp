/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <limits>
#include <string.h>
#include <unistd.h>
#include "Board.h"
#include "minimax_alpha_beta.h"
using namespace std;

int main()
{
    int size = 5;
    int num_rings = 5;
    int num_resolution = 5;
    int num_rings_win = 3;

    int player;
    double timer;

    cin >> player >> size >> timer;

    Board yinsh_board(size, num_rings, num_resolution, num_rings_win, player);

    string dummy;
    getline(cin, dummy);

    int depth = 2;
    vector< vector< vector< int > > > passed_to_my_player;

    while(true)
    {
        string move;
        if(player == 1)
        {
            pair< string, double > minimax_decision_result = minimax_decision(yinsh_board, depth, player, passed_to_my_player, "");
            move = minimax_decision_result.first;
            cout << move << "\n";
            passed_to_my_player = yinsh_board.play_move(move, 1);
            yinsh_board.print_board();

            getline(cin, move);
            move += " ";
            passed_to_my_player = yinsh_board.play_move(move, 2);
            yinsh_board.print_board();
        }
        else
        {
            getline(cin, move);
            move += " ";
            passed_to_my_player = yinsh_board.play_move(move, 1);
            if(passed_to_my_player.size() != 0)
            {
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
                cerr << "CHULL" << "\n";
            }
            for(int i = 0; i < passed_to_my_player.size(); i++)
            {
                for(int j = 0; j < passed_to_my_player[i].size(); j++)
                    cerr << passed_to_my_player[i][j][0] << " " << passed_to_my_player[i][j][1] << " " << passed_to_my_player[i][j][2] << " " << passed_to_my_player[i][j][3] << ",    ";
                cerr << "\n";
            }
            yinsh_board.print_board();

            pair< string, double > minimax_decision_result = minimax_decision(yinsh_board, depth, player, passed_to_my_player, "");
            move = minimax_decision_result.first;
            cout << move << "\n";
            passed_to_my_player = yinsh_board.play_move(move, 2);
            yinsh_board.print_board();
        }
    }
    return 0;
}
