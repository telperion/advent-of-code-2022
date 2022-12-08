#include<algorithm>
#include<numeric>
#include<cassert>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<stdexcept>

using namespace std;

enum Direction {V_NORTH = 0b0001, V_SOUTH = 0b0010, V_EAST = 0b0100, V_WEST = 0b1000};
// (0, 0)		(M, 0)
//
//
//
// (0, N)		(M, N)

struct Tree
{
	int vis;
	int height;
};

class Forest
{
protected:
	vector<vector<Tree>> trees;
	int height;
	int width;

public:
	Tree& operator[] (pair<size_t, size_t> coordinate)
	{
		// (east-west, north-south) -> [north-south][east-west]
		assert(coordinate.second >= 0 && coordinate.second < trees.size());
		assert(coordinate.first  >= 0 && coordinate.first  < trees[coordinate.second].size());
		return trees[coordinate.second][coordinate.first];
	}

	const Tree& operator[] (pair<size_t, size_t> coordinate) const
	{
		// (east-west, north-south) -> [north-south][east-west]
		assert(coordinate.second >= 0 && coordinate.second < trees.size());
		assert(coordinate.first >= 0 && coordinate.first < trees[coordinate.second].size());
		return trees[coordinate.second][coordinate.first];
	}

	void process_visibility(Direction look_from)
	{
		vector<int> iter_look;			// The eye travels along the line from the direction given (inner loop)
		vector<int> iter_move;			// The viewer moves perpendicular (outer loop)
		switch (look_from)				// Length and increasing/decreasing aspect of both counters depend on direction
		{
		case V_NORTH:
			iter_look = vector<int>(height);	iota(iter_look.begin(), iter_look.end(), 0);
			iter_move = vector<int>(width);		iota(iter_move.begin(), iter_move.end(), 0);
			break;
		case V_SOUTH:
			iter_look = vector<int>(height);	iota(iter_look.rbegin(), iter_look.rend(), 0);
			iter_move = vector<int>(width);		iota(iter_move.begin(), iter_move.end(), 0);
			break;
		case V_EAST:
			iter_look = vector<int>(width);		iota(iter_look.rbegin(), iter_look.rend(), 0);
			iter_move = vector<int>(height);	iota(iter_move.begin(), iter_move.end(), 0);
			break;
		case V_WEST:
			iter_look = vector<int>(width);		iota(iter_look.begin(), iter_look.end(), 0);
			iter_move = vector<int>(height);	iota(iter_move.begin(), iter_move.end(), 0);
			break;
		}

		for (int m : iter_move)
		{
			int max_height_encountered = -1;
			for (int l : iter_look)
			{
				Tree *t;
				switch (look_from)
				{
				case V_NORTH:
				case V_SOUTH:
					t = &trees[l][m];		// Move east-west, look north-south
					break;
				case V_EAST:
				case V_WEST:
					t = &trees[m][l];		// Move north-south, look east-west
					break;
				}
				if (t->height > max_height_encountered)
				{
					t->vis |= look_from;
					max_height_encountered = t->height;
				}
			}
		}
	}

	int count_visible_trees() const
	{
		// From outside only!
		return accumulate(
			trees.begin(),
			trees.end(),
			0,
			[](int prev, vector<Tree> v) { return prev + accumulate(
				v.begin(),
				v.end(),
				0,
				[](int prev, Tree t) { return prev + int(t.vis > 0); }
			); }
		);
	}

	int count_visible_trees(pair<size_t, size_t> coordinate, Direction look_toward) const
	{
		// (east-west, north-south) -> [north-south][east-west]
		// From internal coordinate in one direction only!
		int visible = 0;
		int height_here = trees[coordinate.second][coordinate.first].height;
		switch (look_toward)
		{
		case V_NORTH:
			for (int ns = coordinate.second - 1; ns >= 0; ns--)
			{
				visible++;
				if (trees[ns][coordinate.first].height >= height_here) { break; }
			}
			break;
		case V_SOUTH:
			for (int ns = coordinate.second + 1; ns < height; ns++)
			{
				visible++;
				if (trees[ns][coordinate.first].height >= height_here) { break; }
			}
			break;
		case V_EAST:
			for (int ew = coordinate.first + 1; ew < width; ew++)
			{
				visible++;
				if (trees[coordinate.second][ew].height >= height_here) { break; }
			}
			break;
		case V_WEST:
			for (int ew = coordinate.first - 1; ew >= 0; ew--)
			{
				visible++;
				if (trees[coordinate.second][ew].height >= height_here) { break; }
			}
			break;
		}
		return visible;
	}

	int calc_scenic_score(pair<size_t, size_t> coordinate)
	{
		// (east-west, north-south) -> [north-south][east-west]
		// Multiply the visible tree count from each of the four directions, e.g.
		// north 2, south 3, west 1, east 4 = 2*3*1*4 = 24
		int scenic_score = 1;
		for (Direction dir : {V_NORTH, V_SOUTH, V_EAST, V_WEST})
		{
			int scene = count_visible_trees(coordinate, dir);
			// cout << scene << " ";
			scenic_score *= scene;
		}
		// cout << " = " << scenic_score << endl;
		return scenic_score;
	}

	int calc_max_scenic_score()
	{
		vector<int> v_ew(width);		iota(v_ew.begin(), v_ew.end(), 0);
		vector<int> v_ns(height);		iota(v_ns.begin(), v_ns.end(), 0);

		return accumulate(
			v_ns.begin(),
			v_ns.end(),
			0,
			[&](int prev, int ns) { return max(prev, accumulate(
				v_ew.begin(),
				v_ew.end(),
				0,
				[&](int prev, int ew) { return max(prev, calc_scenic_score(make_pair(ew, ns))); }
		)); }
		);
	}

	friend istream& operator>> (istream& is, Forest& obj)
	{
		string line;
		while (getline(is, line))
		{
			vector<Tree> v;
			for (char c : line)
			{
				Tree t;
				t.vis = 0;
				t.height = int(c - '0');
				v.push_back(t);
			}
			obj.trees.push_back(v);
		}

		// Assuming that the forest has no data gaps or unequal row/column sizes!
		obj.height = obj.trees.size();
		obj.width = accumulate(
			obj.trees.begin(),
			obj.trees.end(),
			0,
			[](int prev, vector<Tree> v) { return max(prev, int(v.size())); }
		);

		return is;
	}

	friend ostream& operator<< (ostream& os, const Forest& obj)
	{
		ios_base::fmtflags prev_state(os.flags());
		for (auto& v : obj.trees)
		{
			for (auto& t : v)
			{
				os << t.height << hex << t.vis << " ";
			}
			os << endl << endl;
		}
		os.flags(prev_state);

		return os;
	}
};

int main(int argc, char* argv[])
{
	ifstream input;
	Forest forest;

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Open the input file
	cout << "Reading input file: " << filename << endl;
	input.open(filename, ios::in);

	// Can't see the forest for the trees? Try this 1 weird trick!
	input >> forest;
	
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	for (auto& dir : { V_NORTH, V_SOUTH, V_EAST, V_WEST })
	{
		forest.process_visibility(dir);
	}
	// cout << forest << endl;
	cout << "Total visible trees: " << forest.count_visible_trees() << endl;
	cout << "Maximum scenic score: " << forest.calc_max_scenic_score() << endl;

	// No clean up!

	cout << "Done!" << endl;
	return 0;
}