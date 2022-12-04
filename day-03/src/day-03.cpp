#include<algorithm>
#include<numeric>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<set>

using namespace std;


class Rucksack
{
public:
	set<char> contents;

protected:
	set<char> compartment_l;
	set<char> compartment_r;

public:
	Rucksack() {}

	Rucksack(string s)
	{
		int half = s.length() / 2;
		cout << s << ": " << half << endl;
		for (char c : s)
		{
			this->contents.insert(c);
		}
		for (char c : s.substr(   0, half))
		{
			this->compartment_l.insert(c);
		}
		for (char c : s.substr(half, half))
		{
			this->compartment_r.insert(c);
		}
	}

	vector<char> compartment_overlap()
	{
		vector<char> overlap_items;
		set_intersection(
			this->compartment_l.begin(),
			this->compartment_l.end(),
			this->compartment_r.begin(),
			this->compartment_r.end(),
			back_inserter(overlap_items)
		);
		return overlap_items;
	}

	char single_overlap()
	{
		vector<char> overlap_items = this->compartment_overlap();
		if (overlap_items.size() != 1)
		{
			cout << "Overlap between compartments is not a single item!" << endl;
			return '!';
		}
		return overlap_items[0];
	}

	static int priority(char item)
	{
		if (item >= 'a' && item <= 'z')
		{
			return int(item - 'a') + 1;
		}
		else if(item >= 'A' && item <= 'Z')
		{
			return int(item - 'A') + 27;
		}
		else
		{
			return 0;
		}
	}
};





int main(int argc, char* argv[])
{
	vector<Rucksack> rucksacks;

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
		Rucksack r(line);
		cout << r.single_overlap() << ": " << r.priority(r.single_overlap()) << endl;
		rucksacks.push_back(r);
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	int sum_priorities = accumulate(
		rucksacks.begin(),
		rucksacks.end(),
		0,
		[](int prev, Rucksack next) { return prev + next.priority(next.single_overlap()); }
	);
	cout << "Sum of priorities: " << sum_priorities << endl;

	// Just reading the prose for part B has me so confused lol
	int elf_group_size = 3;
	int sum_sticker_attachment_priorities = 0;
	for (int i = 0; i < rucksacks.size(); i += elf_group_size)
	{
		set<char> reduction = rucksacks[i].contents;
		for (int j = 0; j < elf_group_size; j++)
		{
			set<char> new_reduction;
			set_intersection(
				rucksacks[i+j].contents.begin(),
				rucksacks[i+j].contents.end(),
				reduction.begin(),
				reduction.end(),
				inserter(new_reduction, new_reduction.begin())
			);
			reduction = new_reduction;
		}
		string final;
		for (char c : reduction)
		{
			final += c;
		}
		cout << final << endl;
		sum_sticker_attachment_priorities += rucksacks[i].priority(final[0]);
	}
	cout << "Sum of sticker attachment priorities: " << sum_sticker_attachment_priorities << endl;


	// No clean up!

	cout << "Done!" << endl;
	return 0;
}