/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include "Board.h"
using namespace std;

int main()
{
    int size, num_rings, num_resolution, num_rings_win;

    cout << "Enter N, M, K, L for the Board: ";
    cin >> size >> num_rings >> num_resolution >> num_rings_win;

    Board test_board(size, num_rings, num_resolution, num_rings_win);
    test_board.print_board();

    int player = 1;
    string delimiter = " ";

    while(true)
    {
        cout << "Player " << player << ": ";
        string move;
        getline(cin, move);

        size_t pos = 0;
        string token;

        pos = move.find(delimiter);
        token = move.substr(0, pos);
        move.erase(0, pos + delimiter.length());

        if(move == "exit")
        {
            cout<<"Game Over!";
            break;
        }
        if(token == "P")
        {
            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hexagon = stoi(token);

            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hex_position = stoi(token);

            // pair< int, int > row_pos = test_board.for_map(hexagon, hex_position);
            // int row = row_pos.first;
            // int position = row_pos.second;

            test_board.place_ring(player, hexagon + 5, hex_position + 5);
            test_board.print_board();
        }
        else if(token == "S")
        {
            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hexagon1 = stoi( token );

            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hex_position1 = stoi( token );

            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            if (token != "M")
                cout<<"Invalid Move\n";

            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hexagon2 = stoi(token);

            pos = move.find(delimiter);
            token = move.substr(0, pos);
            move.erase(0, pos + delimiter.length());

            int hex_position2 = stoi(token);

            // pair< int, int > row_pos1 = test_board.for_map(hexagon1, hex_position1);
            // int row1 = row_pos1.first;
            // int position1 = row_pos1.second;
            // pair< int, int > row_pos2 = test_board.for_map(hexagon2, hex_position2);
            // int row2 = row_pos2.first;
            // int position2 = row_pos2.second;

            test_board.move_ring(player, hexagon1 + 5, hex_position1 + 5, hexagon2 + 5, hex_position2 + 5);
            test_board.print_board();

        }
        else if(token == "Q")
        {

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hexagon1_D = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hex_position1_D = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          if (token != "M")
              cout<<"Invalid Move\n";

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hexagon2_D = stoi(token);

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hex_position2_D = stoi(token);

          // pair< int, int > row_pos1 = test_board.for_map(hexagon1, hex_position1);
          // int row1 = row_pos1.first;
          // int position1 = row_pos1.second;
          // pair< int, int > row_pos2 = test_board.for_map(hexagon2, hex_position2);
          // int row2 = row_pos2.first;
          // int position2 = row_pos2.second;

          test_board.move_ring(player, hexagon1_D + 5, hex_position1_D + 5, hexagon2_D + 5, hex_position2_D + 5);
          test_board.print_board();

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          if (token != "RS")
              cout<<"Invalid Move\n";

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hexagon1 = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hex_position1 = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          if ( token != "RE" )
              cout<<"Invalid Move\n";

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hexagon2 = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hex_position2 = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          if ( token != "X" )
              cout<<"Invalid Move\n";

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hexagonR = stoi( token );

          pos = move.find(delimiter);
          token = move.substr(0, pos);
          move.erase(0, pos + delimiter.length());

          int hex_positionR = stoi( token );

          test_board.remove_markers(player, hexagon1 + 5, hex_position1 + 5, hexagon2 + 5, hex_position2 + 5);
          test_board.remove_ring(player, hexagonR + 5, hex_positionR + 5);
          test_board.print_board();
        }
        else
        {
            cout << "Illegal Move!! Play Again...\n";
            player = 3-player;
        }

        player = 3 - player;
    }

    return 0;
}
