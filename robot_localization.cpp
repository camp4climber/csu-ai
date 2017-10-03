#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>

#define E 1
#define W 2
#define WE 3
#define S 4
#define SE 5
#define SW 6
#define SWE 7
#define N 8
#define NE 9
#define NW 10
#define NWE 11
#define NS 12
#define NSE 13
#define NSW 14
#define NSWE 15


using namespace std;

int convert_observation(string obs);
double move_probability(vector <vector <int> > grid, int cell);
bool can_move(vector <vector <int> > grid, int from, int to);
string convert_int_to_binary_string(int num);
double calculate_observation_probability(vector <vector <int> > grid, int observation, int cell, double error);


int main(int argc, char *argv[])
{
    double sensory_error = atof(argv[2]);    
    string input_file = argv[1];
    int sequence_size = argc-3;
    int number_of_cells = 0;
    int number_of_cells_minus_15 = 0;
    double starting_probability = 0;
    double transition_probability = 0;
    vector <int> sequence_of_observations; 
    vector <vector <int> > gridworld;
    vector <double> start_position_matrix;
    vector <vector <double> > transitional_matrix;
    vector <vector <double> > transposed_matrix;
    vector <double> joint_matrix;
    vector <vector <double> > observational_matrix;
    vector <vector <double> > z_matrix;
    vector <double> w_matrix;
    vector <double> final_joint_matrix;

    //open input file
    ifstream input;
    input.open(input_file);
    
    //build gridworld matrix
    //also count total number of cells, ignoring 15
    if (input)
    {
        string line;
        int value;

        while(getline(input, line))
        {
            gridworld.push_back(vector <int>());
            stringstream split(line);
            while (split >> value)
            {
                gridworld.back().push_back(value);
                if (value != 15) number_of_cells_minus_15++;
            }
        }
    }
    
    input.close();

    //get sequence of observations
    for (int i = 0; i < sequence_size; i++)
    {
        sequence_of_observations.push_back(convert_observation(argv[i+3]));
    }

    //get start probablity
    starting_probability = 1.0/number_of_cells_minus_15;
    
    //build starting position matrix
    //1 column. ignore 15s.
    for (int i=0; i < gridworld.size(); i++)
    {
        for (int j=0; j < gridworld[i].size(); j++)
        {
            if (gridworld[i][j] == 15)
                start_position_matrix.push_back(0);
            else
                start_position_matrix.push_back(starting_probability);
        }
    }


    
    number_of_cells = gridworld.size() * gridworld[0].size();
    //build transitional matrix
    for (int i = 0; i < number_of_cells; i++)
    {
        transition_probability = move_probability(gridworld, i);
        transitional_matrix.push_back(vector <double>());
        transposed_matrix.push_back(vector <double>());
        for (int j = 0; j < number_of_cells; j++)
        {
            //find probability of moving from cell i to cell j
            if (can_move(gridworld, i, j))
              transitional_matrix.back().push_back(transition_probability);
            else
              transitional_matrix.back().push_back(0);
            transposed_matrix.back().push_back(0);
        }
    }


    

    //transpose matrix
    for (int i = 0; i < transitional_matrix.size(); i++)
    {
        for (int j = 0; j < transitional_matrix[i].size(); j++)
        {
            transposed_matrix[i][j] = transitional_matrix[j][i];        
        }
    }


    
    //multiply transposed matrix and initial matrix to get joint matrix
    for (int i = 0; i < transposed_matrix.size(); i++)
    {
        double joint_prob_sum = 0.0;
        for (int j = 0; j < transposed_matrix[i].size(); j++)
        {
            joint_prob_sum += transposed_matrix[i][j] * start_position_matrix[j];
        }
        joint_matrix.push_back(joint_prob_sum);
    }


    
    //observations for loop
    for (int x = 0; x < sequence_size-1; x++)
    {
        if (x == 0)
        {
            //build observational_matrix
            for (int i = 0; i < transposed_matrix.size(); i++)
            {
                observational_matrix.push_back(vector <double>());
                for (int j = 0; j < transposed_matrix[i].size(); j++)
                {
                    //only do diagonals
                    if (i == j) {
                        observational_matrix.back().push_back(calculate_observation_probability(gridworld, sequence_of_observations[x], i, sensory_error));
                    }
                    else
                        observational_matrix.back().push_back(0);
                }
            }


            //z matrix = transposed * observational
            for (int i = 0; i < transposed_matrix.size(); i++)
            {
                z_matrix.push_back(vector <double>());
                for (int j = 0; j < transposed_matrix[i].size(); j++)
                {
                    double z_sum = 0.0;
                    for (int k = 0; k < observational_matrix.size(); k++)
                    {
                        z_sum += transposed_matrix[i][k]*observational_matrix[k][j];
                    }
                    z_matrix.back().push_back(z_sum);
                }
            }


    
            //w matrix = z matrix * joint matrix
            for (int i = 0; i < z_matrix.size(); i++)
            {
                double w_prob_sum = 0.0;
                for (int j = 0; j < z_matrix[i].size(); j++)
                {
                    w_prob_sum += z_matrix[i][j] * joint_matrix[j];
                }
                w_matrix.push_back(w_prob_sum);
            }


    
            //j matrix = w matrix
            for (int i = 0; i < joint_matrix.size(); i++)
            {
                joint_matrix[i] = w_matrix[i];
            }
        }
        else
        {
            //build observational_matrix
            for (int i = 0; i < transposed_matrix.size(); i++)
            {
                for (int j = 0; j < transposed_matrix[i].size(); j++)
                {
                    //only do diagonals
                    if (i == j)
                        observational_matrix[i][j] = calculate_observation_probability(gridworld, sequence_of_observations[x], i, sensory_error);
                    else
                        observational_matrix[i][j] = 0;
                }
            }


    
            //z matrix = transposed * observational
            for (int i = 0; i < transposed_matrix.size(); i++)
            {
                for (int j = 0; j < transposed_matrix[i].size(); j++)
                {
                    double z_sum = 0.0;
                    for (int k = 0; k < observational_matrix[0].size(); k++)
                    {
                        z_sum += transposed_matrix[i][k]*observational_matrix[k][j];
                    }
                    z_matrix[i][j] = z_sum;
                }
            }


    

            //w matrix = z matrix * joint matrix
            for (int i = 0; i < z_matrix.size(); i++)
            {
                double w_prob_sum = 0.0;
                for (int j = 0; j < z_matrix[i].size(); j++)
                {
                    w_prob_sum += z_matrix[i][j] * joint_matrix[j];
                }
                w_matrix[i] = w_prob_sum;
            }


    

            //j matrix = w matrix
            for (int i = 0; i < joint_matrix.size(); i++)
            {
                joint_matrix[i] = w_matrix[i];
            }
        }
    }

    //calculate new o matrix for last observation
    for (int i = 0; i < transposed_matrix.size(); i++)
    {
        for (int j = 0; j < transposed_matrix[i].size(); j++)
        {
            //only do diagonals
            if (i == j)
                observational_matrix[i][j] = calculate_observation_probability(gridworld, sequence_of_observations[sequence_size-1], i, sensory_error);
            else
                observational_matrix[i][j] = 0;
        }
    }


    

    //multiply new o matrix with joint matrix to get final joint matrix
    for (int i = 0; i < observational_matrix.size(); i++)
    {
        double final_joint_prob_sum = 0.0;
        for (int j = 0; j < observational_matrix[i].size(); j++)
        {
            final_joint_prob_sum += observational_matrix[i][j] * joint_matrix[j];
        }
        final_joint_matrix.push_back(final_joint_prob_sum);
    }



    //find sum of joint matrix
    double total_joint_sum = 0.0;
    for (int i = 0; i < final_joint_matrix.size(); i++)
    {
        total_joint_sum += final_joint_matrix[i];
    }

    //divide each cell in joint matrix by sum
    for (int i = 0; i < final_joint_matrix.size(); i++)
    {
        final_joint_matrix[i] = final_joint_matrix[i]/total_joint_sum;
    }



    //find max
    double max = 0.0;
    int cell_num = 0;
    for (int i = 0; i < final_joint_matrix.size(); i++)
    {
        if (final_joint_matrix[i] > max)
        {
            max = final_joint_matrix[i];
            cell_num = i;
        }
    }
    
    cout << "Cell Number: " << cell_num << endl;

    int row = cell_num/gridworld[0].size();
    int column = cell_num%gridworld[0].size();
    
    cout << "(" << row << ", " << column << ") " << max << endl;

    return 0;
}

int convert_observation(string obs)
{
    if (obs == "E")
        return E;
    else if (obs == "W")
        return W;
    else if (obs == "WE")
        return WE;
    else if (obs == "S")
        return S;
    else if (obs == "SE")
        return SE;
    else if (obs == "SW")
        return SW;
    else if (obs == "SWE")
        return SWE;
    else if (obs == "N")
        return N;
    else if (obs == "NE")
        return NE;
    else if (obs == "NW")
        return NW;
    else if (obs == "NWE")
        return NWE;
    else if (obs == "NS")
        return NS;
    else if (obs == "NSE")
        return NSE;
    else if (obs == "NSW")
        return NSW;
    else if (obs == "NSWE")
        return NSWE;
    else 
        return 0;
}

double move_probability(vector <vector <int> > grid, int cell)
{
    int sum = 0;
    
    //right
    if (can_move(grid, cell, cell+1))
    {
        sum++;
    }
    //left
    if (can_move(grid, cell, cell-1))
    {
        sum++;
    }
    //up
    if (can_move(grid, cell, cell-grid[0].size()))
    {
        sum++;
    }
    //down
    if (can_move(grid, cell, cell+grid[0].size()))
    {
        sum++;
    }

    if (sum == 0) return 0;
    else return 1.0/sum;
}

bool can_move(vector <vector <int> > grid, int from, int to)
{
    int from_row = from/grid[0].size();
    int from_column = from%grid[0].size();
    int to_row = to/grid[0].size();
    int to_column = to%grid[0].size();

    if (from > grid.size()*grid[0].size() || to > grid.size()*grid[0].size())
        return false;
    
    if (from_row < 0 || from_column < 0 || to_row < 0 || to_column < 0)
        return false;

    if (from_row >= grid.size() || from_column >= grid[0].size() || to_row >= grid.size() || to_column >= grid[0].size())
        return false;
    
    int from_cell = grid[from_row][from_column];
    int to_cell = grid[to_row][to_column];

    string from_binary = convert_int_to_binary_string(from_cell);
    string to_binary = convert_int_to_binary_string(to_cell);
    
    //to cell is east
    if (from + 1 == to)
    {
        if (from_binary[3] == '0' && to_binary[2] == '0')
            return true;
        else 
            return false;
    }
    //to cell is west
    else if (from - 1 == to)
    {
        if (from_binary[2] == '0' && to_binary[3] == '0')
            return true;
        else
            return false;
    }
    //to cell is south
    else if (from + grid[0].size() == to)
    {
        if (from_binary[1] == '0' && to_binary[0] == '0')
            return true;
        else
            return false;
    }
    //to cell is north
    else if (from - grid[0].size() == to)
    {
        if (from_binary[0] == '0' && to_binary[1] == '0')
            return true;
        else
            return false;
    }
    else return false;
}

string convert_int_to_binary_string(int num)
{
    if (num == 0) return "0000";
    else if (num == 1)  return "0001";
    else if (num == 2)  return "0010";
    else if (num == 3)  return "0011";
    else if (num == 4)  return "0100";
    else if (num == 5)  return "0101";
    else if (num == 6)  return "0110";
    else if (num == 7)  return "0111";
    else if (num == 8)  return "1000";
    else if (num == 9)  return "1001";
    else if (num == 10) return "1010";
    else if (num == 11) return "1011";
    else if (num == 12) return "1100";
    else if (num == 13) return "1101";
    else if (num == 14) return "1110";
    else if (num == 15) return "1111";
    else return "0000";
}

double calculate_observation_probability(vector <vector <int> > grid, int observation, int cell, double error)
{
    string binary_observation = convert_int_to_binary_string(observation);
    int row = cell/grid[0].size();
    int column = cell%grid[0].size();
    int cell_value = grid[row][column];

    string binary_cell = convert_int_to_binary_string(cell_value);
    int differences = 0;

    //find number of differences
    if (!(binary_observation[0] == binary_cell[0]))
        differences++;
    if (!(binary_observation[1] == binary_cell[1]))
        differences++;
    if (!(binary_observation[2] == binary_cell[2]))
        differences++;
    if (!(binary_observation[3] == binary_cell[3]))
        differences++;

    return (pow(error, differences) * pow((1.0 - error),(4 - differences)));
}
