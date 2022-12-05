#include<algorithm>
#include<numeric>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

class Ship
{
protected:
	vector<vector<char>> cargo;
	// Should be vector<stack<char>> but I want to examine the stacks nondestructively!

public:
	int move(int stack_src, int stack_dst, int count, bool one_at_a_time = true)
	{
		int actual_move_count = 0;
		// One-indexed stacks
		int index_src = stack_src - 1;
		int index_dst = stack_dst - 1;
		if (index_src < cargo.size() && index_dst < cargo.size())
		{
			if (one_at_a_time)
			{
				for (int i = 0; i < count && cargo[index_src].size() > 0; i++)
				{
					cargo[index_dst].push_back(cargo[index_src].back());
					cargo[index_src].pop_back();
					actual_move_count++;
				}
			}
			else
			{
				int original_height = cargo[index_dst].size();
				for (int i = 0; i < count && cargo[index_src].size() > 0; i++)
				{
					cargo[index_dst].insert(cargo[index_dst].begin() + original_height, cargo[index_src].back());
					cargo[index_src].pop_back();
					actual_move_count++;
				}
			}
		}
		return actual_move_count;
	}

	friend istream& operator>>(istream& is, Ship& obj)
	{
		// Read in the cargo distribution, top down
		string line;
		while (getline(is, line))
		{
			cout << line << endl;
			if (line.find('[') == string::npos)
			{
				// Indexing line (not needed, can consume)
				break;
			}
			for (int stack_index = 0; 4*stack_index < line.length(); stack_index++)
			{
				//"[X] "
				char item = line[4 * stack_index + 1];
				if (item >= 'A' && item <= 'Z')
				{
					while (obj.cargo.size() <= stack_index)
					{
						obj.cargo.push_back(vector<char>());
					}
					obj.cargo[stack_index].push_back(item);
				}
			}
		}

		// Invert stacks from top-down to bottom-up.
		for (auto& iter : obj.cargo)
		{
			reverse(iter.begin(), iter.end());
		}

		return is;
	}

	friend ostream& operator<<(ostream& os, const Ship& obj)
	{
		size_t max_height = 0;
		for (auto& iter : obj.cargo)
		{
			max_height = max(max_height, iter.size());
		}

		for (int i = max_height - 1; i >= 0; i--)
		{
			for (auto& iter : obj.cargo)
			{
				if (iter.size() > i)
				{
					os << "[" << iter[i] << "] ";
				}
				else
				{
					os << "    ";
				}
			}
			os << endl;
		}
		for (int i = 0; i < obj.cargo.size(); i++)
		{
			os << setw(2) << i+1 << "  ";
		}
		os << endl;

		return os;
	}
};


int main(int argc, char* argv[])
{
	Ship ship;
	bool is_over_9000 = (argc > 2) ? bool(stoi(argv[2])) : true;

	ifstream input;

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Open the input file
	cout << "Reading input file: " << filename << endl;
	input.open(filename, ios::in);

	// Read the initial cargo state
	input >> ship;
	cout << ship;

	string line;
	int n_lines = 0;

	// Read in the crane actions
	while (getline(input, line))
	{
		// move N from A to B
		istringstream move_description(line);
		string human_word;
		int stack_src, stack_dst, count;
		input >> human_word >> count >> human_word >> stack_src >> human_word >> stack_dst;
		int actually_moved = ship.move(stack_src, stack_dst, count, is_over_9000);
		cout << "Attempted to move " << count << " from " << stack_src << " to " << stack_dst << ": actually moved " << actually_moved << endl;

		n_lines++;
		/*
		cout << ship;
		if (n_lines > 10)
		{
			break;
		}
		*/		
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	cout << "Final ship state:" << endl;
	cout << ship;

	// No clean up!

	cout << "Done!" << endl;
	return 0;
}