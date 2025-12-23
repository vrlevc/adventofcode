#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		cout << "Puzzle file name: " << argv[1] << '\n';

		ifstream inputPuzzle(argv[1]);

		if (inputPuzzle.is_open())
		{
			int code = 0, possition = 50;
			string line;
			while (getline(inputPuzzle, line))
			{
				int value = stoi(line.substr(1));
				int dir = line[0] == 'L' ? -1 : 1;
				while (value-- > 0) {
					possition += dir;
					if (possition == 0 || possition == 100) ++code;
					if (possition == -1)  possition = 99;
					if (possition == 100) possition = 0;
				}
			}

			cout << "Code: " << code << '\n';

			inputPuzzle.close();

			return 0;
		}
		else
		{
			cout << "Error: Could not open input puzzle file.\n";
		}
	}
	else
	{
		cout << "Error: input puzzle file is not set.\n";
	}

	return -1;
}