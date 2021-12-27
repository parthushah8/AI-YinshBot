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
using namespace std;

double min_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player);
double max_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player);

void makeCombiUtil(vector<vector<int> >& ans, vector<int>& tmp, int n, int left, int k)
{
    if (k == 0) {
        ans.push_back(tmp);
        return;
    }

    for (int i = left; i <= n; ++i)
    {
        tmp.push_back(i);
        makeCombiUtil(ans, tmp, n, i + 1, k - 1);
        tmp.pop_back();
    }
}

vector<vector<int> > makeCombi(int n, int k)
{
    vector<vector<int> > ans;
    vector<int> tmp;
    makeCombiUtil(ans, tmp, n, 1, k);
    return ans;
}

vector < vector < int > > subset(int player, vector< pair< int, int > > ring_map, int k)
{
    vector < int > real_ring_map;
    for(int i = player - 1 ; i < ring_map.size(); i += 2)
        if(ring_map[i].first != -1)
            real_ring_map.push_back(i);
    int n = real_ring_map.size();
    vector< vector< int > > subset_indices = makeCombi(n, k);
    vector< vector< int > > answer;
    for(int i = 0; i < subset_indices.size(); i++)
    {
        vector< int > subset_ring;
        for(int j = 0; j < k; j++)
            subset_ring.push_back(real_ring_map[subset_indices[i][j] - 1]);
        answer.push_back(subset_ring);
    }
    return answer;
}

pair< string, double > minimax_decision(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, string previous_string)
{
    string answer = previous_string;

    if(depth == 0)
        return make_pair(answer, 0);

    int max_removable_markers = -1;
    int max_removable_markers_in = -1;
    for(int i = 0; i < all_remove_markers_player.size(); i++)
    {
        if(all_remove_markers_player[i].size() > max_removable_markers)
        {
            max_removable_markers = all_remove_markers_player[i].size();
            max_removable_markers_in = i;
        }
    }

    int num_rings_removed;
    if(player == 1)
        num_rings_removed = state.num_rings_removed1;
    else
        num_rings_removed = state.num_rings_removed2;

    if(max_removable_markers + num_rings_removed >= state.num_rings_win)
    {
        int num_rings_more = state.num_rings_win - num_rings_removed;
        vector< vector <int> > min_removal_set = all_remove_markers_player[max_removable_markers_in];
        int current_removed_ring = (player - 1);
        for(int i = 0; i < num_rings_more; i++)
        {
            if(state.ring_map[current_removed_ring].first != -1)
            {
                pair< int, int > hex_cord1 = state.back_map(min_removal_set[i][0], min_removal_set[i][1]);
                pair< int, int > hex_cord2 = state.back_map(min_removal_set[i][2], min_removal_set[i][3]);
                pair< int, int > removed_ring = state.back_map(state.ring_map[current_removed_ring].first, state.ring_map[current_removed_ring].second);
                answer = previous_string;
                answer += "RS " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                answer += "RE " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                answer += "X " + to_string(removed_ring.first) + " " + to_string(removed_ring.second) + " ";
            }
            current_removed_ring += 2;
        }
        return make_pair(answer, 0);
    }

    double optimal = -numeric_limits<double>::infinity();
    vector< vector< vector< int > > > empty;
    if(all_remove_markers_player.size() != 0)
    {
        for(int i = 0; i < all_remove_markers_player.size(); i++)
        {
            vector< vector< int > > remove_markers_player = all_remove_markers_player[i];
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
            for(int l = 0; l < subset_rings.size(); l++)
            {
                vector< pair< int, int > > retained_ring_positions;
                for(int m = 0; m < remove_markers_player.size(); m++)
                {
                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                }
                pair< string, double> minimax_decision_result = minimax_decision(state, depth, player, empty, "");
                answer = minimax_decision_result.first;
                double neigbour_optimal = minimax_decision_result.second;
                if(neigbour_optimal > optimal)
                {
                    optimal = neigbour_optimal;
                    for(int m = 0; m < remove_markers_player.size(); m++)
                    {
                        pair< int, int > hex_cord1 = state.back_map(remove_markers_player[m][0], remove_markers_player[m][1]);
                        pair< int, int > hex_cord2 = state.back_map(remove_markers_player[m][2], remove_markers_player[m][3]);
                        pair< int, int > removed_ring = state.back_map(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                        string temp = "";
                        temp += "RS " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                        temp += "RE " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                        temp += "X " + to_string(removed_ring.first) + " " + to_string(removed_ring.second) + " ";
                        answer = temp + answer;
                    }
                }
                for(int m = 0; m < remove_markers_player.size(); m++)
                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
            }
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.backtrack_remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
        }
    }
    else
    {
        if(state.get_phase() == 0)
        {
            for(int i = 0; i < state.num_rows; i++)
            {
                pair< int, int > range = state.low_high(i);
                int low = range.first;
                int high = range.second;

                for(int j = low; j <= high; j++)
                {
                    if(state.board[i][j] == 0)
                    {
                        state.place_ring(player, i, j);
                        double neigbour_optimal = min_value(state, depth - 1, 3 - player, empty);
                        if(neigbour_optimal > optimal)
                        {
                            optimal = neigbour_optimal;
                            pair< int, int > hex_cord = state.back_map(i, j);
                            answer = previous_string;
                            answer += "P " + to_string(hex_cord.first) + " " + to_string(hex_cord.second) + " ";
                        }
                        state.backtrack_place_ring(player, i, j);
                    }
                }
            }
        }
        else
        {
            for(int i = (player - 1); i < state.ring_map.size(); (i += 2))
            {
                pair<int, int> ring_pos = state.ring_map[i];
                if(ring_pos.first == -1 || ring_pos.second == -1)
                    continue;
                vector< pair< int, int > > valid_ring_pos = state.generate_valid_move_ring(ring_pos.first, ring_pos.second);
                for(int j = 0; j < valid_ring_pos.size(); j++)
                {
                    state.move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    pair< vector< vector < int > >, vector< vector< int > > > valid_remove_markers = state.generate_remove_markers(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    vector< vector< vector < int > > >  valid_remove_markers_player = state.unique_non_intersecting_remove_markers(valid_remove_markers.first);
                    vector< vector< vector < int > > > valid_remove_markers_opponent = state.unique_non_intersecting_remove_markers(valid_remove_markers.second);
                    if(valid_remove_markers_player.size() == 4)
                    {
                        for(int k = 0; k < 4; k++)
                        {
                            for(int l = 0; l < valid_remove_markers_player[k].size(); l++)
                            {
                                cout << valid_remove_markers_player[k][l][0] << " " << valid_remove_markers_player[k][l][1] << " " << valid_remove_markers_player[k][l][2] << " " << valid_remove_markers_player[k][l][3] << ", ";
                            }
                            cout << "\n";
                        }
                    }
                    if(valid_remove_markers_player.size() == 0)
                    {
                        double neigbour_optimal = min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent);
                        if(neigbour_optimal > optimal)
                        {
                            optimal = neigbour_optimal;
                            pair< int, int > hex_cord1 = state.back_map(ring_pos.first, ring_pos.second);
                            pair< int, int > hex_cord2 = state.back_map(valid_ring_pos[j].first, valid_ring_pos[j].second);
                            answer = previous_string;
                            answer += "S " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                            answer += "M " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                        }
                    }
                    else
                    {
                        max_removable_markers = -1;
                        max_removable_markers_in = -1;
                        for(int k = 0; k < valid_remove_markers_player.size(); k++)
                            if(valid_remove_markers_player[k].size() > max_removable_markers)
                            {
                                max_removable_markers = valid_remove_markers_player[k].size();
                                max_removable_markers_in = k;
                            }

                        num_rings_removed;
                        if(player == 1)
                            num_rings_removed = state.num_rings_removed1;
                        else
                            num_rings_removed = state.num_rings_removed2;

                        if(max_removable_markers + num_rings_removed >= state.num_rings_win)
                        {
                            int num_rings_more = state.num_rings_win - num_rings_removed;
                            vector< vector <int> > min_removal_set = valid_remove_markers_player[max_removable_markers_in];
                            int current_removed_ring = (player - 1);
                            for(int k = 0; k < num_rings_more; k++)
                            {
                                if(state.ring_map[current_removed_ring].first != -1)
                                {
                                    pair< int, int > hex_cord1 = state.back_map(min_removal_set[k][0], min_removal_set[k][1]);
                                    pair< int, int > hex_cord2 = state.back_map(min_removal_set[k][2], min_removal_set[k][3]);
                                    pair< int, int > removed_ring = state.back_map(state.ring_map[current_removed_ring].first, state.ring_map[current_removed_ring].second);
                                    answer = previous_string;
                                    answer += "RS " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                                    answer += "RE " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                                    answer += "X " + to_string(removed_ring.first) + " " + to_string(removed_ring.second) + " ";
                                }
                                current_removed_ring += 2;
                            }
                            state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                            return make_pair(answer, optimal);
                        }
                        for(int k = 0; k < valid_remove_markers_player.size(); k++)
                        {
                            vector< vector< int > > remove_markers_player = valid_remove_markers_player[k];
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
                            for(int l = 0; l < subset_rings.size(); l++)
                            {
                                vector< pair< int, int > > retained_ring_positions;
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                {
                                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                                }
                                double neigbour_optimal = min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent);
                                if(neigbour_optimal > optimal)
                                {
                                    optimal = neigbour_optimal;
                                    pair< int, int > hex_cord1 = state.back_map(ring_pos.first, ring_pos.second);
                                    pair< int, int > hex_cord2 = state.back_map(valid_ring_pos[j].first, valid_ring_pos[j].second);
                                    answer = previous_string;
                                    answer += "S " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                                    answer += "M " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                                    for(int m = 0; m < remove_markers_player.size(); m++)
                                    {
                                        hex_cord1 = state.back_map(remove_markers_player[m][0], remove_markers_player[m][1]);
                                        hex_cord2 = state.back_map(remove_markers_player[m][2], remove_markers_player[m][3]);
                                        pair< int, int > removed_ring = state.back_map(retained_ring_positions[m].first, retained_ring_positions[m].second);
                                        answer += "RS " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                                        answer += "RE " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                                        answer += "X " + to_string(removed_ring.first) + " " + to_string(removed_ring.second) + " ";
                                    }
                                }
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                            }
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.backtrack_remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                        }
                    }
                    state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                }
            }
        }
    }
    return make_pair(answer, optimal);
}

double max_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player)
{
    if(depth == 0)
        return state.heuristic(player);

    int max_removable_markers = -1;
    int max_removable_markers_in = -1;
    for(int i = 0; i < all_remove_markers_player.size(); i++)
        if(all_remove_markers_player[i].size() > max_removable_markers)
        {
            max_removable_markers = all_remove_markers_player[i].size();
            max_removable_markers_in = i;
        }

    int num_rings_removed;
    if(player == 1)
        num_rings_removed = state.num_rings_removed1;
    else
        num_rings_removed = state.num_rings_removed2;

    if(max_removable_markers + num_rings_removed >= state.num_rings_win)
        return numeric_limits<double>::infinity();

    double optimal = -numeric_limits<double>::infinity();
    vector< vector< vector< int > > > empty;

    if(all_remove_markers_player.size() != 0)
    {
        for(int i = 0; i < all_remove_markers_player.size(); i++)
        {
            vector< vector< int > > remove_markers_player = all_remove_markers_player[i];
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
            for(int l = 0; l < subset_rings.size(); l++)
            {
                vector< pair< int, int > > retained_ring_positions;
                for(int m = 0; m < remove_markers_player.size(); m++)
                {
                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                }
                optimal = max(optimal, max_value(state, depth, player, empty));
                for(int m = 0; m < remove_markers_player.size(); m++)
                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
            }
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.backtrack_remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
        }
    }
    else
    {
        if(state.get_phase() == 0)
        {
            for(int i = 0; i < state.num_rows; i++)
            {
                pair< int, int > range = state.low_high(i);
                int low = range.first;
                int high = range.second;

                for(int j = low; j <= high; j++)
                {
                    if(state.board[i][j] == 0)
                    {
                        state.place_ring(player, i, j);
                        optimal = max(optimal, min_value(state, depth - 1, 3 - player, empty));
                        state.backtrack_place_ring(player, i, j);
                    }
                }
            }
        }
        else
        {
            for(int i = (player - 1); i < state.ring_map.size(); (i += 2))
            {
                pair<int, int> ring_pos = state.ring_map[i];
                if(ring_pos.first == -1 || ring_pos.second == -1)
                    continue;
                vector< pair< int, int > > valid_ring_pos = state.generate_valid_move_ring(ring_pos.first, ring_pos.second);
                for(int j = 0; j < valid_ring_pos.size(); j++)
                {
                    state.move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    pair< vector< vector < int > >, vector< vector< int > > > valid_remove_markers = state.generate_remove_markers(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    vector< vector< vector < int > > >  valid_remove_markers_player = state.unique_non_intersecting_remove_markers(valid_remove_markers.first);
                    vector< vector< vector < int > > > valid_remove_markers_opponent = state.unique_non_intersecting_remove_markers(valid_remove_markers.second);
                    if(valid_remove_markers_player.size() == 0)
                    {
                        optimal = max(optimal, min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent));
                    }
                    else
                    {
                        for(int k = 0; k < valid_remove_markers_player.size(); k++)
                        {
                            vector< vector< int > > remove_markers_player = valid_remove_markers_player[k];
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
                            for(int l = 0; l < subset_rings.size(); l++)
                            {
                                vector< pair< int, int > > retained_ring_positions;
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                {
                                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                                }
                                optimal = max(optimal, min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent));
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                            }
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.backtrack_remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                        }
                    }
                    state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                }
            }
        }
    }
    return optimal;
}

double min_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player)
{
    if(depth == 0)
        return state.heuristic(player);

    int max_removable_markers = -1;
    int max_removable_markers_in = -1;
    for(int i = 0; i < all_remove_markers_player.size(); i++)
        if(all_remove_markers_player[i].size() > max_removable_markers)
        {
            max_removable_markers = all_remove_markers_player[i].size();
            max_removable_markers_in = i;
        }

    int num_rings_removed;
    if(player == 1)
        num_rings_removed = state.num_rings_removed1;
    else
        num_rings_removed = state.num_rings_removed2;

    if(max_removable_markers + num_rings_removed >= state.num_rings_win)
        return -numeric_limits<double>::infinity();

    double optimal = numeric_limits<double>::infinity();
    vector< vector< vector< int > > > empty;
    if(all_remove_markers_player.size() != 0)
    {
        for(int i = 0; i < all_remove_markers_player.size(); i++)
        {
            vector< vector< int > > remove_markers_player = all_remove_markers_player[i];
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
            for(int l = 0; l < subset_rings.size(); l++)
            {
                vector< pair< int, int > > retained_ring_positions;
                for(int m = 0; m < remove_markers_player.size(); m++)
                {
                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                }
                optimal = min(optimal, min_value(state, depth, player, empty));
                for(int m = 0; m < remove_markers_player.size(); m++)
                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
            }
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.backtrack_remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
        }
    }
    else
    {
        if(state.get_phase() == 0)
        {
            for(int i = 0; i < state.num_rows; i++)
            {
                pair< int, int > range = state.low_high(i);
                int low = range.first;
                int high = range.second;

                for(int j = low; j <= high; j++)
                {
                    if(state.board[i][j] == 0)
                    {
                        state.place_ring(player, i, j);
                        optimal = min(optimal, max_value(state, depth - 1, 3 - player, empty));
                        state.backtrack_place_ring(player, i, j);
                    }
                }
            }
        }
        else
        {
            for(int i = (player - 1); i < state.ring_map.size(); (i += 2))
            {
                pair<int, int> ring_pos = state.ring_map[i];
                if(ring_pos.first == -1 || ring_pos.second == -1)
                    continue;
                vector< pair< int, int > > valid_ring_pos = state.generate_valid_move_ring(ring_pos.first, ring_pos.second);
                for(int j = 0; j < valid_ring_pos.size(); j++)
                {
                    state.move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    pair< vector< vector < int > >, vector< vector< int > > > valid_remove_markers = state.generate_remove_markers(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    vector< vector< vector < int > > >  valid_remove_markers_player = state.unique_non_intersecting_remove_markers(valid_remove_markers.first);
                    vector< vector< vector < int > > > valid_remove_markers_opponent = state.unique_non_intersecting_remove_markers(valid_remove_markers.second);
                    if(valid_remove_markers_player.size() == 0)
                    {
                        optimal = min(optimal, max_value(state, depth - 1, 3 - player, valid_remove_markers_opponent));
                    }
                    else
                    {
                        for(int k = 0; k < valid_remove_markers_player.size(); k++)
                        {
                            vector< vector< int > > remove_markers_player = valid_remove_markers_player[k];
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
                            for(int l = 0; l < subset_rings.size(); l++)
                            {
                                vector< pair< int, int > > retained_ring_positions;
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                {
                                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                                }
                                optimal = min(optimal, max_value(state, depth - 1, 3 - player, valid_remove_markers_opponent));
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                            }
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.backtrack_remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                        }
                    }
                    state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                }
            }
        }
    }
    return optimal;
}

int main()
{
    int size = 5;
    int num_rings = 5;
    int num_resolution = 5;
    int num_rings_win = 3;

    Board yinsh_board(size, num_rings, num_resolution, num_rings_win);
    yinsh_board.print_board();

    int player = 1;
    int depth = 1;
    vector< vector< vector< int > > > passed_to_my_player;
    int count = 0;
    while(true)
    {
        count++;
        string move;
        if(yinsh_board.phase == 0)
            depth = 2;
        else
            if(player == 1)
                depth = 2;
            else
                depth = 4;

        pair< string, double > minimax_decision_result = minimax_decision(yinsh_board, depth, player, passed_to_my_player, "");
        move = minimax_decision_result.first;
        if(player == 1)
            cout << "Zelex: " << move << "\n";
        else
            cout << "Rayon: " << move << "\n";

        size_t pos = 0;
        string token;

        while(!move.empty())
        {
            pos = move.find(" ");
            token = move.substr(0, pos);
            move.erase(0, pos + 1);

            if(token == "P")
            {
                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position = stoi(token);

                pair< int, int > row_pos = yinsh_board.for_map(hexagon, hex_position);
                int row = row_pos.first;
                int position = row_pos.second;

                yinsh_board.place_ring(player, row, position);
            }
            else if(token == "S")
            {
                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon_start = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position_start = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon_end = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position_end = stoi(token);

                pair< int, int > start = yinsh_board.for_map(hexagon_start, hex_position_start);
                int row_start = start.first;
                int position_start = start.second;
                pair< int, int > end = yinsh_board.for_map(hexagon_end, hex_position_end);
                int row_end = end.first;
                int position_end = end.second;

                yinsh_board.move_ring(player, row_start, position_start, row_end, position_end);
                passed_to_my_player = yinsh_board.unique_non_intersecting_remove_markers(yinsh_board.generate_remove_markers(player, row_start, position_start, row_end, position_end).second);
            }
            else if(token == "RS")
            {
                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon_start = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position_start = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon_end = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position_end = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hexagon_ring = stoi(token);

                pos = move.find(" ");
                token = move.substr(0, pos);
                move.erase(0, pos + 1);
                int hex_position_ring = stoi(token);

                pair< int, int > start = yinsh_board.for_map(hexagon_start, hex_position_start);
                int row_start = start.first;
                int position_start = start.second;
                pair< int, int > end = yinsh_board.for_map(hexagon_end, hex_position_end);
                int row_end = end.first;
                int position_end = end.second;
                pair< int, int > ring = yinsh_board.for_map(hexagon_ring, hex_position_ring);
                int row_ring = ring.first;
                int position_ring = ring.second;

                yinsh_board.remove_markers(player, row_start, position_start, row_end, position_end);
                yinsh_board.remove_ring(player, row_ring, position_ring);
            }
        }
        cout << "\n";
        yinsh_board.print_board();
        cout << "\n";
        player = 3 - player;
        if(yinsh_board.num_rings_removed1 == num_rings_win)
        {
            cout << "Zelex wins the game" << "\n";
            break;
        }
        else if(yinsh_board.num_rings_removed2 == num_rings_win)
        {
            cout << "Rayon wins the game" << "\n";
            break;
        }
        usleep(2);
    }
    return 0;
}
