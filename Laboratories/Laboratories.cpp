#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

void FOLLOW(vector<string>& graph, int c, int r, int& PATHS);

int main(int argc, char* argv[])
{
	vector<string> graph;

	ifstream puzzle(argv[1]);
	if (puzzle.is_open())
	{
		int res = 0;
		string line, back;
		while (getline(puzzle, line))
		{
			if (!back.empty())
			{
				for (int i = 0; i < line.length(); ++i)
				{
					if (back[i] == '|')
					{
						if (line[i] == '^')
						{
							++res; // new split
							if (0 <= i - 1) line[i - 1] = '|';
							if (i + 1 < line.length()) line[i + 1] = '|';
						}
						else line[i] = '|';
					}
				}
			}
			else for (char& c : line) if (c == 'S') c = '|';
			graph.push_back(line);
			back = move(line);
		}
		puzzle.close();

		cout << "Splitted : " << res << endl;
	}
	else
	{
		cout << "Error: Unable to open file." << endl;
		return -1;
	}

	for (const auto& line : graph) cout << line << endl;

	//
	// search all possible paths
	//
	int N = graph[0].length();
	vector<unsigned long long int> v(N, 0);
	for (int i = 0; i < N; ++i) if (graph[graph.size() - 1][i] == '|') v[i] = 1;

	for (int i = graph.size() - 2; 0 <= i; --i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (graph[i][j] == '^') 
			{
				v[j] = v[j - 1] + v[j + 1];
			}
		}
	}

	int i = 0;
	for (; i < N && graph[0][i] != '|'; ++i);

	cout << "PATHS : " << v[i] << endl;

	return 0;
}
