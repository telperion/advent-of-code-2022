#include<algorithm>
#include<numeric>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<set>

using namespace std;

int main(int argc, char* argv[])
{
	ifstream input;

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Marker length as second argument
	int marker_length = (argc > 2) ? stoi(argv[2]) : 4;

	// Open the input file
	cout << "Reading input file: " << filename << endl;
	input.open(filename, ios::in);

	// Read the initial cargo state
	string data;
	int start_of_packet = -1;
	while (!input.eof())
	{
		set<char> packet_start_check;
		char c;
		input.get(c);
		data += c;
		if (data.length() >= marker_length)
		{
			for (int i = 0; i < marker_length; i++)
			{
				packet_start_check.insert(*(data.rbegin() + i));
			}
			if (packet_start_check.size() == marker_length)
			{
				start_of_packet = data.length();
				break;
			}
		}
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	cout << "Start of packet marker ends at: " << start_of_packet << endl;
	
	// No clean up!

	cout << "Done!" << endl;
	return 0;
}