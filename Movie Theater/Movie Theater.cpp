#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <assert.h>
#include <iomanip>

using namespace std;
using INT = int;

struct Point { INT x, y; };
struct Range { INT min, max; };

class Area
{
	vector<Point> red; // red squares positions
	INT X = 0, Y = 0;  // Area size
	vector<vector<char>> area;
	vector<vector<INT>> matrix;
	bool print = true;
public:
	Area(bool print) : print(print) {}
	void ReadRedPoints(const char* file);
	void Initialize();
	void Print();
	void CreateMatrix();
	void PrintMatrix();
	long CalculateMaxRect();
private:
	void InitializeAreaSize(); // define area size by red points coords
	void InitializeArea();	   // fill area with red points
	void InitializeLines();    // Add vertical and horizontal lines 
	void InitializeOutArea();  // Mark out area - invalid area 
	bool IsCellOut(INT y, INT x);
};

int main(int argc, char* argv[])
{
	Area area(false);
	area.ReadRedPoints(argv[1]);
	area.Initialize();
	area.Print();
	cout << endl;
	area.CreateMatrix();
	area.PrintMatrix();
	cout << endl;
	long square = area.CalculateMaxRect();
	cout << "MAX Valid Square: " << square << endl;
}

void Area::ReadRedPoints(const char* file)
{
	ifstream puzzle(file);
	if (puzzle.is_open())
	{
		string line;
		while (getline(puzzle, line))
		{
			INT x = 0LL, y = 0LL;
			for (int i = 0; i < line.size(); ++i) if (line[i] == ',')
			{
				red.push_back({ stoi(line.substr(0, i)), stoi(line.substr(1 + i)) });
				break;
			}
		}
		puzzle.close();
	}
}

void Area::Initialize() 
{
	InitializeAreaSize();
	InitializeArea();
	InitializeLines();
	InitializeOutArea();
}

void Area::InitializeAreaSize()
{
	INT x = red[0].x, y = red[0].y;
	for (const Point& p : red) { x = min(x, p.x); y = min(y, p.y); }
	for (Point& p : red) { p.x -= x; p.y -= y; }
	for (const Point& p : red) { X = max(X, p.x); Y = max(Y, p.y); }
	++X; ++Y;
}

void Area::InitializeArea()
{
	vector<char> row(X, '.');
	area = vector<vector<char>>(Y, row);
	for (const Point& r : red) area[r.y][r.x] = '#';
}

void Area::InitializeLines()
{
	ranges::sort(red, [](const Point& a, const Point& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });
	for (INT i = 0; i < (INT)red.size() - 1; i += 2)
	{
		const Point& a = red[i], &b = red[i + 1];
		for (INT y = a.y + 1; y <= b.y - 1; ++y) area[y][a.x] = 'X';
	}

	ranges::sort(red, [](const Point& a, const Point& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); });
	for (INT i = 0; i < (INT)red.size() - 1; i += 2)
	{
		const Point& a = red[i], & b = red[i + 1];
		for (INT x = a.x + 1; x <= b.x - 1; ++x) area[a.y][x] = 'X';
	}
}

void Area::InitializeOutArea()
{
	INT y0 = 0, y1 = Y - 1, x0 = 0, x1 = X - 1;
	bool OUT = true;
	do
	{
		for (INT x = x0; x <= x1; ++x)
		{
			if (area[y0][x] == '.' && (OUT || IsCellOut(y0, x))) area[y0][x] = 'O';
			if (area[y1][x] == '.' && (OUT || IsCellOut(y1, x))) area[y1][x] = 'O';
		}
		for (INT y = y0; y <= y1; ++y)
		{
			if (area[y][x0] == '.' && (OUT || IsCellOut(y, x0))) area[y][x0] = 'O';
			if (area[y][x1] == '.' && (OUT || IsCellOut(y, x1))) area[y][x1] = 'O';
		}
		OUT = false;
		++y0; --y1; ++x0; --x1;
	} while (y0 <= y1 && x0 <= x1);
}

bool Area::IsCellOut(INT y, INT x)
{
	if (0 <= y - 1 && area[y - 1][x] == 'O') return true;
	if (0 <= x - 1 && area[y][x - 1] == 'O') return true;
	if (Y >  y + 1 && area[y + 1][x] == 'O') return true;
	if (X >  x + 1 && area[y][x + 1] == 'O') return true;
	return false;
}

void Area::Print()
{
	if (!print) return;
	for (const auto& row : area)
	{
		for (const char c : row) cout << c; 
		cout << endl;
	}
}

void Area::CreateMatrix()
{
	vector<INT> row(X, 0);
	matrix = vector<vector<INT>>(Y, row);
	auto Fn = [&](INT y, INT x) { if (0 <= x && x < X && 0 <= y && y < Y) return matrix[y][x]; return 0; };
	for (INT r = 0; r < Y; ++r) for (INT c = 0; c < X; ++c)
		matrix[r][c] = (area[r][c] == 'O' ? 1 : 0) + Fn(r - 1, c) + Fn(r, c - 1) - Fn(r - 1, c - 1);
}

void Area::PrintMatrix()
{
	if (!print) return;
	for (const auto& row : matrix)
	{
		for (const INT& cell : row) cout << fixed << setw(6) << to_string(cell);
		cout << endl;
	}
}

long Area::CalculateMaxRect()
{
	auto Fn = [&](INT y, INT x) { if (0 <= x && x < X && 0 <= y && y < Y) return matrix[y][x]; return 0; };
	long SQUARE = 0;
	for (int i = 0; i < red.size() - 1; ++i) for (int j = i + 1; j < red.size(); ++j)
	{
		INT w = abs(red[i].x - red[j].x) + 1, h = abs(red[i].y - red[j].y) + 1;
		INT x = max(red[i].x, red[j].x), y = max(red[i].y, red[j].y);
		long square = w * h;
		if (SQUARE < square)
		{
			INT out = Fn(y, x) - Fn(y - h, x) - Fn(y, x - w) + Fn(y - h, x - w);
			if (out == 0) SQUARE = square;
			if (print && out==0)
			{
				cout << "a[" << red[i].x << ":" << red[i].y << "] b[" << red[j].x << ":" << red[j].y << "]" << endl;
				cout << "x:" << x << " y:" << y << " w:" << w << " h:" << h << endl;
				cout << "Fn(" << y << "," << x << ")=" << Fn(y, x) << endl;
				cout << "Fn(" << y - h << "," << x << ")=" << Fn(y - h, x) << endl;
				cout << "Fn(" << y << "," << x - w << ")=" << Fn(y, x - w) << endl;
				cout << "Fn(" << y - h << "," << x - w << ")=" << Fn(y - h, x - w) << endl;
				cout << "Square = " << square << endl << endl;
			}
		}
	}
	return SQUARE;
}