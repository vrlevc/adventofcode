#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class ISolution
{
public:
	virtual int Result() const = 0;
	virtual void SetFail() = 0;
	virtual void Process(const string& line) = 0;
};

static void ReadPazzleInput(const string& filepath, ISolution* solution);

class  Solution : public ISolution
{
	int res = 0;
	int joltage = 0;
public:
	void SetFail() override { res = -1; }
	void Process(const string& line) override
	{
		joltage += MaxJoltage(line);
	}
	int Result() const override { return res; }
	void PrintResult() { cout << endl << "Maximum joltage: " << joltage << endl; }
private:
	int MaxJoltage(const string& line)
	{
		cout << "    Max joltage: " << 0 << endl;
		return 0;
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