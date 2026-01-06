#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <stack>
#include <unordered_set>
#include <assert.h>
#include <numeric>

using namespace std;
using INT = long long int;

class SolutionPart1
{
    class Node
    {
        string name;
        vector<string> next;
        vector<INT> next_ids;
        vector<INT> path_num; // current path number for this case.
        long paths = -1;           // total paths number = sum of path_num
    public:
        Node(const string& name) : name(name) {}
        const string& Name() const { return name; }
        void AddNext(string name) { next.push_back(name); }
        bool operator<(const Node& rhs) const { return name < rhs.name; }
        const vector<string>& Next() const { return next; }
        void Resolve(vector<INT>&& next) { next_ids = next; this->next.clear(); path_num = vector<INT>(next_ids.size(), -1); }
        const vector<INT>& NextIds() const { return next_ids; }
        void UpdateCache(INT id, INT num) { assert(0 <= id && id < path_num.size()); path_num[id] = num; }
        void GatherCache() 
        { 
            paths = accumulate(begin(path_num), end(path_num), 0); 
            //cout << name << " : " << paths << endl; 
        }
        INT GetCache() const { return paths; }
        void Init() { path_num = vector<INT>(next_ids.size(), -1); paths = -1; }
    };

    struct node_state { INT id, next_i; };

public:
    SolutionPart1(const char* pazzle_file) : pazzle(pazzle_file) {}
    void InitSolution()
    {
        ReadGrapFromFile();
        ResolveStrings();
    }
    long long int SolutionPart2(const char* start, const char* finish)
    {
        InitGraph();
        long long int res = PathsNumber(start, finish);
        cout << "Paths " << start << " -> " << finish << " " << res << endl;
        return res;
    }

private:
    string pazzle;
    vector<Node> graph;
    void ReadGrapFromFile()
    {
        ifstream puzzle(pazzle);
        if (puzzle.is_open())
        {
            string line;
            while (getline(puzzle, line))
            {
                INT l = 0, r = 0;
                while (r < line.size() && line[r] != ':') ++r;
                graph.push_back(line.substr(l, r - l)); ++r; ++r; l = r;
                while (r < line.size()) { if (line[r] == ' ') { graph.back().AddNext(line.substr(l, r - l)); ++r; l = r; } else ++r; }
                graph.back().AddNext(line.substr(l, r - l));
            }
            puzzle.close();
        }
    }
    void ResolveStrings()
    {
        graph.push_back({ "out" });
        ranges::sort(graph, [](const Node& a, const Node& b) { return a < b; });
        for (Node& node : graph)
        {
            vector<INT> next;
            for (const string& n : node.Next())
            {
                auto next_id = (INT)distance(begin(graph), ranges::lower_bound(graph, Node{ n }, [](const Node& a, const Node& b) { return a < b; }));
                next.push_back(next_id);
            }
            node.Resolve(move(next));
        }
    }
    INT PathsNumber(const string& start, const string& finish)
    {
        INT start_id = (INT)distance(begin(graph), ranges::lower_bound(graph, Node{ start }, [](const Node& a, const Node& b) { return a < b; }));
        INT finish_id = (INT)distance(begin(graph), ranges::lower_bound(graph, Node{ finish }, [](const Node& a, const Node& b) { return a < b; }));

        unordered_set<INT> visited;
        stack<node_state> state; state.push({ start_id, 0 }); visited.insert(start_id);
        while (!state.empty())
        {
            Node& curr_node = graph[state.top().id];
            if (state.top().next_i < curr_node.NextIds().size())
            {
                INT next_id = curr_node.NextIds()[state.top().next_i];
                if (graph[next_id].GetCache() != -1) // node calculation done.
                {
                    curr_node.UpdateCache(state.top().next_i, graph[next_id].GetCache());
                    state.top().next_i += 1;
                }
                else if (next_id == finish_id)
                {
                    // way is found - update total number of ways and cache it
                    curr_node.UpdateCache(state.top().next_i, 1);
                    state.top().next_i += 1;
                }
                else if (visited.contains(next_id))
                {
                    //Print(state, graph, "...");
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
                curr_node.GatherCache();
                INT curr_node_cache = curr_node.GetCache();
                visited.erase(state.top().id);
                state.pop();
                if (!state.empty())
                {
                    Node& prev_node = graph[state.top().id];
                    prev_node.UpdateCache(state.top().next_i, curr_node_cache);
                    state.top().next_i += 1;
                }
            }
        }

        return graph[start_id].GetCache();
    }
    void Print(const stack<node_state>& path, string last)
    {
        stack<node_state> path_copy = path;
        stack<string> out_path;
        while (!path_copy.empty()) { out_path.push(graph[path_copy.top().id].Name()); path_copy.pop(); }
        while (!out_path.empty()) { cout << out_path.top() << ","; out_path.pop(); }
        cout << last << endl;
    }
    void InitGraph() { for (auto& n : graph) n.Init(); }
};

#include <chrono>
int main(int argc, char* argv[])
{
    auto start = std::chrono::high_resolution_clock::now();

    SolutionPart1 sol1(argv[1]);
    sol1.InitSolution();
    sol1.SolutionPart2("svr", "out");

    long long int svr_fft = sol1.SolutionPart2("svr", "fft");
    long long int svr_dac = sol1.SolutionPart2("svr", "dac");
    long long int dac_fft = sol1.SolutionPart2("dac", "fft");
    long long int fft_dac = sol1.SolutionPart2("fft", "dac");
    long long int dac_out = sol1.SolutionPart2("dac", "out");
    long long int fft_out = sol1.SolutionPart2("fft", "out"); 
    cout << endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    long long int srv_fft_not_dac = svr_fft - svr_dac * dac_fft;
    cout << "svr -> fft not dac : " << srv_fft_not_dac << endl;
    cout << "fft -> dac         : " << fft_dac << endl;
    long long int dac_out_not_fft = dac_out - dac_fft * fft_out;
    cout << "dac -> out not fft : " << dac_out_not_fft << endl;
    cout << endl;

    long long int srv_dac_not_fft = svr_dac - svr_fft * fft_dac;
    cout << "svr -> dac not fft : " << srv_dac_not_fft << endl;
    cout << "dac -> fft         : " << dac_fft << endl;
    long long int fft_out_not_dac = fft_out - fft_dac * dac_out;
    cout << "fft -> out not dac : " << fft_out_not_dac << endl;
    cout << endl;

    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}
    


