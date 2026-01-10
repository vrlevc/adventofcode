#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>

using namespace std;

class Puzzle
{
public:
    static constexpr int SHAPE_SIZE = 3;
    static constexpr int SHAPES_NUM = 6;
    using Shape = array<array<char, SHAPE_SIZE>, SHAPE_SIZE>;  // 3x3 shape of a present.
    using Shapes = array<Shape, SHAPES_NUM>;                   // presents of 6 different shapes.

    struct Square { int width, length; };
    using Presents = array<short, SHAPES_NUM>; // number of presents of each shape.    
    struct Tree { Square squares; Presents presents; };
    using Trees = vector<Tree>;

public:
    void ReadPuzzle(const char* file);


private:
    Shapes shapes;
    Trees trees;
};

int main(int argc, char* argv[])
{
	Puzzle puzzle;
    puzzle.ReadPuzzle(argv[1]);
	return 0;
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
