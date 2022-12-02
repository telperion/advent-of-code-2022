#include<algorithm>
#include<numeric>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

enum Shape { ROCK = 1, PAPER = 2, SCISSORS = 3, RPS_INVALID };
enum Result { LOSE = -1, DRAW = 0, WIN = 1, RESULT_INVALID };

class Choice
{
protected:
	Shape hand;

public:
	static Shape translate(char c)
	{
		switch (c)
		{
		case 'A': case 'X': return ROCK;		break;
		case 'B': case 'Y': return PAPER;		break;
		case 'C': case 'Z': return SCISSORS;	break;
		default:			return RPS_INVALID;
		}
	}

	Choice()
	{
		this->hand = RPS_INVALID;
	}

	Choice(char c)
	{
		this->hand = translate(c);
	}

	Choice(Shape hand)
	{
		this->hand = hand;
	}

	Choice(Choice against, Result desired_result)
	{
		if (against.hand == RPS_INVALID || desired_result == RESULT_INVALID)
		{
			this->hand = RPS_INVALID;
		}
		else
		{
			// Treat the three possible shapes as a cycle, and
			// the desired result hops from the opponent's shape to
			// the required shape.
			this->hand = Shape((against.hand + desired_result + 2) % 3 + 1);
		}
	}

	operator int() const
	{
		return int(this->hand);
	}

	friend istream& operator>>(istream& is, Choice& obj)
	{
		string s;
		is >> s;
		if (s.length() != 1)
		{
			obj.hand = RPS_INVALID;
		}
		else
		{
			obj.hand = translate(s[0]);
		}
		return is;
	}

	friend ostream& operator<<(ostream& os, const Choice& obj)
	{
		switch (obj.hand)
		{
		case ROCK:		os << "Rock";		break;
		case PAPER:		os << "Paper";		break;
		case SCISSORS:	os << "Scissors";	break;
		default:		os << "<INVALID>";
		}
		return os;
	}

	friend int cmp(const Choice& l, const Choice& r)
	{
		if (l.hand == RPS_INVALID || r.hand == RPS_INVALID)
		{
			return 0;
		}

		switch (l.hand - r.hand)
		{
		case 1: case -2: return  1;		// (P, R) =  1, (S, P) =  1, (R, S) = -2
		case 2: case -1: return -1;		// (R, P) = -1, (P, S) = -1, (S, R) =  2
		default:		 return  0;		// Invoked if l == r
		}
	}

	// Could overload boolean operators using cmp() if so inclined :)
};

class Round
{
protected:
	Choice me;
	Choice you;

public:
	Round() {}

	Round(Choice me, Choice you)
	{
		this->me = me;
		this->you = you;
	}

	int score_for_me() const
	{
		return 3 * (1 + cmp(this->me, this->you)) + this->me;
	}

	int score_for_you() const
	{
		return 3 * (1 + cmp(this->you, this->me)) + this->you;
	}

	friend istream& strategy_interpretation_A(istream& is, Round& obj)
	{
		// Assuming opponent "throws" first
		is >> obj.you >> obj.me;
		return is;
	}

	friend istream& strategy_interpretation_B(istream& is, Round& obj)
	{
		char desired_result_code;
		Result desired_result;

		// Assuming opponent "throws" first
		is >> obj.you >> desired_result_code;
		switch (desired_result_code)
		{
		case 'X': desired_result = LOSE; break;
		case 'Y': desired_result = DRAW; break;
		case 'Z': desired_result = WIN;  break;
		default: desired_result = RESULT_INVALID;
		}
		obj.me = Choice(obj.you, desired_result);
		return is;
	}

	friend ostream& operator<<(ostream& os, const Round& obj)
	{
		os << obj.me << " vs. " << obj.you << ": ";
		switch (cmp(obj.me, obj.you))
		{
		case  WIN: os << "WIN";  break;
		case LOSE: os << "LOSE"; break;
		case DRAW: os << "DRAW"; break;
		default:   os << "???";  break;
		}
		os << " (+" << obj.score_for_me() << " to player, +" << obj.score_for_you() << " to opponent)";
		return os;
	}
};


void summarize_match(vector<Round> match)
{
	// Tally up the scores for both player and elf.
	int player_total_score = accumulate(
		match.begin(),
		match.end(),
		0,
		[](int prev, const Round& m) { return prev + m.score_for_me(); }
	);
	int elf_total_score = accumulate(
		match.begin(),
		match.end(),
		0,
		[](int prev, const Round& m) { return prev + m.score_for_you(); }
	);

	// Give me my answers!
	cout << "Total scores from this strategy guide: Player " << player_total_score << ", Elf " << elf_total_score << endl;
}



int main(int argc, char* argv[])
{
	vector<Round> match_A;
	vector<Round> match_B;
	
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
		istringstream round_record_A(line);
		istringstream round_record_B(line);
		Round r;

		strategy_interpretation_A(round_record_A, r);
		cout << "Interpretation A: " << r << endl;
		match_A.push_back(r);

		strategy_interpretation_B(round_record_B, r);
		cout << "Interpretation B: " << r << endl;
		match_B.push_back(r);
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	cout << "### Under strategy guide interpretation A:" << endl;
	summarize_match(match_A);
	cout << "### Under strategy guide interpretation B:" << endl;
	summarize_match(match_B);

	// No clean up!

	cout << "Done!" << endl;
	return 0;
}