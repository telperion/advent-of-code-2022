#include<algorithm>
#include<numeric>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<tuple>
#include<set>

using namespace std;

struct Elf
{
	int lo;
	int hi;
};

// Assuming correct ordering
constexpr bool contains(Elf inner, Elf outer)
{
	return (outer.lo <= inner.lo && outer.hi >= inner.hi);
}

constexpr bool contains(Elf elf, int section)
{
	return (elf.lo <= section && elf.hi >= section);
}

class Team
{
protected:
	Elf l;
	Elf r;

public:
	friend istream& operator>>(istream& is, Team& obj)
	{
		char punc;
		is >> obj.l.lo >> punc >> obj.l.hi >> punc >> obj.r.lo >> punc >> obj.r.hi;
		return is;
	}

	friend ostream& operator<<(ostream& os, const Team& obj)
	{
		os << obj.l.lo << "-" << obj.l.hi << " & " << obj.r.lo << "-" << obj.r.hi;
		if (obj.contains())
		{
			os << " [overlaps / contains!]";
		}
		else if (obj.overlap())
		{
			os << " [overlaps!]";
		}
		return os;
	}

	bool contains() const
	{
		return (
			::contains(this->l, this->r) ||
			::contains(this->r, this->l)
		);
	}

	bool overlap() const
	{
		return (
			::contains(this->l, this->r.lo) ||
			::contains(this->l, this->r.hi) ||
			this->contains()
			);
	}
};


int main(int argc, char* argv[])
{
	vector<Team> teams;

	ifstream input;

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Open the input file
	cout << "Reading input file: " << filename << endl;
	input.open(filename, ios::in);
	string line;

	// Read in the strategy guide
	while (getline(input, line))
	{
		istringstream team_description(line);
		Team t;
		team_description >> t;
		cout << t << endl;
		teams.push_back(t);
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	int n_overlapping_teams = accumulate(
		teams.begin(),
		teams.end(),
		0,
		[](int prev, Team next) { return prev + (next.overlap() ? 1 : 0); }
	);
	int n_contained_teams = accumulate(
		teams.begin(),
		teams.end(),
		0,
		[](int prev, Team next) { return prev + (next.contains() ? 1 : 0); }
	);
	cout << "Number of overlapping teams: " << n_overlapping_teams << endl;
	cout << "Number of fully contained teams: " << n_contained_teams << endl;

	// No clean up!

	cout << "Done!" << endl;
	return 0;
}