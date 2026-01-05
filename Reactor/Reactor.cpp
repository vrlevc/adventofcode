#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace std;

class Node
{
    string name;
    vector<string> next;
    vector<int> next_ids;
public:
    Node(const string& name) : name(name) {}
    const string& Name() const { return name; }
    void AddNext(string name) { next.push_back(name); }
    bool operator<(const Node& rhs) const { return name < rhs.name; }
    const vector<string>& Next() const { return next; }
    void Resolve(vector<int>&& next) { next_ids = next; this->next.clear(); }
    const vector<int>& NextIds() const { return next_ids; }
};

struct node_state { int id, next_i; };

void Print(const stack<node_state>& path, const vector<Node>& graph, string last)
{
    stack<node_state> path_copy = path;
    stack<string> out_path;
    while (!path_copy.empty()) { out_path.push( graph[path_copy.top().id].Name() ); path_copy.pop(); }
    while (!out_path.empty()) { cout << out_path.top() << ","; out_path.pop(); }
    cout << last << endl;
}

int PathsNumber(const vector<Node>& graph, const string& start, const string& finish, const vector<string>& ignore);

int main(int argc, char* argv[])
{
    vector<Node> graph;
    ifstream puzzle(argv[1]);
    if (puzzle.is_open())
    {
        string line;
        while (getline(puzzle, line))
        {
            int l = 0, r = 0;
            while (r < line.size() && line[r] != ':') ++r;
            graph.push_back(line.substr(l, r - l)); ++r; ++r; l = r;
            while (r < line.size()) { if (line[r] == ' ') { graph.back().AddNext(line.substr(l, r - l)); ++r; l = r; } else ++r; }
            graph.back().AddNext(line.substr(l, r - l));
        }
        puzzle.close();
    }

    graph.push_back({ "out" });
    ranges::sort(graph, [](const Node& a, const Node& b) { return a < b; });
    for (Node& node : graph)
    {
        vector<int> next;
        for (const string& n : node.Next())
        {
            auto next_id = (int)distance(begin(graph), ranges::lower_bound(graph, Node{ n }, [](const Node& a, const Node& b) { return a < b; }));
            next.push_back(next_id);
        }
        node.Resolve(move(next));
    }

    int srv_fft = 0;
    int fft_dac = 0;
    int dac_out = 0;

    //PathsNumber(graph, "you", "out", {});

    srv_fft = PathsNumber(graph, "you", "fft", { "out", "dac" });
    if (true || srv_fft)
        fft_dac = PathsNumber(graph, "fft", "dac", { "out", "you" });
    if (true || srv_fft && fft_dac)
        dac_out = PathsNumber(graph, "dac", "out", { "fft", "you" });

    int srv_dac = 0;
    int dac_fft = 0;
    int fft_out = 0;

    srv_dac = PathsNumber(graph, "you", "dac", { "out", "fft" });
    if (true || srv_dac)
        dac_fft = PathsNumber(graph, "dac", "fft", { "out", "you" });
    if (true || srv_dac && dac_fft)
        fft_out = PathsNumber(graph, "fft", "out", { "dac", "you" });

    int way_num = srv_fft * fft_dac * dac_out + srv_dac * dac_fft * fft_out;
    cout << "Paths Number : " << way_num << endl;
    return 0;
}
    
int PathsNumber(const vector<Node>& graph, const string& start, const string& finish, const vector<string>& ignore)
{
    int start_id = (int)distance(begin(graph), ranges::lower_bound(graph, Node{ start }, [](const Node& a, const Node& b) { return a < b; }));
    int finish_id = (int)distance(begin(graph), ranges::lower_bound(graph, Node{ finish }, [](const Node& a, const Node& b) { return a < b; }));

    unordered_set<int> visited;
    for (const string& node : ignore)
        visited.insert((int)distance(begin(graph), ranges::lower_bound(graph, Node{ node }, [](const Node& a, const Node& b) { return a < b; })));

    int way_num = 0;
    stack<node_state> state; state.push({ start_id, 0 }); visited.insert(start_id);
    while (!state.empty())
    {
        if (state.top().next_i < graph[state.top().id].NextIds().size())
        {
            int next_id = graph[state.top().id].NextIds()[state.top().next_i];
            if (next_id == finish_id) 
            { 
                Print(state, graph, "out");
                ++way_num;
                state.top().next_i += 1; 
            }
            else if (visited.contains(next_id)) 
            { 
                Print(state, graph, "...");
                state.top().next_i += 1; 
            }
            else
            {
                state.push({ next_id, 0 });
                visited.insert(next_id);
            }
        }
        else
        {
            visited.erase(state.top().id);
            state.pop();
            if (!state.empty()) state.top().next_i += 1;
        }
    }
    
    return way_num;
}

