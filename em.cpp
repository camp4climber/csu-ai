#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

double number_of_transitions(vector <char> transitions, char state1, char state2);

int main(int argc, char *argv[])
{
    cout << fixed;
    cout << setprecision(6);
    int num_of_iterations = atoi(argv[5]);
    ifstream obs_input;
    ifstream trans_input;
    ifstream sens_input;
    ifstream orig_input;
    ofstream output;
    vector <char> observations;
    vector <vector <double> > transition_matrix;
    vector <vector <double> > sensory_matrix;
    vector <char> originals;
    vector <vector <double> > em_matrix;
    vector <vector <char> > backtrack_matrix;
    vector <char> probable_sequence;
    double b, l, m, b_zero, l_zero, m_zero, accuracy;
    int num_b, num_l, num_m, num_bh, num_bt, num_lh, num_lt, num_mh, num_mt, same;

    //get observations
    obs_input.open(argv[1]);
    if (obs_input)
    {
        char ch;
        while (obs_input >> ch)
        {
            observations.push_back(ch);
        }
    }
    obs_input.close();
    
    //get transitions
    trans_input.open(argv[2]);
    if (trans_input)
    {
        string line;
        double value;

        while(getline(trans_input, line))
        {
            transition_matrix.push_back(vector <double>());
            stringstream split(line);
            while (split >> value)
            {
              transition_matrix.back().push_back(-log2(value));
            }
        }
    }
    trans_input.close();
    
    //get sensory
    sens_input.open(argv[3]);
    if (sens_input)
    {
        string line;
        double value;

        while(getline(sens_input, line))
        {
            sensory_matrix.push_back(vector <double>());
            stringstream split(line);
            while (split >> value)
            {
                sensory_matrix.back().push_back(-log2(value));
                sensory_matrix.back().push_back(-log2(1-value));
            }
        }
    }
    sens_input.close();
    
    //get original input
    orig_input.open(argv[4]);
    if (orig_input)
    {
        char ch;
        while (orig_input >> ch)
        {
            originals.push_back(ch);
        }

    }
    orig_input.close();

    //output file sensory + transition
    output.open("output.txt");

    //diagnosis
    cout << "Observations: ";
    for (int i = 0; i < observations.size(); i++)
    {
        cout << observations[i];
    }
    cout << endl << endl;

    cout << "Transition: " << endl;
    for (int i = 0; i < transition_matrix.size(); i++)
    {
        for (int j = 0; j < transition_matrix[i].size(); j++)
        {
            cout << transition_matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Sensory: " << endl;
    for (int i = 0; i < sensory_matrix.size(); i++)
    {
        for (int j = 0; j < sensory_matrix[i].size(); j++)
        {
            cout << sensory_matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Original: ";
    for (int i = 0; i < originals.size(); i++)
    {
        cout << originals[i];
    }
    cout << endl << endl;


    //initialize em + bt matrix
    for (int i = 0; i < 3; i++)
    {
        em_matrix.push_back(vector <double>());
        backtrack_matrix.push_back(vector <char>());
        for (int j = 0; j < observations.size(); j++)
        {
            em_matrix.back().push_back(0);
            backtrack_matrix.back().push_back('0');
        }
    }

    //initialize probable sequence
    for (int i = 0; i < observations.size()+1; i++)
    {
        probable_sequence.push_back('0');
    }

    //viterbi algorithm
    for (int i = 0; i < num_of_iterations; i++)
    {
        //start probs
        b_zero = 1.5849;
        l_zero = 1.5849;
        m_zero = 1.5849;

        //find min for em[0][0]
        b = b_zero + transition_matrix[0][0];
        l = l_zero + transition_matrix[0][1];
        m = m_zero + transition_matrix[0][2];
        if (b == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[0][0] = b + sensory_matrix[0][0];
            else
                em_matrix[0][0] = b + sensory_matrix[0][1];
            backtrack_matrix[0][0] = 'b';
        }
        else if (l == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[0][0] = l + sensory_matrix[0][0];
            else
                em_matrix[0][0] = l + sensory_matrix[0][1];
            backtrack_matrix[0][0] = 'l';
        }
        else if (m == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[0][0] = m + sensory_matrix[0][0];
            else
                em_matrix[0][0] = m + sensory_matrix[0][1];
            backtrack_matrix[0][0] = 'm';
        }

        //find min for em[1][0]
        b = b_zero + transition_matrix[1][0];
        l = l_zero + transition_matrix[1][1];
        m = m_zero + transition_matrix[1][2];
        if (b == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[1][0] = b + sensory_matrix[1][0];
            else
                em_matrix[1][0] = b + sensory_matrix[1][1];
            backtrack_matrix[1][0] = 'b';
        }
        else if (l == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[1][0] = l + sensory_matrix[1][0];
            else
                em_matrix[1][0] = l + sensory_matrix[1][1];
            backtrack_matrix[1][0] = 'l';
        }
        else if (m == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[1][0] = m + sensory_matrix[1][0];
            else
                em_matrix[1][0] = m + sensory_matrix[1][1];
            backtrack_matrix[1][0] = 'm';
        }

        //find min for em[2][0]
        b = b_zero + transition_matrix[2][0];
        l = l_zero + transition_matrix[2][1];
        m = m_zero + transition_matrix[2][2];
        if (b == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[2][0] = b + sensory_matrix[2][0];
            else
                em_matrix[2][0] = b + sensory_matrix[2][1];
            backtrack_matrix[2][0] = 'b';
        }
        else if (l == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[2][0] = l + sensory_matrix[2][0];
            else
                em_matrix[2][0] = l + sensory_matrix[2][1];
            backtrack_matrix[2][0] = 'l';
        }
        else if (m == min(min(b, l), m))
        {
            if (observations[0] == 'H')
                em_matrix[2][0] = m + sensory_matrix[2][0];
            else
                em_matrix[2][0] = m + sensory_matrix[2][1];
            backtrack_matrix[2][0] = 'm';
        }

        for (int j = 1; j < observations.size(); j++)
        {
            //find minimum for em_matrix[0][j]
            b = em_matrix[0][j-1] + transition_matrix[0][0];
            l = em_matrix[1][j-1] + transition_matrix[0][1];
            m = em_matrix[2][j-1] + transition_matrix[0][2];
            if (b == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[0][j] = b + sensory_matrix[0][0];
                else
                    em_matrix[0][j] = b + sensory_matrix[0][1];
                backtrack_matrix[0][j] = 'b';
            }
            else if (l == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[0][j] = l + sensory_matrix[0][0];
                else
                    em_matrix[0][j] = l + sensory_matrix[0][1];
                backtrack_matrix[0][j] = 'l';
            }
            else if (m == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[0][j] = m + sensory_matrix[0][0];
                else
                    em_matrix[0][j] = m + sensory_matrix[0][1];
                backtrack_matrix[0][j] = 'm';
            }

            //find minimum for em_matrix[1][j]
            b = em_matrix[0][j-1] + transition_matrix[1][0];
            l = em_matrix[1][j-1] + transition_matrix[1][1];
            m = em_matrix[2][j-1] + transition_matrix[1][2];
            if (b == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[1][j] = b + sensory_matrix[1][0];
                else
                    em_matrix[1][j] = b + sensory_matrix[1][1];
                backtrack_matrix[1][j] = 'b';
            }
            else if (l == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[1][j] = l + sensory_matrix[1][0];
                else
                    em_matrix[1][j] = l + sensory_matrix[1][1];
                backtrack_matrix[1][j] = 'l';
            }
            else if (m == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[1][j] = m + sensory_matrix[1][0];
                else
                    em_matrix[1][j] = m + sensory_matrix[1][1];
                backtrack_matrix[1][j] = 'm';
            }

            //find minimum for em_matrix[2][j]
            b = em_matrix[0][j-1] + transition_matrix[2][0];
            l = em_matrix[1][j-1] + transition_matrix[2][1];
            m = em_matrix[2][j-1] + transition_matrix[2][2];
            if (b == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[2][j] = b + sensory_matrix[2][0];
                else
                    em_matrix[2][j] = b + sensory_matrix[2][1];
                backtrack_matrix[2][j] = 'b';
            }
            else if (l == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[2][j] = l + sensory_matrix[2][0];
                else
                    em_matrix[2][j] = l + sensory_matrix[2][1];
                backtrack_matrix[2][j] = 'l';
            }
            else if (m == min(min(b, l), m))
            {
                if (observations[j] == 'H')
                    em_matrix[2][j] = m + sensory_matrix[2][0];
                else
                    em_matrix[2][j] = m + sensory_matrix[2][1];
                backtrack_matrix[2][j] = 'm';
            }
        }

        cout << "Last column: \n\n";
        cout << em_matrix[0][em_matrix.size()] << endl;
        cout << em_matrix[1][em_matrix.size()] << endl;
        cout << em_matrix[2][em_matrix.size()] << endl << endl;


        //find probable sequence from backtrack matrix
        if (em_matrix[0][em_matrix.size()-1] == 
            min(min(em_matrix[0][em_matrix.size()-1],
            em_matrix[1][em_matrix.size()-1]),
            em_matrix[2][em_matrix.size()-1]))
        {
            probable_sequence[0] = 'b';
        }
        else if (em_matrix[1][em_matrix.size()-1] == 
            min(min(em_matrix[0][em_matrix.size()-1],
            em_matrix[1][em_matrix.size()-1]),
            em_matrix[2][em_matrix.size()-1]))
        {
            probable_sequence[0] = 'l';
        }
        else if (em_matrix[2][em_matrix.size()-1] == 
            min(min(em_matrix[0][em_matrix.size()-1], 
            em_matrix[1][em_matrix.size()-1]),
            em_matrix[2][em_matrix.size()-1]))
        {
            probable_sequence[0] = 'm';
        }
        for (int i = 1; i < probable_sequence.size(); i++)
        {
            if (probable_sequence[i-1] == 'b')
            {
                probable_sequence[i] = backtrack_matrix[0][probable_sequence.size()-1-i];
            }
            else if (probable_sequence[i-1] == 'l')
            {
                probable_sequence[i] = backtrack_matrix[1][probable_sequence.size()-1-i];
            }
            else if (probable_sequence[i-1] == 'm')
            {
                probable_sequence[i] = backtrack_matrix[2][probable_sequence.size()-1-i];
            }
        }

        reverse(probable_sequence.begin(), probable_sequence.end());

        cout << "@ iteration " << i << endl;
        cout << "EM Matrix: " << endl;
        for (int i = 0; i < em_matrix.size(); i++)
        {
            for (int j = 0; j <em_matrix[i].size(); j++)
            {
                cout << em_matrix[i][j] << "\t";
            }
            cout << endl;
        }
        cout << endl;
    
        cout << "Backtrack: " << endl;
        for (int i = 0; i < backtrack_matrix.size(); i++)
        {
            for (int j = 0; j <backtrack_matrix[i].size(); j++)
            {
                cout << backtrack_matrix[i][j] << "\t";
            }
            cout << endl;
        }
        cout << endl;

        cout << "Probable Sequence: " << endl;
        for (int i = 0; i < probable_sequence.size(); i++)
        {
            cout << probable_sequence[i] << "\t";
        }
        cout << endl << endl;

        //get new transition matrix using laplacian smoothing
        num_b = 0;
        num_l = 0;
        num_m = 0;
        for (int j = 1; j < probable_sequence.size(); j++)
        {
            if (probable_sequence[j] == 'b')
                num_b++;
            else if (probable_sequence[j] == 'l')
                num_l++;
            else if (probable_sequence[j] == 'm')
                num_m++;
        }

        transition_matrix[0][0] = -log2((number_of_transitions(probable_sequence, 'b', 'b') + 1) / (num_b + 3));
        transition_matrix[1][0] = -log2((number_of_transitions(probable_sequence, 'b', 'l') + 1) / (num_b + 3));
        transition_matrix[2][0] = -log2((number_of_transitions(probable_sequence, 'b', 'm') + 1) / (num_b + 3));

        transition_matrix[0][1] = -log2((number_of_transitions(probable_sequence, 'l', 'b') + 1) / (num_l + 3));
        transition_matrix[1][1] = -log2((number_of_transitions(probable_sequence, 'l', 'l') + 1) / (num_l + 3));
        transition_matrix[2][1] = -log2((number_of_transitions(probable_sequence, 'l', 'm') + 1) / (num_l + 3));

        transition_matrix[0][2] = -log2((number_of_transitions(probable_sequence, 'm', 'b') + 1) / (num_m + 3));
        transition_matrix[1][2] = -log2((number_of_transitions(probable_sequence, 'm', 'l') + 1) / (num_m + 3));
        transition_matrix[2][2] = -log2((number_of_transitions(probable_sequence, 'm', 'm') + 1) / (num_m + 3));

        cout << "New Transition Matrix: " << endl;
        for (int j = 0; j < transition_matrix.size(); j++)
        {
            for (int k = 0; k < transition_matrix[j].size(); k++)
            {
                cout << transition_matrix[j][k] << "\t";
            }
            cout << endl;
        }
        cout << endl;

        //get new sensory matrix using laplacian smoothing
        num_bh = 0;
        num_bt = 0;
        num_lh = 0;
        num_lt = 0;
        num_mh = 0;
        num_mt = 0;
        for (int j = 1; j < probable_sequence.size(); j++)
        {
            if (probable_sequence[j] == 'b')
            {
                if (observations[j-1] == 'H')
                    num_bh++;
                else
                    num_bt++;
            }
            else if (probable_sequence[j] == 'l')
            {
                if (observations[j-1] == 'H')
                    num_lh++;
                else
                    num_lt++;
            }
            else if (probable_sequence[j] == 'm')
            {
                if (observations[j-1] == 'H')
                    num_mh++;
                else
                    num_mt++;
            }
        }

        sensory_matrix[0][0] = -log2((double)(num_bh + 1) / (num_b + 2));
        sensory_matrix[0][1] = -log2((double)(num_bt + 1) / (num_b + 2));

        sensory_matrix[1][0] = -log2((double)(num_lh + 1) / (num_l + 2));
        sensory_matrix[1][1] = -log2((double)(num_lt + 1) / (num_l + 2));

        sensory_matrix[2][0] = -log2((double)(num_mh + 1) / (num_m + 2));
        sensory_matrix[2][1] = -log2((double)(num_mt + 1) / (num_m + 2));

        output << fixed;
        output << setprecision(6);
 
        cout << "New Sensory Matrix: " << endl;
        for (int j = 0; j < sensory_matrix.size(); j++)
        {
            for (int k = 0; k < sensory_matrix[j].size(); k++)
            {
                cout << sensory_matrix[j][k] << "\t";
            }
            cout << endl;
        }
        cout << endl;

        //classifier accuracy
        same = 0;
        for (int j = 0; j < probable_sequence.size(); j++)
        {
            if (toupper(probable_sequence[j+1]) == originals[j])
                same++;
        }

        accuracy = ((double)same / (probable_sequence.size()-1))*100;
        cout << "Accuracy: " << accuracy << "%" << endl;
    }
    
    if (output)
    {
        output << "Transition Probabilities Learned: \n";
        for (int i = 0; i < transition_matrix.size(); i++)
        {
            for (int j = 0; j < transition_matrix[i].size(); j++)
            {
                output << pow(2, -transition_matrix[i][j]) << " ";
            }
            output << endl;
        }

        output << "\nSensory Probabilities Learned: \n";
        for (int i = 0; i < sensory_matrix.size(); i++)
        {
            for (int j = 0; j < sensory_matrix[i].size(); j++)
            {
                output << pow(2, -sensory_matrix[i][j]) << " ";
            }
            output << endl;
        }
        output << endl;
    }

    output << "Sequence: \n";
    for (int j = 0; j < probable_sequence.size(); j++)
    {
        output << (char)toupper(probable_sequence[j]);
    }

    output << "\n\nAccuracy: " << accuracy << "%\n";
    output.close();
    return 0;
}

double number_of_transitions(vector <char> transitions, char state1, char state2)
{
    int num_trans = 0;
    for (int i = 0; i < transitions.size()-1; i++)
    {
        if (transitions[i] == state1 && transitions[i+1] == state2)
        {
            num_trans++;
        }
    }
    return (double)num_trans;
}