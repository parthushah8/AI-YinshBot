/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#include "minimax_alpha_beta.h"

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
    // cerr << "Max removable markers:" << max_removable_markers << "\n";
    // cerr << all_remove_markers_player.size() << "\n";
    for(int i = 0; i < all_remove_markers_player.size(); i++)
    {
        // cerr << "Max removable markers uptill now:" << max_removable_markers << "\n";
        // cerr << "Size of first subset " << all_remove_markers_player[i].size() << "\n";
        int temp = all_remove_markers_player[i].size();
        // cerr << "temp : " << temp << "\n";
        // cerr << "max removable markers: " << max_removable_markers << "\n";
        // cerr << "equality boolean: " << (temp > max_removable_markers) << "\n";
        if(temp > max_removable_markers)
        {
            // cerr << "Fck!! it went inside too\n";
            max_removable_markers = temp;
            // cerr << "Max removable markers after changig:" << max_removable_markers << "\n";
            max_removable_markers_in = i;
        }
    }

    int num_rings_removed;
    if(player == 1)
        num_rings_removed = state.num_rings_removed1;
    else
        num_rings_removed = state.num_rings_removed2;

    // cerr << "Max removable markers:" << max_removable_markers << "\n";
    // cerr << "Num rings removed:" << num_rings_removed << "\n";
    // cerr << "NUm rings win:" << state.num_rings_win << "\n";
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

    double optimal = NEG_INF;
    vector< vector< vector< int > > > empty;

    if(all_remove_markers_player.size() != 0)
    {
        // cerr << "all_reomve_markers_player: " << all_remove_markers_player.size() << "\n";
        for(int i = 0; i < all_remove_markers_player.size(); i++)
        {
            vector< vector< int > > remove_markers_player = all_remove_markers_player[i];
            // cerr << "remove_markers_player: " << remove_markers_player.size() << "\n";
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);

            vector< vector< int > > subset_rings = subset(player, state.ring_map, remove_markers_player.size());
            // cerr << "Printing all possible subsets of rings\n";
            // for(int l = 0; l < subset_rings.size() ; l++)
            // {
            //   cerr << "A possible Subset\n";
            //   for(int m = 0; m < subset_rings[l].size() ; m++)
            //       cerr << state.ring_map[subset_rings[l][m]].first << " , " << state.ring_map[subset_rings[l][m]].second << " || ";
            //   cerr << "\n";
            // }
            for(int l = 0; l < subset_rings.size() ; l++)
            {
                vector< pair< int, int > > retained_ring_positions;
                for(int m = 0; m < remove_markers_player.size(); m++)
                {
                    retained_ring_positions.push_back(make_pair(state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second));
                    state.remove_ring(player, state.ring_map[subset_rings[l][m]].first, state.ring_map[subset_rings[l][m]].second);
                }
                // cerr << "After removing a subset of rings he board state we get\n";
                state.print_board();
                pair< string, double> minimax_decision_result = minimax_decision(state, depth, player, empty, "");
                string neigbour_answer = minimax_decision_result.first;
                double neigbour_optimal = minimax_decision_result.second;
                // cerr << "Neighbour Optimum found: " << neigbour_optimal << " and optimal we had was " << optimal << "\n";
                if(neigbour_optimal > optimal)
                {
                    // cerr << "YUPP!! neigbour_optimal > optimal, Now making string\n";
                    optimal = neigbour_optimal;
                    // cerr << "remove_markers_player.size() : " << remove_markers_player.size() << "\n";
                    for(int m = 0; m < remove_markers_player.size(); m++)
                    {
                        // cerr << "String after removing " << m << " rings is " << answer << "\n";
                        pair< int, int > hex_cord1 = state.back_map(remove_markers_player[m][0], remove_markers_player[m][1]);
                        pair< int, int > hex_cord2 = state.back_map(remove_markers_player[m][2], remove_markers_player[m][3]);
                        pair< int, int > removed_ring = state.back_map(retained_ring_positions[m].first, retained_ring_positions[m].second);
                        string temp = "";
                        temp += "RS " + to_string(hex_cord1.first) + " " + to_string(hex_cord1.second) + " ";
                        temp += "RE " + to_string(hex_cord2.first) + " " + to_string(hex_cord2.second) + " ";
                        temp += "X " + to_string(removed_ring.first) + " " + to_string(removed_ring.second) + " ";
                        answer = temp + neigbour_answer;
                    }
                    // cerr << "ANSWER:" << answer << "\n";
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
                        double neigbour_optimal = min_value(state, depth - 1, 3 - player, empty, NEG_INF, POS_INF);
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
                // cerr << "1\n";
                vector< pair< int, int > > valid_ring_pos = state.generate_valid_move_ring(ring_pos.first, ring_pos.second);
                for(int j = 0; j < valid_ring_pos.size(); j++)
                {
                    state.move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    // cerr << "burrabench\n";
                    pair< vector< vector < int > >, vector< vector< int > > > valid_remove_markers = state.generate_remove_markers(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    // cerr << "nkfammsfka\n";
                    vector< vector< vector < int > > >  valid_remove_markers_player = state.unique_non_intersecting_remove_markers(valid_remove_markers.first);
                    vector< vector< vector < int > > > valid_remove_markers_opponent = state.unique_non_intersecting_remove_markers(valid_remove_markers.second);
                    // cerr << "burramskmka\n";
                    // cerr << valid_remove_markers_player.size() << "\n";
                    // for(int k = 0; k < valid_remove_markers_player.size();k++)
                    // {
                    //     for(int l = 0; l < valid_remove_markers_player[k].size(); l++)
                    //         cerr << valid_remove_markers_player[k][l][0] << " " << valid_remove_markers_player[k][l][1] << " " << valid_remove_markers_player[k][l][2] << " " << valid_remove_markers_player[k][l][3] << ",  ";
                    //     cerr << "\n";
                    // }
                    if(valid_remove_markers_player.size() == 0)
                    {
                        double neigbour_optimal = min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, NEG_INF, POS_INF);
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
                                double neigbour_optimal = min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, NEG_INF, POS_INF);
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

double max_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, double alpha, double beta)
{
    if(depth == 0)
        return state.heuristic();

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
        return POS_INF;

    double optimal = NEG_INF;
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
                optimal = max(optimal, max_value(state, depth, player, empty, alpha, beta));
                alpha = max(alpha, optimal);
                for(int m = 0; m < remove_markers_player.size(); m++)
                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                if(alpha >= beta)
                    break;
            }
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.backtrack_remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
            if(alpha >= beta)
                break;
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
                        optimal = max(optimal, min_value(state, depth - 1, 3 - player, empty, alpha, beta));
                        alpha = max(alpha, optimal);
                        state.backtrack_place_ring(player, i, j);
                        if(alpha >= beta)
                            break;
                    }
                }
                if(alpha >= beta)
                    break;
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
                        optimal = max(optimal, min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, alpha, beta));
                        alpha = max(alpha, optimal);
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
                                optimal = max(optimal, min_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, alpha, beta));
                                alpha = max(alpha, optimal);
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                                if(alpha >= beta)
                                    break;
                            }

                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.backtrack_remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                            if(alpha >= beta)
                                break;
                        }
                    }
                    state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    if(alpha >= beta)
                        break;
                }
                if(alpha >= beta)
                    break;
            }
        }
    }

    return optimal;
}

double min_value(Board state, int depth, int player, vector< vector< vector< int > > > all_remove_markers_player, double alpha, double beta)
{
    if(depth == 0)
        return state.heuristic();

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
        return NEG_INF;

    double optimal = POS_INF;
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
                optimal = min(optimal, min_value(state, depth, player, empty, alpha, beta));
                beta = min(beta, optimal);
                for(int m = 0; m < remove_markers_player.size(); m++)
                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                if(alpha >= beta)
                    break;
            }
            for(int j = 0; j < remove_markers_player.size(); j++)
                state.backtrack_remove_markers(player, remove_markers_player[j][0], remove_markers_player[j][1], remove_markers_player[j][2], remove_markers_player[j][3]);
            if(alpha >= beta)
                break;
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
                        optimal = min(optimal, max_value(state, depth - 1, 3 - player, empty, alpha, beta));
                        beta = min(beta, optimal);
                        state.backtrack_place_ring(player, i, j);
                        if(alpha >= beta)
                            break;
                    }
                }
                if(alpha >= beta)
                    break;
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
                        optimal = min(optimal, max_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, alpha, beta));
                        beta = min(beta, optimal);
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
                                optimal = min(optimal, max_value(state, depth - 1, 3 - player, valid_remove_markers_opponent, alpha, beta));
                                beta = min(beta, optimal);
                                for(int m = 0; m < remove_markers_player.size(); m++)
                                    state.backtrack_remove_ring(player, subset_rings[l][m], retained_ring_positions[m].first, retained_ring_positions[m].second);
                                if(alpha >= beta)
                                    break;
                            }
                            for(int l = 0; l < remove_markers_player.size(); l++)
                                state.backtrack_remove_markers(player, remove_markers_player[l][0], remove_markers_player[l][1], remove_markers_player[l][2], remove_markers_player[l][3]);
                            if(alpha >= beta)
                                break;
                        }
                    }
                    state.backtrack_move_ring(player, ring_pos.first, ring_pos.second, valid_ring_pos[j].first, valid_ring_pos[j].second);
                    if(alpha >= beta)
                        break;
                }
                if(alpha >= beta)
                    break;
            }
        }
    }

    return optimal;
}
