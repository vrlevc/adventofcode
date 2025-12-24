#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <queue>

using namespace std;

int main(int argc, char* argv[])
{
    vector<vector<char>> table;

    ifstream puzzle(argv[1]);
    if (puzzle.is_open())
    {    
        string line;
        while (getline(puzzle, line))
        {
            int R = line.length(), C = 0;
            table.emplace_back(R); C = table.size() - 1;
            for (int i = 0; i < R; ++i) table[C][i] = line[i]=='@' ? 1 : 0;
        }
        puzzle.close();
    }

    struct v { int r, c; };
    array<v, 8> D{ { {+1, 0}, {+1, +1}, {0, +1}, {-1, +1}, {-1, 0}, {-1, -1}, {0, -1}, {+1, -1} } };

    int R = table.size(), C = table[0].size();

    queue<v> REM;

    // 1. Set into 1 cells actual values 
    for (int r = 0; r < R; ++r) for (int c = 0; c < C; ++c) if (table[r][c])
    {
        for (const auto d : D)
        {
            int cr = r + d.r, cc = c + d.c;
            if (0 <= cr && cr < R && 0 <= cc && cc < C && table[cr][cc]) ++table[r][c];
        }
        if (table[r][c] <= 4) REM.push({ r, c });
    }

    //for (const auto& r : table) { for (const auto& c : r) cout << (int)c; cout << endl; }
    
    int REMOVED = 0;

    // 2. loop removing...
    while (!REM.empty())
    {
        v cell = REM.front();
        for (const auto d : D)
        {
            int cr = cell.r + d.r, cc = cell.c + d.c;
            if (0 <= cr && cr < R && 0 <= cc && cc < C)
            {
                if (table[cr][cc] == 5) REM.push({ cr, cc });
                if (table[cr][cc] >= 5) --table[cr][cc];
            }
        }
        table[cell.r][cell.c] = 0;
        REM.pop();
        ++REMOVED;
    }

    //for (const auto& r : table) { for (const auto& c : r) cout << (int)c; cout << endl; }

    cout << "Rolls: " << REMOVED << endl;

    return 0;
}
