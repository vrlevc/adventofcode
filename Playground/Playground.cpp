#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <cmath>
#include <list>
#include <set>
#include <algorithm>

using namespace std;

using LLI = long long int;

int main(int argc, char* argv[])
{
    auto up = [](const vector<LLI>& a, const vector<LLI>& b) { return a[0] < b[0]; };
    priority_queue<vector<LLI>, vector<vector<LLI>>, decltype(up)> circuits{ up };
    vector<vector<LLI>> b;
    set<LLI> ids;

    ifstream puzzle(argv[1]);
    if (puzzle.is_open())
    {
        string line;
        while (getline(puzzle, line))
        {
            vector<LLI> p(3, 0);
            LLI i = 0, j = 0;
            for (; i < (LLI)line.length(); ++i) if (line[i] == ',') { p[0] = stoi(line.substr(0, i++)); j = i; break; }
            for (; i < (LLI)line.length(); ++i) if (line[i] == ',') { p[1] = stoi(line.substr(j, i++)); j = i; break; }
            p[2] = stoi(line.substr(j));
            b.push_back(p);
        }
        puzzle.close();
    }

    for (int i = 0; i < b.size(); ++i) ids.insert(i);

    auto D = [](const vector<LLI>& a, const vector<LLI>& b)
        {
            return ((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
        };

    for (LLI i = 0; i < (int)b.size() - 1; ++i) for (LLI j = i + 1; j < (int)b.size(); ++j)
    {
        circuits.push({ D(b[i],b[j]), i, j });
    }

    vector<vector<LLI>> pairs(circuits.size()); // use for link, contains pairs

    LLI i = (LLI)pairs.size();
    while (!circuits.empty())
    {
        pairs[--i] = { circuits.top()[1], circuits.top()[2] };
        circuits.pop();
    }

    list<set<LLI>> groups; vector<LLI> last_pair;
    for (const auto& p : pairs)
    {
        ids.erase(p[0]); ids.erase(p[1]);
        last_pair = p;
        list<set<LLI>>::iterator pa = end(groups), pb = end(groups);
        for (auto it = begin(groups); it != end(groups) && (pa == end(groups) || pb == end(groups)); ++it)
        {
            if (it->contains(p[0])) pa = it;
            if (it->contains(p[1])) pb = it;
        }

        if (pa != end(groups) && pb != end(groups)) // merge groups
        {
            if (pa != pb) { pa->merge(*pb); groups.erase(pb); }
        }
        else if (pa != end(groups) || pb != end(groups)) // add to group
        {
            if (pa != end(groups)) pa->insert(p[1]); else pb->insert(p[0]); 
        }
        else // new group
        {
            groups.push_back({ p[0], p[1] }); 
        }
        if (ids.empty()) break;
    }

    cout << "MULT : " << b[last_pair[0]][0] * b[last_pair[1]][0] << endl;

    return 0;
}
