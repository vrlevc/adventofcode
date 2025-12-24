#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

auto less_id = [](const string& a, const string& b)
    {
        if (a.length() < b.length()) return true;
        if (a.length() > b.length()) return false;
        return a < b;
    };

struct Range { string a, b; };
static void FreshIDs(vector<Range>& Ranges, vector<string>& Values);
static void FreshRanges(vector<Range>& Ranges);
string SUB(const string& _A, const string& _B);
string SUM(const string&  A, const string&  B);

int main(int argc, char* argv[])
{
    vector<Range>  Ranges;
    vector<string> Values;

    ifstream puzzle(argv[1]);
    if (puzzle.is_open())
    {
        enum read { interval, values };
        read state = interval;
        string line;
        
        
        while (getline(puzzle, line))
        {
            if (line.empty()) state = values;
            else if (state == interval)
            {
                for (int i = 0; i < line.length(); ++i) if (line[i] == '-')
                {
                    Ranges.push_back( Range{ line.substr(0, i), line.substr(i+1) } );
                    break;
                }
            }
            else if (state == values)
            {
                Values.push_back( line );
            }
        }
        puzzle.close();
    }

    // FreshIDs(Ranges, Values);
    FreshRanges(Ranges);

    return 0;
}

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

void SWAP(string& A) { for (int i = 0, j = (int)A.length() - 1; i < j; ++i, --j) swap(A[i], A[j]); }

string SUB(const string& _A, const string& _B) // A-B where B<=A
{
    if (_A == _B) return "0";
    string A = _A, B = _B;
    SWAP(A); SWAP(B);
    int a, b, d = 0;
    for (int i = 0; i < A.length(); ++i)
    {
        a = A[i]-'0'; b = d + (i<B.length() ? B[i]-'0' : 0);
        a -= b; d = 0; if (a < 0) { a += 10; d = 1; }
        A[i] = a + '0';
    }
    while (A.back() == '0') A.pop_back();
    SWAP(A);
    return A;
}

static void FreshRanges(vector<Range>& Ranges)
{
    string valid_num = "0";

    auto less_range = [](const Range& a, const Range& b)
        {
            return less_id(a.a, b.a);
        };

    ranges::sort(Ranges, less_range);

    auto cur = begin(Ranges);
    while (cur != end(Ranges))
    {
        Range res = *cur;
        auto first = cur + 1;
        auto next = first;

        do
        {
            auto next = upper_bound(first, end(Ranges), Range{ res.b, "" }, less_range);
            if (distance(first, next) > 0)
            {
                for (auto it = first; it != next; ++it) if (less_id(res.b, it->b)) res.b = it->b;
                first = next;
            }
            else break;
        } while (first!=end(Ranges));

        cout << res.a << "-" << res.b << " => " << SUM(SUB(res.b, res.a), "1") << endl;
        valid_num = SUM(valid_num, SUM(SUB(res.b, res.a), "1"));

        cur = first;
    }

    cout << "\nSUM : " << valid_num << endl;
}

static void FreshIDs(vector<Range>& Ranges, vector<string>& Values)
{

    ranges::sort(Values, less_id);

    int REM = 0;
    for (const auto& range : Ranges)
    {
        auto ia = ranges::lower_bound(Values, range.a, less_id);
        auto ib = ranges::upper_bound(Values, range.b, less_id);
        if (ia != end(Values))
        {
            REM += ranges::distance(ia, ib);
            Values.erase(ia, ib);
        }
        if (Values.empty()) break;
    }
    cout << "Fresh : " << REM << endl;
}