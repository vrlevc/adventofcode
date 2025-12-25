#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	vector<string> values;

	ifstream puzzle(argv[1]);
	if (puzzle.is_open())
	{
		string line;
		bool init = true;
		unsigned long long int base = 1;
		while (getline(puzzle, line))
		{
			if (line[0] != '+' && line[0] != '*')
			{
				for (int i = 0; i < line.length(); ++i)
				{
					if (init) values.emplace_back("");
					if (line[i] != ' ') values[i].push_back(line[i]);
				}
				init = false;
			}
			else // calculate
			{
				int i = 0, j = 0;
				unsigned long long int sum = 0;
				for (char c : line)
				{
					if (c == '+')
					{
						while (j < values.size() && !values[j].empty()) sum += stoull(values[j++]);
						++i; ++j;
					}
					else if (c == '*')
					{
						unsigned long long int mul = 1;
						while (j < values.size() && !values[j].empty()) mul *= stoull(values[j++]);
						sum += mul;
						++i; ++j;
					}					
				}
				std::cout << "Sum : " << sum << endl;
			}
			base *= 10;
		}
		puzzle.close();
	}
}

