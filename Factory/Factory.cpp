#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include <chrono>

using namespace std;

using INT = unsigned short;
INT INT_INF = UINT16_MAX;

class Machine
{
	INT L = 0;		// Indicator
	vector<INT> B;  // Buttons
	vector<INT> J;  // Jvoltage
public:
	static Machine ReadMachine(const string& line); // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
	static INT ReadLights(const string& line); // .##.
	static INT ReadButton(const string& line); // 1,3
	INT MinCombination();
	bool IsPossible(INT N, INT CURR, INT ID);
	int ButtonsNum() const { return (int)B.size(); }
};

struct Range
{
	INT a, b;
	static Range Cross(const Range& a, const Range& b)
	{
		assert(a.a <= a.b); assert(b.a <= b.b);
		Range cross = { max(a.a, b.a), min(a.b, b.b) };
		assert(cross.a <= cross.b);
		return cross;
	}
};

class Junit
{
	INT S; // sum of 0*x0 + 1*x1 + ...
	vector<INT> J; // 0 1 1 0
public:
	Junit(INT S, const vector<INT> J) : S(S), J(J) {}
	bool operator==(const Junit& JU) const 
	{
		if (S != JU.S) return false;
		for (int i = 0; i < J.size(); ++i) if (J[i] != JU.J[i]) return false;
		return true; 
	}
	vector<Range> GetRanges() const
	{
		int rang = 0; for (const auto& r : J) if (r == 1) ++rang;
		vector<Range> ranges(J.size(), Range{0, INT_INF});
		for (int i = 0; i < J.size(); ++i) 
			if (J[i] == 1) 
				ranges[i] = ( rang == 1 ? Range{ S, S} : Range{ 0, S } );
		return ranges;
	}
	int VariablesCount() const { return (int)J.size(); }
};

class Jvoltage
{
	vector<Junit> R;
public:
	// [B0(Jn-1, ..., J0),..., Bn-1(Jn-1,...,J0)] [S0,...,Sn-1]
	Jvoltage(const vector<INT>& B, const vector<INT>& S)
	{
		for (int j = (int)S.size() - 1; 0 <= j; --j)
		{
			INT s = S[j]; // jvoltage for j indes start from 0
			INT i = 1 << j;
			vector<INT> r(B.size(), 0);
			for (int b = 0; b < B.size(); ++b)
			{
				if (B[b] & i) r[b] = 1;
			}
			Junit junit(s, r);
			bool add = true;
			for (const Junit& ju : R) if (ju == junit) { add = false; break; }
			if (add) R.push_back(junit);
		}
	}

	void Solve()
	{
		vector<Range> BOUNDS = R[0].GetRanges();
		for (const Junit& ju : R)
		{
			vector<Range> bounds = ju.GetRanges();
			for (int i = 0; i < BOUNDS.size(); ++i) BOUNDS[i] = Range::Cross(BOUNDS[i], bounds[i]);
		}

		long long int N = 1;
		for (const Range& r : BOUNDS)
		{
			assert(r.b != INT_INF);
			N *= r.b - r.a + 1;
		}
		string solution = (R.size() >= R[0].VariablesCount() ? "SOLVE  " : "ITERATE");
		cout << solution << " - Irterations : " << N;
		cout << "  EQUATIONS: " << R.size() << "  VARIABLES: " << R[0].VariablesCount() << endl;
	}
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
	for (int i = (int)max(ids.length(), LEN) - 1; 0 <= i; --i)
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
	int r = l + 1;
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
	l = ++r;
	while (line[r] != '}')
	{
		if (line[r] == ',')
		{
			m.J.push_back((INT)stoi(line.substr(l, r - l)));
			l = r + 1;
		}
		++r;
	}
	m.J.push_back((INT)stoi(line.substr(l, r - l)));

	// [B0(Jn-1, ..., J0),..., Bn-1(Jn-1,...,J0)] [S0,...,Sn-1]
	Jvoltage jv(m.B, m.J); 
	jv.Solve();

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
