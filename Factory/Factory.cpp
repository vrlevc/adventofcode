#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include <chrono>

using namespace std;

using INT = unsigned short;

class Machine
{
	INT L = 0;		// Indicator
	vector<INT> B;  // Buttons
public:
	static Machine ReadMachine(const string& line); // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
	static INT ReadLights(const string& line); // .##.
	static INT ReadButton(const string& line); // 1,3
	INT MinCombination();
	bool IsPossible(INT N, INT CURR, INT ID);
	int ButtonsNum() const { return B.size(); }
};

class Factory
{
public:
	static INT SUM;
	static void ReadFactory(const char* file);
};
INT Factory::SUM = 0;

int main(int argc, char* argv[])
{
	auto start = chrono::high_resolution_clock::now();
	Factory::ReadFactory(argv[1]);
	cout << "SUM : " << Factory::SUM << endl;
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;
	std::cout << "Function execution time: " << duration.count() << " milliseconds" << std::endl;
}

void PrintID(INT id, size_t LEN)
{
	string ids;
	INT k = 1;
	while (id / k)
	{
		ids.push_back( id & k ? '1' : '0');
		k <<= 1;
	}
	for (int i = max(ids.length(), LEN) - 1; 0 <= i; --i)
	{
		if (i < ids.length()) cout << ids[i];
		else cout << '0';
	}
}

bool Machine::IsPossible(INT N, INT CURR, INT ID) // select N buttons
{
	if (N == 0) return CURR == L;
	for (int b = 0; b < B.size(); ++b)
	{
		INT id = 1 << b;
		if ((ID & id) == 0) // not used yet!
		{	
			id |= ID;
			// PrintID(id, B.size()); cout << "  XOR : " << (CURR ^ B[b]) << "  B : " << B[b] << endl;
			if (IsPossible(N - 1, CURR ^ B[b], id)) return true;
		}
	}
	return false;
}

INT Machine::MinCombination()
{
	for (int i = 1; i <= B.size(); ++i) 
		if (IsPossible(i, 0, 0)) 
			return i;
	assert(false);
	return 0;
}

INT Machine::ReadLights(const string& line) // .##.
{
	INT l = 0;
	INT a = 1;
	for (const char c : line)
	{
		if (c == '#') 
			l |= a;
		a <<= 1;
	}
	return l;
}

INT Machine::ReadButton(const string& line) // 1,3
{
	INT B = 0;
	assert('0' <= line[0] && line[0] <= '9');
	for (const char c : line) 
	{
		if (c != ',')
		{
			INT v = INT(c - '0');
			INT b = (1 << v);
			B |= b;
		}
	}
	return B;
}

Machine Machine::ReadMachine(const string& line)
{
	Machine m;
	// Read lights:
	assert(line[0] == '[');
	int l = 1;
	int r = l+1;
	for (; r < line.length(); ++r) if (line[r] == ']') break;
	m.L = ReadLights(line.substr(l, (r++) - l)); l = r;
	while (line[r] != '{')
	{
		for (; r < line.length(); ++r) { if (line[r] == '(') break; } 
		l = ++r;
		for (; r < line.length(); ++r) { if (line[r] == ')') break; } 
		m.B.push_back(ReadButton(line.substr(l, r - l)));
		++r;
		while (line[r] == ' ') ++r;
	}
	for (int i = 0; i < m.B.size() / 2; ++i) swap(m.B[i], m.B[m.B.size() - 1 - i]);
	return m;
}

void Factory::ReadFactory(const char* file)
{
	ifstream puzzle(file);
	if (puzzle.is_open())
	{
		int max_buttons = 0;
		string line;
		while (getline(puzzle, line))
		{
			Machine m = Machine::ReadMachine(line);
			max_buttons = max(max_buttons, m.ButtonsNum());
			INT sum = m.MinCombination();
			// cout << "MIN Buttons : " << sum << endl;
			SUM += sum;
		}
		puzzle.close();
		cout << "Max buttons : " << max_buttons << endl;
	}
}
