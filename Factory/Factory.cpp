#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include <chrono>

using namespace std;

using INT = int;
INT INT_INF = INT16_MAX;

class Machine
{
	INT L = 0;		// Indicator
	vector<INT> B;  // Buttons
	vector<INT> J;  // Jvoltage
public:
	int	MIN_JV = 0;
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
	Range(INT a, INT b) : a(a), b(b) {}
	
	void Print() const
	{
		if (b < a) cout << " O ";
		else cout << " [" << a << ".." << b << "] ";
	}
	void Cross(const Range& rhs) 
	{ 
		a = max(a, rhs.a); 
		b = min(b, rhs.b); 
	}
};

class ROW : public vector<INT>
{
public:
	ROW(INT S, const vector<INT> J) 
	{
		this->resize( J.size() + 1 ); // matrix row + value.
		int i = 0;
		while (i < J.size()) { this->operator[](i) = J[i]; ++i; }
		this->operator[](i) = S;
	}
	ROW& operator+=(const ROW& rhs)
	{
		assert(size() == rhs.size());
		for (int i = 0; i < size(); ++i) this->operator[](i) += rhs[i];
		return *this;
	}
	ROW& operator-=(const ROW& rhs)
	{
		assert(size() == rhs.size());
		for (int i = 0; i < size(); ++i) this->operator[](i) -= rhs[i];
		return *this;
	}
	ROW& operator*(INT k)
	{
		for (auto& v : *this) v *= k;
		return *this;
	}
	bool IsZero() const 
	{
		bool zero = true;
		for (auto v : *this) if (v != 0) { zero = false; break; }
		return zero;
	}
	void Print()
	{
		for (auto v : *this) cout << setw(6) << v; cout << endl;
	}
	bool Solved() const
	{
		bool solved = true;
		for (int i = 0; i < size() - 1; ++i) if ((*this)[i] != 1) { solved = false; break; }
		return solved;
	}
	Range GetRange(int i) const
	{
		bool positive = true;
		for (INT v : *this) if (v < 0) { positive = false; break; }
		INT v = (*this)[i], s = back();
		if (!positive || v == 0) return Range{ 0, INT_INF };
		return Range{ 0, (INT)((float)s / (float)v) };
	}
};

class MATRIX : public vector<ROW>
{
public:
	void Print()
	{
		for (const auto& row : *this)
		{
			for (const auto& v : row) cout << setw(6) << v;
			cout << endl;
		}
		cout << endl;
	}
	void SwapRows(int r0, int r1)
	{
		if (r0 == r1) return;
		std::swap(this->operator[](r0), this->operator[](r1));
	}
	void SwapCols(int c0, int c1)
	{
		if (c0 == c1) return;
		for (auto& row : *this) std::swap(row[c0], row[c1]);
	}
	int GetR() const { return (int)size(); }
	int GetC() const { return (int)(*this)[0].size() - 1; }
	bool SolveFor(int x) // swap rows and add it to make 0 in column c : return false if impossible.
	{
		int R = GetR(), C = GetC();
		
		if (x >= min(R, C)) return false;

		// search for the first non 0 in (x,x) ... (n,m)
		int r = x, c = x; bool done = false;
		for (; r < R; ++r) {
			for (c = x; c < C; ++c) {
				done = (abs((*this)[r][c]) == 1);
				if (done) break;
			}
			if (done) break;
		}

		if (r >= R) 
		{ 
			/*Print();*/ 
			if ((*this)[x][x] < 0) for (auto& v : (*this)[x]) v *= -1;
			return false; 
		}
		assert(r < R);
		
		// make [x,x] != 0
		SwapRows(x, r); SwapCols(x, c);
		if ((*this)[x][x] < 0) for (auto& v : (*this)[x]) v *= -1;

		//Print();

		// nullify colimn x - solwe for it:
		for (r = 0; r < size(); ++r) if (r != x && (*this)[r][x] != 0)
		{
			ROW row = (*this)[x];
			if ((*this)[r][x] > 0)
				(*this)[r] -= row * abs((*this)[r][x]);
			if ((*this)[r][x] < 0)
				(*this)[r] += row * abs((*this)[r][x]);
			
			NormalizeRow(r);
		}

		// remove 0 rows if any
		erase_if(*this, [](ROW& r) { return r.IsZero(); });

		return true;
	}
	void NormalizeRow(int r)
	{
		int GCD = 1;
		for (const auto gcd : (*this)[r])
		{	
			if (gcd != 0)
			{
				bool isGCD = true;
				for (const auto v : (*this)[r]) if (v % gcd != 0) { isGCD = false; break; }
				if (isGCD) GCD = max(GCD, abs(gcd));
			}
		}
		if (GCD != 1) for (auto& v : (*this)[r]) v /= GCD;

		bool all_negative = true;
		for (auto v : (*this)[r]) if (v > 0) { all_negative = false; break; }
		if (all_negative) (*this)[r] = (*this)[r] * -1;
	}
	INT MaxS() const 
	{
		INT max_s = (*this)[0].back();
		for (const auto& r : *this) max_s = max(max_s, (INT)abs(r.back()));
		return max_s;
	}
	int Solution(vector<INT> X)
	{
		// Test last row coof: 
		const ROW& last = back();
		if (last[size() - 1] != 1)
		{
			INT last_x = last.back();
			int x_idx = min(GetR(), GetC());
			for (int di = x_idx; di < GetC(); ++di)
			{
				last_x -= last[di] * X[di - x_idx];
			}
			// must be integer
			if (last_x % last[size() - 1] != 0) return -1; // incompatible
			if (last_x / last[size() - 1] < 0 ) return -1; // incompatible

			vector<INT> _X{ last_x / last[size() - 1] };
			for (const auto& x : X) _X.push_back(x);
			return SolutionCalc(_X, true);
		}
		return SolutionCalc(X, false);
	}
	int SolutionCalc(vector<INT> X, bool skip_last)
	{
		int SUM = 0;
		for (const auto& x : X) SUM += x;
		int x_idx = min(GetR(), GetC()) + (skip_last ? -1 : 0);
		for (int r = 0; r < size(); ++r)
		{
			if (skip_last && r == size() - 1) break;
			ROW& row = (*this)[r];
			int xv = row.back();
			for (int di = x_idx; di < GetC(); ++di) xv -= row[di] * X[di - x_idx];
			if (xv < 0) return -1;
			SUM += xv;
		}
		return SUM;
	}

	INT Value(int xi) const { return (*this)[xi][xi]; }
};

class Jvoltage
{
	MATRIX M;
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
			ROW junit(s, r);
			bool add = true;
			for (const ROW& ju : M) if (ju == junit) { add = false; break; }
			if (add) M.push_back(junit);
		}
	}

	int Solve()
	{
		bool solved = false;
		static int ID = 0;
		//M.Print();
		int solutions = min(M.GetR(), M.GetC());
		for (int x = 0; x < solutions; ++x)
		{
			solved = M.SolveFor(x);
		}
		//M.Print();
		ROW SUM = M[0];
		for (int i = 1; i < M.size(); ++i) SUM += M[i];

		//assert(SUM.Solved() == solved);
		if (!SUM.Solved())
		{
			//M.Print(); cout << endl;
			int variables = M.GetC() - min(M.GetR(), M.GetC());
			//cout << "Variables : " << variables << endl;
			int MAX_I = 200;
			if (variables == 1)
			{
				int min_sum = INT_MAX;
				for (int x = 0; x < MAX_I; ++x)
				{
					int sum = M.Solution({ x });
					if (sum >= 0) min_sum = min(sum, min_sum);
				}
				cout << ++ID << " MIN SUM : " << min_sum << endl;
				assert(min_sum > 0);
				if (min_sum == INT_MAX) { M.Print(); cout << endl; }
				return min_sum;
			} 
			else if (variables == 2)
			{
				int min_sum = INT_MAX;
				for (int x0 = 0; x0 < MAX_I; ++x0) 
				for (int x1 = 0; x1 < MAX_I; ++x1)
				{
					int sum = M.Solution({ x0, x1 });
					if (sum >= 0) min_sum = min(sum, min_sum);
				}
				cout << ++ID << " MIN SUM : " << min_sum << endl;
				assert(min_sum > 0);
				if (min_sum == INT_MAX) { M.Print(); cout << endl; }
				return min_sum;
			}
			else if (variables == 3)
			{
				int min_sum = INT_MAX;
				for (int x0 = 0; x0 < MAX_I; ++x0) 
				for (int x1 = 0; x1 < MAX_I; ++x1) 
				for (int x2 = 0; x2 < MAX_I; ++x2)
				{
					int sum = M.Solution({ x0, x1, x2 });
					if (sum >= 0) min_sum = min(sum, min_sum);
				}
				cout << ++ID << " MIN SUM : " << min_sum << endl;
				assert(min_sum > 0);
				if (min_sum == INT_MAX) { M.Print(); cout << endl; }
				return min_sum;
			}

		}
		else
		{
			cout << ++ID << " MIN SUM : " << SUM.back() << endl;
			return SUM.back();
		}

		return 0;
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
	m.MIN_JV += jv.Solve();

	return m;
}

void Factory::ReadFactory(const char* file)
{
	ifstream puzzle(file);
	if (puzzle.is_open())
	{
		int max_buttons = 0;
		int min_buttons = 0;
		string line;
		while (getline(puzzle, line))
		{
			Machine m = Machine::ReadMachine(line);
			max_buttons = max(max_buttons, m.ButtonsNum());
			INT sum = m.MinCombination();
			// cout << "MIN Buttons : " << sum << endl;
			SUM += sum;
			min_buttons += m.MIN_JV;
		}
		puzzle.close();
		cout << "Max buttons : " << max_buttons << endl;
		cout << "MIN JVOLAGE : " << min_buttons << endl;
	}
}
