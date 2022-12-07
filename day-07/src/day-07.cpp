#include<algorithm>
#include<numeric>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<stdexcept>

using namespace std;

class FileItem
{
protected:
	// No distinction between directory and file...tehe
	string name;
	int leaf_size;
	vector<FileItem*> children;

public:
	FileItem()
	{
		this->name = "";
		this->leaf_size = 0;
		this->children = vector<FileItem*>();
	}

	FileItem(string name, int leaf_size)
	{
		this->name = name;
		this->leaf_size = leaf_size;
		this->children = vector<FileItem*>();
	}

	string named() const
	{
		return this->name;
	}

	void append(FileItem *obj)
	{
		this->children.push_back(obj);
	}

	FileItem* find(string name)
	{
		for (auto& iter : this->children)
		{
			if (iter->name == name)
			{
				return iter;
			}
		}
		throw invalid_argument("Couldn't find a file or directory named " + name + " under " + this->name + "!");
	}

	int total() const
	{
		int running_total = this->leaf_size;
		for (auto& iter : this->children)
		{
			running_total += iter->total();
		}
		return running_total;
	}

	void totals(map<string, int>& m) const
	{
		m[this->name] = this->total();
		for (auto& iter : this->children)
		{
			iter->totals(m);
		}
	}

	void print(string indenter = "") const
	{
		cout << indenter << this->name << ": " << this->leaf_size;
		if (this->children.size() > 0)
		{
			cout << " (" << this->total() << " total)";
		}
		cout << endl;

		for (auto& iter : this->children)
		{
			iter->print(indenter + "\t");
		}
	}

	int total_small_dirs(int size_upper_limit) const
	{
		int running_count = 0;
		int total_here = this->total();
		if ((total_here <= size_upper_limit) && (this->children.size() > 0))
		{
			running_count = total_here;
		}
		for (auto& iter : this->children)
		{
			running_count += iter->total_small_dirs(size_upper_limit);
		}
		return running_count;
	}

	const FileItem* deletion_candidate(int target_size) const
	{
		// Find the smallest directory that is at least target_size.
		const FileItem* candidate = nullptr;
		int closest_size = -1;

		cout << this->name << ": " << this->total() << endl;

		int total_here = this->total();
		if (total_here >= target_size)
		{
			candidate = this;
			closest_size = total_here;
		}

		for (auto& iter : this->children)
		{
			const FileItem* child_candidate = iter->deletion_candidate(target_size);
			if (child_candidate == nullptr)
			{
				// No subdirectories of at least target_size.
				continue;
			}

			int child_size = child_candidate->total();
			if (child_size < closest_size)
			{
				candidate = child_candidate;
				closest_size = child_size;
			}
		}
		
		if (candidate != nullptr)
		{
			cout << "\t" << candidate->name << ": " << candidate->total() << endl;
		}

		return candidate;
	}

	void clean()
	{
		for (auto& iter : this->children)
		{
			iter->clean();
			free(iter);
		}
	}
};

int main(int argc, char* argv[])
{
	ifstream input;
	FileItem root("/", 0);

	// Accept file input as first argument
	string filename = (argc > 1) ? argv[1] : "../data/input.txt";

	// Open the input file
	cout << "Reading input file: " << filename << endl;
	input.open(filename, ios::in);

	// Use the file system traversal to reconstruct the file system
	string line;
	bool in_ls = false;
	vector<FileItem*> nesting;
	while (getline(input, line))
	{
		istringstream interpret(line);
		string word;
		interpret >> word;
		if (word == "$")
		{
			// Navigation
			in_ls = false;
			interpret >> word;
			if (word == "cd")
			{
				// Change directory
				interpret >> word;
				if (word == "/")
				{
					cout << "Parsing: $ cd /" << endl;
					// All the way up
					nesting.clear();
					nesting.push_back(&root);
				}
				else if (word == "..")
				{
					cout << "Parsing: $ cd .." << endl;
					// Up one directory
					nesting.pop_back();
				}
				else
				{
					cout << "Parsing: $ cd <>" << endl;
					// root.print();
					// Down to the specified directory
					nesting.push_back(nesting.back()->find(word));
				}
			}
			else if (word == "ls")
			{
				cout << "Parsing: $ ls" << endl;
				// Directory listing
				in_ls = true;
			}
		}
		else
		{
			// File item in listing
			string name;
			interpret >> name;
			if (word == "dir")
			{
				cout << "Parsing: dir <>" << endl;
				// Directory
				nesting.back()->append(new FileItem(name, 0));
			}
			else
			{
				cout << "Parsing: file <>" << endl;
				// File
				nesting.back()->append(new FileItem(name, stoi(word)));
			}
		}

		for (auto& iter : nesting)
		{
			cout << iter->named() << "/";
		}
		cout << endl;
	}
	input.close();
	cout << "Read input file!" << endl;

	// Give me my answers!
	root.print();
	cout << endl;
	cout << "Total size of directories under 100000: " << root.total_small_dirs(100000) << endl;

	int total = root.total();
	int capacity = 70000000;
	int target = 30000000;
	const FileItem* delete_this_one = root.deletion_candidate(total - (capacity - target));
	cout << "Deletion candidate to clear " << total - (capacity - target) << ": " << delete_this_one->named() << " (" << delete_this_one->total() << " total)" << endl;
	
	// I'm dumb and misread the answer format but I'll leave this here as well!
	map<string, int> total_map;
	root.totals(total_map);
	multimap<int, string> total_map_rev;
	transform(
		total_map.begin(),
		total_map.end(),
		inserter(total_map_rev, total_map_rev.begin()),
		[](const pair<string, int>& p) { return pair<int, string>(p.second, p.first); }
	);
	for (auto& iter : total_map_rev)
	{
		cout << iter.first << ": " << iter.second << endl;
	}

	// Yes clean up!
	root.clean();

	cout << "Done!" << endl;
	return 0;
}