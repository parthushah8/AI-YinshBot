/*
* Authors: Parth Shah
*        : Shashank Goel
*/

#include "Board.h"

Board::Board(int size, int num_rings, int num_resolution, int num_rings_win, int first_player)
{
    this -> size = size;
    this -> num_rows = (2 * size + 1);
    this -> num_rings = num_rings;
    this -> num_resolution = num_resolution;
    this -> num_rings_win = num_rings_win;
    this -> ring_counter[0] = 3;
    this -> ring_counter[1] = 4;
    this -> phase = 0;
    this -> first_player = first_player;

    for (int i = 0; i < num_rows; i++)
    {
        vector<int> row_temp;
        pair< int, int > range = low_high(i);
        for(int j = 0; j < num_rows; j++)
        {
            if((j >= range.first) && (j <= range.second))
                row_temp.push_back(0);
            else
                row_temp.push_back(-1);
        }
        this -> board.push_back(row_temp);
    }

    for (int i = 0; i < 2 * num_rings; i++)
        this -> ring_map.push_back(make_pair(-1, -1));

    this -> num_rings_removed1 = 0;
    this -> num_rings_removed2 = 0;
    this -> num_markers_1 = 0;
    this -> num_markers_2 = 0;
}

pair< int, int > Board::low_high(int row)
{
      int x = row - size;
      int low = 0;
      int high = 2 * size;

      if(x == 0)
      {
          low = 1;
          high = 2 * size - 1;
      }
      else if((x >= 1) && (x <= (size - 1)))
      {
          low = x;
      }
      else if(x == size)
      {
          low = size + 1;
          high = 2 * size - 1;
      }
      else if((x >= -(size - 1)) && (x <= -1))
      {
          high = 2 * size + x;
      }
      else if(x == -size)
      {
          low = 1;
          high = size - 1;
      }

      return make_pair(low, high);
}

pair< int, int > Board::for_map(int hexagon, int hex_position)
{
    if(hexagon == 0)
        return make_pair(size, size);

    int quo = hex_position / hexagon;
    int rem = hex_position % hexagon;
    int row;
    int position;

    if(quo == 0)
    {
        row = rem;
        position = hexagon;
    }
    else if(quo == 1)
    {
        row = hexagon;
        position = hexagon - rem;
    }
    else if(quo == 2)
    {
        row = hexagon - rem;
        position = -rem;
    }
    else if(quo == 3)
    {
        row = -rem;
        position = -hexagon;
    }
    else if(quo == 4)
    {
        row = -hexagon;
        position = -hexagon + rem;
    }
    else
    {
        row = -hexagon + rem;
        position = rem;
    }

    row += size;
    position += size;

    return make_pair(row, position);
}

pair< int, int > Board::back_map(int row, int position)
{
    if((row == size) && (position == size))
        return make_pair(0, 0);

    int quo = 0;
    int shifted_row = row - size;
    int shifted_position = position - size;

    if((shifted_row >= 0) && (shifted_position > shifted_row))
        quo = 0;
    else if((shifted_position > 0) && (shifted_row >= shifted_position))
        quo = 1;
    else if((shifted_row > 0) && (shifted_position <= 0))
        quo = 2;
    else if((shifted_row <= 0) && (shifted_position < shifted_row))
        quo = 3;
    else if((shifted_position < 0) && (shifted_row <= shifted_position))
        quo = 4;
    else
        quo = 5;

    int hexagon = 0;
    int hex_position = 0;

    if(quo == 0)
    {
         hexagon = shifted_position;
         hex_position = shifted_row;
      }
    else if(quo == 1)
    {
        hexagon = shifted_row;
        hex_position = shifted_row - shifted_position;
    }
    else if(quo == 2)
    {
        hexagon = shifted_row - shifted_position;
        hex_position = -shifted_position;
    }
    else if(quo == 3)
    {
        hexagon = -shifted_position;
        hex_position = -shifted_row;
    }
    else if(quo == 4)
    {
        hexagon = -shifted_row;
        hex_position =  shifted_position - shifted_row;
    }
    else
    {
        hexagon = -shifted_row + shifted_position;
        hex_position = shifted_position;
    }

    hex_position += quo * hexagon;

    return make_pair(hexagon, hex_position);
}

void Board::place_ring(int player, int row, int position)
{
    board[row][position] = ring_counter[player - 1];
    ring_map[board[row][position] - 3] = make_pair(row, position);
    ring_counter[player - 1] += 2;
    if(ring_counter[1] == (2 * num_rings + 4))
        phase = 1;
}

void Board::backtrack_place_ring(int player, int row, int position)
{
    ring_map[(board[row][position] - 3)] = make_pair(-1, -1);
    board[row][position] = 0;
    ring_counter[player - 1] -= 2;
    phase = 0;
}

void Board::remove_ring(int player, int row, int position)
{
    ring_map[(board[row][position] - 3)] = make_pair(-1, -1);
    board[row][position] = 0;
    ring_counter[player - 1] -= 2;
    if(player == 1)
        num_rings_removed1++;
    else
        num_rings_removed2++;
}

void Board::backtrack_remove_ring(int player, int ring_number, int row, int position)
{
    board[row][position] = ring_number + 3;
    ring_map[ring_number] = make_pair(row, position);
    ring_counter[player - 1] += 2;
    if(player == 1)
        num_rings_removed1--;
    else
        num_rings_removed2--;
}

void Board::move_ring(int player, int row1, int position1, int row2, int position2)
{
    if(row1 == row2)
    {
        int start = min(position1, position2);
        int end = max(position1, position2);
        for(int i = (start + 1); i < end; i++)
            if(board[row1][i] == 1)
            {
                board[row1][i] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[row1][i] == 2)
            {
                board[row1][i] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }
    else if(position1 == position2)
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        for(int i = (start + 1); i < end; i++)
            if(board[i][position1] == 1)
            {
                board[i][position1] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[i][position1] == 2)
            {
                board[i][position1] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }
    else
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        int difference = row1 - position1;
        for(int i = (start + 1); i < end; i++)
            if(board[i][i - difference] == 1)
            {
                board[i][i - difference] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[i][i - difference] == 2)
            {
                board[i][i - difference] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }

    int ring_number = board[row1][position1];
    board[row1][position1] = player;
    board[row2][position2] = ring_number;
    ring_map[ring_number - 3] = make_pair(row2, position2);
    if(player == 1)
        num_markers_1++;
    else
        num_markers_2++;
}

void Board::backtrack_move_ring(int player, int row1, int position1, int row2, int position2)
{
    if(row1 == row2)
    {
        int start = min(position1, position2);
        int end = max(position1, position2);
        for(int i = (start + 1); i < end; i++)
            if(board[row1][i] == 1)
            {
                board[row1][i] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[row1][i] == 2)
            {
                board[row1][i] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }
    else if(position1 == position2)
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        for(int i = (start + 1); i < end; i++)
            if(board[i][position1] == 1)
            {
                board[i][position1] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[i][position1] == 2)
            {
                board[i][position1] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }
    else
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        int difference = row1 - position1;
        for(int i = (start + 1); i < end; i++)
            if(board[i][i - difference] == 1)
            {
                board[i][i - difference] = 2;
                num_markers_1--;
                num_markers_2++;
            }
            else if(board[i][i - difference] == 2)
            {
                board[i][i - difference] = 1;
                num_markers_1++;
                num_markers_2--;
            }
    }

    int ring_number = board[row2][position2];
    board[row2][position2] = 0;
    board[row1][position1] = ring_number;
    ring_map[ring_number - 3] = make_pair(row1, position1);
    if(player == 1)
        num_markers_1--;
    else
        num_markers_2--;
}

void Board::remove_markers(int player, int row1, int position1, int row2, int position2)
{
    if(row1 == row2)
    {
        int start = min(position1, position2);
        int end = max(position1, position2);
        for(int i = start; i <= end; i++)
            board[row1][i] = 0;
    }
    else if(position1 == position2)
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        for(int i = start; i <= end; i++)
            board[i][position1] = 0;
    }
    else
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        int difference = row1 - position1;
        for(int i = start; i <= end; i++)
            board[i][i - difference] = 0;
    }
    if(player == 1)
        num_markers_1 -= num_resolution;
    else
        num_markers_2 -= num_resolution;
}

void Board::backtrack_remove_markers(int player, int row1, int position1, int row2, int position2)
{
    if(row1 == row2)
    {
        int start = min(position1, position2);
        int end = max(position1, position2);
        for(int i = start; i <= end; i++)
            board[row1][i] = player;
    }
    else if(position1 == position2)
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        for(int i = start; i <= end; i++)
            board[i][position1] = player;
    }
    else
    {
        int start = min(row1, row2);
        int end = max(row1, row2);
        int difference = row1 - position1;
        for(int i = start; i <= end; i++)
            board[i][i - difference] = player;
    }
    if(player == 1)
        num_markers_1 += num_resolution;
    else
        num_markers_2 += num_resolution;
}

vector< pair< int, int > > Board::generate_valid_move_ring(int row, int position)
{
    vector< pair< int, int > > answer;

    pair< int, int > range;
    int low;
    int high;
    int i;
    int difference;

    range = low_high(row);
    i = position + 1;
    high = range.second;
    for(; ((i <= high) && (board[row][i] == 0)); i++)
        answer.push_back(make_pair(row, i));
    while((i <= high) && ((board[row][i] == 1) || (board[row][i] == 2)))
        i++;
    if((i <= high) && (board[row][i] == 0))
        answer.push_back(make_pair(row, i));

    range = low_high(row + 1);
    i = row + 1;
    high = range.second;
    difference = row - position;
    for(; ((i < num_rows) && (i - difference <= high) && (board[i][i - difference] == 0)); i++)
    {
        high = low_high(i + 1).second;
        answer.push_back(make_pair(i, i - difference));
    }
    for(; ((i < num_rows) && (i - difference <= high) && ((board[i][i - difference] == 1) || (board[i][i - difference] == 2))) ; i++)
        high = low_high(i + 1).second;
    if((i < num_rows) && (i - difference <= high) && (board[i][i - difference] == 0))
        answer.push_back(make_pair(i, i - difference));

    range = low_high(row + 1);
    i = row + 1;
    low = range.first;
    for(;((i < num_rows) && (position >= low) && (board[i][position] == 0)); i++)
    {
        low = low_high(i + 1).first;
        answer.push_back(make_pair(i, position));
    }
    for(; ((i < num_rows) && (position >= low) && ((board[i][position] == 1) || (board[i][position] == 2))); i++)
        low = low_high(i + 1).first;
    if((position >= low) && (i < num_rows) && (board[i][position] == 0))
        answer.push_back(make_pair(i, position));

    range = low_high(row);
    i = position - 1;
    low = range.first;
    for(; ((i >= low) && (board[row][i] == 0)); i--)
        answer.push_back(make_pair(row, i));
    while((i >= low) && (board[row][i] == 1) || (board[row][i] == 2))
        i--;
    if((i >= low) && (board[row][i] == 0))
        answer.push_back(make_pair(row, i));

    range = low_high(row - 1);
    i = row - 1;
    low = range.first;
    difference = row - position;
    for(; ((i >= 0) && (i - difference >= low) && (board[i][i - difference] == 0)); i--)
    {
        low = low_high(i - 1).first;
        answer.push_back(make_pair(i, i - difference));
    }
    for(; ((i >= 0) && (i - difference >= low) && ((board[i][i - difference] == 1) || (board[i][i - difference] == 2))); i--)
        low = low_high(i - 1).first;
    if((i >= 0) && (i - difference >= low) && (board[i][i - difference] == 0))
        answer.push_back(make_pair(i, i - difference));

    range = low_high(row - 1);
    i = row - 1;
    high = range.second;
    for(;((i >= 0) && (position <= high) && board[i][position] == 0); i--)
    {
        high = low_high(i - 1).second;
        answer.push_back(make_pair(i, position));
    }
    for(; ((i >= 0) && (position <= high) && ((board[i][position] == 1) || (board[i][position] == 2))); i--)
        high = low_high(i - 1).second;
    if((position <= high) && (i >= 0) && (board[i][position] == 0))
        answer.push_back(make_pair(i, position));

    return answer;
}

pair< vector< vector< int > > , vector< vector< int > > > Board::generate_remove_markers(int player, int row1, int position1, int row2, int position2)
{
    int count_front;
    int count_back;
    pair< int, int > range;
    int low;
    int high;

    vector< vector< int > > remove_markers_player;
    vector< vector< int > > remove_markers_opponent;

    if(row1 == row2)
    {
        int c = 1;
        if(position1 > position2)
            c = -1;
        for(int itr = position1; (itr - position2) * c < 0; itr+=c)
        {
            vector< vector< int > > cross_remove_markers;

            if(board[row1][itr] != 0)
            {
                int row = row1;
                int position = itr;
                int color = board[row][position];

                count_front = 0;
                range = low_high(row + 1);
                low = range.first;
                high = range.second;
                for(int i = row + 1; ((i < num_rows) && (position >= low && position <= high) && (board[i][position] == color)); i++)
                {
                    range = low_high(i + 1);
                    low = range.first;
                    high = range.second;
                    count_front++;
                }
                count_back = 0;
                range = low_high(row - 1);
                low = range.first;
                high = range.second;
                for(int i = row - 1; ((i >= 0) && (position >= low && position <= high) && (board[i][position] == color)); i--)
                {
                    range = low_high(i - 1);
                    low = range.first;
                    high = range.second;
                    count_back++;
                }
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = row - count_back; (i + num_resolution) <= row + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(i); row_pos.push_back(position);
                        row_pos.push_back(i + num_resolution - 1); row_pos.push_back(position);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }

                int difference = row - position;

                count_front = 0;
                range = low_high(row + 1);
                low = range.first;
                high = range.second;
                for(int i = row + 1; ((i < num_rows) && (i - difference >= low && i - difference <= high) && (board[i][i - difference] == color)); i++)
                {
                    range = low_high(i + 1);
                    low = range.first;
                    high = range.second;
                    count_front++;
                }
                count_back = 0;
                range = low_high(row - 1);
                low = range.first;
                high = range.second;
                for(int i = row - 1; ((i >= 0) && (i - difference >= low && i - difference <= high) && (board[i][i - difference] == color)); i--)
                {
                    range = low_high(i - 1);
                    low = range.first;
                    high = range.second;
                    count_back++;
                }
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = row - count_back; (i + num_resolution) <= row + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(i); row_pos.push_back(i - difference);
                        row_pos.push_back(i + num_resolution - 1); row_pos.push_back(i - difference + num_resolution - 1);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }
            }
        }

        int row = row1;
        range = low_high(row);
        low = range.first;
        high = range.second;
        int counter_player = 0;
        int counter_opponent = 0;
        for(int i = low; i <= high; i++)
        {
            if(board[row][i] == player)
                counter_player++;
            else
                counter_player = 0;
            if(board[row][i] == 3 - player)
                counter_opponent++;
            else
                counter_opponent = 0;
            if(counter_player >= num_resolution)
            {
                vector<int> row_pos;
                row_pos.push_back(row); row_pos.push_back(i - num_resolution + 1);
                row_pos.push_back(row); row_pos.push_back(i);
                remove_markers_player.push_back(row_pos);
            }
            if(counter_opponent >= num_resolution)
            {
                vector<int> row_pos;
                row_pos.push_back(row); row_pos.push_back(i - num_resolution + 1);
                row_pos.push_back(row); row_pos.push_back(i);
                remove_markers_opponent.push_back(row_pos);
            }
        }

        return make_pair(remove_markers_player, remove_markers_opponent);
    }
    else if(position1 == position2)
    {
        int c = 1;
        if(row1 > row2)
            c = -1;
        for(int itr = row1; (itr - row2) * c < 0; itr+=c)
        {
            vector< vector< int > > cross_remove_markers;

            if(board[itr][position1] != 0)
            {
                int row = itr;
                int position = position1;
                int color = board[row][position];

                count_front = 0;
                count_back = 0;
                range = low_high(row);
                low = range.first;
                high = range.second;
                for(int i = position + 1; ((i < min(position + num_resolution, high)) && (board[row][i] == color)); i++)
                    count_front++;
                for(int i = position - 1; ((i > max(position - num_resolution, low)) && (board[row][i] == color)); i--)
                    count_back++;
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = (position - count_back); (i + num_resolution) <= position + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(row); row_pos.push_back(i);
                        row_pos.push_back(row); row_pos.push_back(i + num_resolution - 1);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }

                count_front = 0;
                range = low_high(row + 1);
                low = range.first;
                high = range.second;
                int difference = row - position;
                for(int i = row + 1; ((i < num_rows) && (i - difference >= low && i - difference <= high) && (board[i][i - difference] == color)); i++)
                {
                    range = low_high(i + 1);
                    low = range.first;
                    high = range.second;
                    count_front++;
                }
                count_back = 0;
                range = low_high(row - 1);
                low = range.first;
                high = range.second;
                for(int i = row - 1; ((i >= 0) && (i - difference >= low && i - difference <= high) && (board[i][i - difference] == color)); i--)
                {
                    range = low_high(i - 1);
                    low = range.first;
                    high = range.second;
                    count_back++;
                }
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = row - count_back; (i + num_resolution) <= row + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(i); row_pos.push_back(i - difference);
                        row_pos.push_back(i + num_resolution - 1); row_pos.push_back(i - difference + num_resolution - 1);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }
            }

        }

        int position = position1;
        int counter_player = 0;
        int counter_opponent = 0;
        for(int i = 0; i < num_rows; i++)
        {
            range = low_high(i);
            low = range.first; high = range.second;
            if(position >= low && position <= high)
            {
                if(board[i][position] == player)
                    counter_player++;
                else
                    counter_player = 0;
                if(board[i][position] == 3 - player)
                    counter_opponent++;
                else
                    counter_opponent = 0;

                if(counter_player >= num_resolution)
                {
                    vector<int> row_pos;
                    row_pos.push_back(i - num_resolution + 1); row_pos.push_back(position);
                    row_pos.push_back(i); row_pos.push_back(position);
                    remove_markers_player.push_back(row_pos);
                }
                if(counter_opponent >= num_resolution)
                {
                    vector<int> row_pos;
                    row_pos.push_back(i - num_resolution + 1); row_pos.push_back(position);
                    row_pos.push_back(i); row_pos.push_back(position);
                    remove_markers_opponent.push_back(row_pos);
                }
            }
        }

        return make_pair(remove_markers_player, remove_markers_opponent);
    }
    else
    {
        int c = 1;
        int difference = row1 - position1;
        if(row1 > row2)
            c = -1;
        for(int itr = row1; (itr - row2) * c < 0; itr+=c)
        {
            vector< vector< int > > cross_remove_markers;

            if(board[itr][itr - difference] != 0)
            {
                int row = itr;
                int position = itr - difference;
                int color = board[row][position];

                count_front = 0;
                count_back = 0;
                range = low_high(row);
                low = range.first;
                high = range.second;
                for(int i = position + 1; ((i < min(position + num_resolution, high)) && (board[row][i] == color)); i++)
                    count_front++;
                for(int i = position - 1; ((i > max(position - num_resolution, low)) && (board[row][i] == color)); i--)
                    count_back++;
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = (position - count_back); (i + num_resolution) <= position + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(row); row_pos.push_back(i);
                        row_pos.push_back(row); row_pos.push_back(i + num_resolution - 1);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }

                count_front = 0;
                range = low_high(row + 1);
                low = range.first;
                high = range.second;
                for(int i = row + 1; ((i < num_rows) && (position >= low && position <= high) && (board[i][position] == color)); i++)
                {
                    range = low_high(i + 1);
                    low = range.first;
                    high = range.second;
                    count_front++;
                }

                count_back = 0;
                range = low_high(row - 1);
                low = range.first;
                high = range.second;
                for(int i = row - 1; ((i >= 0) && (position >= low && position <= high) && (board[i][position] == color)); i--)
                {
                    range = low_high(i - 1);
                    low = range.first;
                    high = range.second;
                    count_back++;
                }
                if(count_front + count_back + 1 >= num_resolution)
                {
                    for(int i = row - count_back; (i + num_resolution) <= row + count_front + 1; i++)
                    {
                        vector<int> row_pos;
                        row_pos.push_back(i); row_pos.push_back(position);
                        row_pos.push_back(i + num_resolution - 1); row_pos.push_back(position);
                        if(board[row][position] == player)
                            remove_markers_player.push_back(row_pos);
                        else
                            remove_markers_opponent.push_back(row_pos);
                    }
                }
            }

        }

        int row = row1;
        int position = position1;
        difference = row - position;
        int counter_player = 0;
        int counter_opponent = 0;
        for(int i = 0; i < num_rows; i++)
        {
            range = low_high(i);
            low = range.first; high = range.second;
            if(i - difference >= low && i - difference <= high)
            {
                if(board[i][i - difference] == player)
                    counter_player++;
                else
                    counter_player = 0;
                if(board[i][i - difference] == 3 - player)
                    counter_opponent++;
                else
                    counter_opponent = 0;

                if(counter_player >= num_resolution)
                {
                    vector<int> row_pos;
                    row_pos.push_back(i - num_resolution + 1); row_pos.push_back(i - num_resolution + 1 - difference);
                    row_pos.push_back(i); row_pos.push_back(i - difference);
                    remove_markers_player.push_back(row_pos);
                }
                if(counter_opponent >= num_resolution)
                {
                    vector<int> row_pos;
                    row_pos.push_back(i - num_resolution + 1); row_pos.push_back(i - num_resolution + 1 - difference);
                    row_pos.push_back(i); row_pos.push_back(i - difference);
                    remove_markers_opponent.push_back(row_pos);
                }
            }
        }
        return make_pair(remove_markers_player, remove_markers_opponent);
    }
}

bool Board::intersect(int row1s, int position1s, int row1e, int position1e, int row2s, int position2s, int row2e, int position2e)
{
      if(row1s == row1e)
      {
          if(row2s == row2e)
          {
              if(row1s == row2s)
              {
                  int min1 = min(position1s, position1e);
                  int max1 = max(position1s, position1e);
                  int min2 = min(position2s, position2e);
                  int max2 = max(position2s, position2e);
                  if(((min1 >= min2) && (min1 <= max2)) || ((min2 >= min1) && (min2 <= max1)))
                      return true;
                  else
                      return false;
              }
              else
              {
                  return false;
              }
          }
          else if(position2s == position2e)
          {
              pair< int, int > range = low_high(row1s);
              int low = range.first;
              int high = range.second;
              if((position2s >= min(position1s, position1e)) && (position2s <= max(position1s, position1e)) && (row1s >= min(row2s, row2e)) && (row1s <= max(row2s, row2e)))
                  return true;
              else
                  return false;
          }
          else
          {
              int difference = row2s - position2s;
              if((row1s - difference >= min(position1s, position1e)) && (row1s - difference <= max(position1s, position1e)) && (row1s >= min(row2s, row2e)) && (row1s <= max(row2s, row2e)))
                  return true;
              else
                  return false;
          }
      }
      else if(position1s == position1e)
      {
          if(row2s == row2e)
          {
              return intersect(row2s, position2s, row2e, position2e, row1s, position1s, row1e, position1e);
          }
          else if(position2s == position2e)
          {
              if(position1s == position1e)
              {
                  int min1 = min(row1s, row1e);
                  int max1 = max(row1s, row1e);
                  int min2 = min(row2s, row2e);
                  int max2 = max(row2s, row2e);
                  if(((min1 >= min2) && (min1 <= max2)) || ((min2 >= min1) && (min2 <= max1)))
                      return true;
                  else
                      return false;
              }
              else
              {
                  return false;
              }
          }
          else
          {
              int difference = row2s - position2s;
              if((position1s + difference >= min(row1s, row1e)) && (position1s + difference <= max(row1s, row1e)) && (position1s >= min(position2s, position2e)) && (position1s <= max(position2s, position2e)))
                  return true;
              else
                  return false;
          }
      }
      else
      {
          if(row2s == row2e)
          {
              return intersect(row2s, position2s, row2e, position2e, row1s, position1s, row1e, position1e);
          }
          else if(position2s == position2e)
          {
              return intersect(row2s, position2s, row2e, position2e, row1s, position1s, row1e, position1e);
          }
          else
          {
              if(row1s - position1s == row2e - position2e)
              {
                  int min1 = min(row1s, row1e);
                  int max1 = max(row1s, row1e);
                  int min2 = min(row2s, row2e);
                  int max2 = max(row2s, row2e);
                  if(((min1 >= min2) && (min1 <= max2)) || ((min2 >= min1) && (min2 <= max1)))
                      return true;
                  else
                      return false;
              }
              else
              {
                  return false;
              }
          }
      }
}

vector< vector< vector< int > > > Board::unique_non_intersecting_remove_markers(vector< vector< int > > remove_markers_player)
{
    vector < vector< vector< int > > > answer;
    vector < vector< vector< int > > > answer1;
    vector < vector< vector< int > > > answer2;
    if(remove_markers_player.size() == 0)
        return answer;
    else
    {
        vector< int > removed_0 = remove_markers_player[0];
        remove_markers_player.erase(remove_markers_player.begin());
        answer1 = unique_non_intersecting_remove_markers(remove_markers_player);
        vector< vector< int > > non_intersecting_remove_markers;
        for(int i = 0; i < remove_markers_player.size(); i++)
            if(!intersect(removed_0[0], removed_0[1], removed_0[2], removed_0[3], remove_markers_player[i][0], remove_markers_player[i][1], remove_markers_player[i][2], remove_markers_player[i][3]))
                non_intersecting_remove_markers.push_back(remove_markers_player[i]);

        answer2 = unique_non_intersecting_remove_markers(non_intersecting_remove_markers);
        if(answer2.size() == 0)
        {
            vector< vector < int > > single;
            single.push_back(removed_0);
            answer2.push_back(single);
        }
        else
        {
            for(int i = 0; i < answer2.size(); i++)
                answer2[i].push_back(removed_0);
        }
    }
    answer1.insert(answer1.end(), answer2.begin(), answer2.end());
    return answer1;
}

int Board::get_phase()
{
  return phase;
}

double Board::heuristic()
{
    int player = first_player;
    double utility = 0.0;
    double weight1 = 1;
    double weight2 = 10;

    if(phase == 0)
    {
        for(int i = 0; i < ring_map.size(); i++)
        {
            if(ring_map[i].first == -1)
                continue;
            pair< int, int > range = low_high(ring_map[i].first);
            double temp = back_map(ring_map[i].first, ring_map[i].second).first;
            if(i%2 != player%2)
                utility += temp;
            else
                utility -= temp;
        }
        utility = -utility;
    }
    else
    {
        double utility1 = (2 * player - 3) * (num_markers_2 - num_markers_1);
        double utility2 = (2 * player - 3) * (num_rings_removed2 - num_rings_removed1);
        utility = utility1 * weight1 + utility2 * weight2;
    }
    return utility;
}

vector< vector < vector < int > > > Board::play_move(string move, int player)
{
    size_t pos = 0;
    string token;
    vector < vector < vector < int > > > passed_to_my_player;

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

            pair< int, int > row_pos = for_map(hexagon, hex_position);
            int row = row_pos.first;
            int position = row_pos.second;

            place_ring(player, row, position);
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

            pair< int, int > start = for_map(hexagon_start, hex_position_start);
            int row_start = start.first;
            int position_start = start.second;
            pair< int, int > end = for_map(hexagon_end, hex_position_end);
            int row_end = end.first;
            int position_end = end.second;

            move_ring(player, row_start, position_start, row_end, position_end);
            passed_to_my_player = unique_non_intersecting_remove_markers(generate_remove_markers(player, row_start, position_start, row_end, position_end).second);
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

            pair< int, int > start = for_map(hexagon_start, hex_position_start);
            int row_start = start.first;
            int position_start = start.second;
            pair< int, int > end = for_map(hexagon_end, hex_position_end);
            int row_end = end.first;
            int position_end = end.second;
            pair< int, int > ring = for_map(hexagon_ring, hex_position_ring);
            int row_ring = ring.first;
            int position_ring = ring.second;

            remove_markers(player, row_start, position_start, row_end, position_end);
            remove_ring(player, row_ring, position_ring);
        }
    }
    return passed_to_my_player;
}

void Board::print_board()
{
    int count;
    map< int, char > printKey;
    printKey[0] = 'O';
    printKey[1] = '+';
    printKey[2] = '-';
    for( int i = 3; i <= (2 * num_rings + 2); i++)
        printKey[i] = '0' + (2 - (i%2));
    // first (size - 1) rows(literally)
    for(int i = 0; i < size - 1; i++)
    {
        int count = 2 * size - 1 - i;
        for(int j = 0; j < num_rows; j++)
            if((j >= size - 1 - i) && (j <= size + 1 + i) && (j % 2 == i % 2))
                cerr << "  " << printKey[board[j][count++]];
            else
                cerr << "   ";
        cerr << "\n";
    }

    // the next adhoc row(literally)
    count = size + 1;
    for(int j = 0; j < num_rows - 2; j++)
        if((j != 0) && (j % 2 != size % 2))
            cerr << "  " << printKey[board[j][count++]];
        else
            cerr << "   ";
    cerr << "\n";

    // the center (2 * size - 1) rows(literally)
    for(int i = 0; i < 2 * size - 1; i++)
    {
        int count = size - (i + 1) / 2;
        for(int j = 0; j < num_rows; j++)
            if(j % 2 != i % 2)
                cerr << "  " << printKey[board[j][count++]];
            else
                cerr << "   ";
        cerr << "\n";
    }

    // the next adhoc row(literally)
    count = 1;
    for(int j = 0; j < num_rows - 2; j++)
        if((j != 0) && (j % 2 != size % 2))
            cerr << "  " << printKey[board[j][count++]];
        else
            cerr << "   ";
    cerr << "\n";

    // last (size - 1) rows(literally)
    for(int i = size - 2; i >= 0; i--)
    {
        int count = 0;
        for(int j = 0; j < num_rows; j++)
            if((j >= size - 1 - i) && (j <= size + 1 + i) && (j % 2 == i % 2))
                cerr << "  " << printKey[board[j][count++]];
            else
                cerr << "   ";
        cerr << "\n";
    }
}
