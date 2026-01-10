#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <numeric>

using namespace std;

class Puzzle
{
public:
    static constexpr int SHAPE_SIZE = 3;
    static constexpr int SHAPES_NUM = 6;
    using Shape = array<array<char, SHAPE_SIZE>, SHAPE_SIZE>;  // 3x3 shape of a present.
    using Shapes = array<Shape, SHAPES_NUM>;                   // presents of 6 different shapes.
	using ShapesWeight = array<char, SHAPES_NUM>;              // filled part of the shapes.

    struct Square { int width, length; };
    using Presents = array<short, SHAPES_NUM>; // number of presents of each shape.    
    struct Tree 
    { 
        Square squares; 
        Presents presents; 
        int PresentsCount() const { return accumulate(begin(presents), end(presents), 0); }
        int Area3X3() const { return (squares.width / SHAPE_SIZE) * (squares.length / SHAPE_SIZE); }
		int Area() const { return squares.width * squares.length; }
        bool FeetAbs3X3() const { return PresentsCount() <= Area3X3(); }
        int PresentsWeight(const ShapesWeight& shape_wights) const { 
            int w = 0; for (int s = 0; s < SHAPES_NUM; ++s) w += presents[s] * shape_wights[s]; return w; }
        bool UnfeetByWeight(const ShapesWeight& shape_wights) const { return PresentsWeight(shape_wights) > Area(); }
    };
    using Trees = vector<Tree>;

public:
    void ReadPuzzle(const char* file);
    void PrintInformation() const;
private:


private:
    Shapes shapes;
    ShapesWeight shape_wights;
    Trees trees;
};

int main(int argc, char* argv[])
{
	Puzzle puzzle;
    puzzle.ReadPuzzle(argv[1]);
	puzzle.PrintInformation();
	return 0;
}

void Puzzle::PrintInformation() const
{
    int feet_without_collisions = 0;
    int unfeet_by_weight = 0;
    for (const auto& tree : trees)
    {
        if (tree.FeetAbs3X3()) ++feet_without_collisions;
		if (tree.UnfeetByWeight(shape_wights)) ++unfeet_by_weight;
        if (!tree.FeetAbs3X3() && !tree.UnfeetByWeight(shape_wights))
        {
            cout << " available 3x3 squares = " << tree.Area3X3();
            cout << " required 3x3 squares = " << tree.PresentsCount();
			cout << " available weight = " << tree.Area();
            cout << " presents weight = " << tree.PresentsWeight(shape_wights);
            cout << endl;
        }
	}    
	cout << "\n\nTrees without collisions: " << feet_without_collisions << endl;
	cout << "Unfeet by weight: " << unfeet_by_weight << endl;
}

void Puzzle::ReadPuzzle(const char* file)
{
    ifstream puzzle(file);
    if (puzzle.is_open())
    {
        string line;

        // Read shapes.
        for (Shape& shape : shapes)
        {
            getline(puzzle, line); // shape id: - just skip it
            for (int r = 0; r < SHAPE_SIZE; ++r)
            {
                getline(puzzle, line);
                for (int c = 0; c < SHAPE_SIZE; ++c)
                {
                    shape[r][c] = line[c] == '#' ? 1 : 0;
                }
            }
            getline(puzzle, line); // empty line - just skip it
        }

		for (int s = 0; s < SHAPES_NUM; ++s) 
            shape_wights[s] = accumulate(
                begin(shapes[s]), end(shapes[s]), 0, 
                [](int v, array<char, SHAPE_SIZE>& a) { 
                    return accumulate(begin(a), end(a), v); });

        // Read squares and presents numbers.
        while (getline(puzzle, line))
        {
            Square square{ stoi(line.substr(0, 2)), stoi(line.substr(3, 2)) };
            Presents presents;
            for (int i = 0; i < SHAPES_NUM; ++i)
                presents[i] = stoi(line.substr(7 + i * 3, 2));
            trees.push_back({ square, presents });
        }

        puzzle.close();
    }
}


