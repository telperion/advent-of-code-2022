#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

int ELF_COUNT = 0;

class Elf
{
protected:
	int id;
	vector<int> carrying;

public:
	Elf()
	{
		this->id = ELF_COUNT++;
	}

	void add_item(int calories)
	{
		this->carrying.push_back(calories);
	}

	int total() const
	{
		return accumulate(
			this->carrying.begin(),
			this->carrying.end(),
			0
		);
	}

	string repr() const
	{
		return accumulate(
			this->carrying.begin() + 1,
			this->carrying.end(),
			to_string(this->carrying[0]),
			[](string prev, int item) {
				return prev + " + " + to_string(item);
			}
		);
	}

	friend ostream& operator<<(ostream& os, const Elf& obj)
	{
		os << "Elf #" << obj.id << ": " << obj.repr() << " = " << obj.total() << " calories";
		return os;
	}
};

int main(int argc, char* argv[])
{
	vector<Elf*> elves;
	Elf* elf = nullptr;
	ifstream input;

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Open the input file
	cout << "Reading input file..." << filename << endl;
	input.open(filename, ios::in);
	string line;

	// Load up the elves with food and stuff
	while (getline(input, line))
	{
		// Empty lines delimit elves
		if (line.length() > 0)
		{
			// Food
			if (elf == nullptr)
			{
				// Call up a new elf
				elf = new Elf();
			}
			elf->add_item(stoi(line));
		}
		else
		{
			// End of elf's backpack
			if (elf != nullptr)
			{
				// Run along now!
				elves.push_back(elf);
				elf = nullptr;
			}
		}
	}
	input.close();
	cout << "Read input file!" << endl;


	// Use std::algorithm::sort with a lambda function
	cout << "Sorting elves..." << endl;
	sort(elves.begin(), elves.end(), [](Elf* a, Elf* b) {return (a->total() > b->total());});
	cout << "Sorted!" << endl;


	// Elf listing
	int elf_index = 0;
	for (auto elf : elves)
	{
		cout << *elf << endl;
	}

	// Give me my answers!
	cout << "Elf carrying the most calories:" << endl;
	cout << "\t" << *elves[0] << endl;
	cout << "The three elves carrying the most calories:" << endl;
	int t = 0;
	for (int i = 0; i < 3; i++)
	{
		t += elves[i]->total();
		cout << "\t" << *elves[i] << endl;
	}
	cout << "\tTotal: " << t << " calories" << endl;

	// Clean up?
	for (auto elf : elves)
	{
		free(elf);
	}

	cout << "Done!" << endl;
	return 0;
}
