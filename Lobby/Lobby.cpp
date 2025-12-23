#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std;

class ISolution
{
public:
	virtual int Result() const = 0;
	virtual void SetFail() = 0;
	virtual void Process(const string& line) = 0;
};

static void ReadPazzleInput(const string& filepath, ISolution* solution);

string SUM(const string& A, const string& B)
{
	int a = 0, b = 0, d = 0;
	string sum;
	auto i = A.rbegin(); auto j = B.rbegin();
	for (; i != A.rend() || j != B.rend();)
	{
		a = i != A.rend() ? *i - '0' : 0;
		b = j != B.rend() ? *j - '0' : 0;
		int s = a + b + d; d = s / 10;
		sum.push_back(char((int)'0' + (s % 10)));
		if (i != A.rend()) ++i;
		if (j != B.rend()) ++j;
	}
	if (d > 0) sum.push_back(char((int)'0' + d));
	for (int i = 0, j = (int)sum.length() - 1; i < j; ++i, --j) swap(sum[i], sum[j]);
	return sum;
}

class  Solution : public ISolution
{
	int res = 0;
	string sum;
public:
	Solution() : sum("0") {}
	void SetFail() override { res = -1; }
	void Process(const string& line) override
	{
		string jolgate = MaxJoltage(line);
		sum = SUM(sum, jolgate);
	}
	int Result() const override { return res; }
	void PrintResult() { cout << endl << "Maximum joltage: " << sum << endl; }
private:
	string MaxJoltage(const string& line)
	{
		string joltage_max = line.substr(line.length() - 12);
		assert(joltage_max.length() == 12);

		for (int i = line.length() - 12 - 1; 0 <= i; --i)
		{
			int new_dig = static_cast<int>(line[i] - '0');
			int cur_dig = static_cast<int>(joltage_max[0] - '0');
			if (new_dig >= cur_dig)
			{
				string joltage_cur;
				for (int j = 0; j < 12; ++j)
				{
					string cur = joltage_max;
					cur.erase(j, 1);
					cur = to_string(new_dig) + cur;
					if (joltage_cur.empty() || joltage_cur < cur) joltage_cur = cur;
				}
				if (joltage_max < joltage_cur) joltage_max = joltage_cur;
			}
		}

		cout << "    Max joltage: " << joltage_max << endl;
		return joltage_max;
	}
};

int main(int argc, char* argv[])
{
	Solution solution;
	if (argc == 2)
	{
		cout << "Read pazzle file: " << argv[1] << endl;
		ReadPazzleInput(argv[1], &solution);
		if (solution.Result() == 0)
		{
			solution.PrintResult();
		}
	}
	else
	{
		cout << "Error: Input parameter. Specify pazzle file path." << endl;
		return -1;
	}

	return solution.Result();
}

static void ReadPazzleInput(const string& filepath, ISolution* solution)
{
	ifstream puzzle(filepath);

	if (puzzle.is_open())
	{
		string line;
		while (getline(puzzle, line) && solution->Result() == 0)
		{
			cout << "Process line: " << line << endl;
			solution->Process(line);
		}

		puzzle.close();
	}
	else
	{
		cout << "Error: Open pazzle file.";
		solution->SetFail();
	}
}