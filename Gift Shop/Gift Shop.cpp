#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <unordered_set>

using namespace std;

static int read_pazzle(const string& filepath, function<void(const string& a, const string& b)> process_range);

class Number
{
protected:
	string value;
public:
	Number() : value("0") {}
	Number(const string& value) : value(value) {}
	string String() const { return value; }

	bool operator<(const Number& other) const
	{
		if (value.length() < other.value.length()) return true;
		if (value.length() > other.value.length()) return false;
		return value < other.value;
	}

	bool operator>(const Number& other)
	{
		return !(*this < other);
	}

	bool operator==(const Number& other)
	{
		return other.value == value;
	}

	bool operator<=(const Number& other)
	{
		return *this < other || *this == other;
	}

	Number& operator++()
	{
		int d = 1;
		for (int i = (int)value.length() - 1; 0 <= i && d == 1; --i)
		{
			if (value[i] == '9') { value[i] = '0'; }
			else { d = 0; value[i] = char(int(value[i]) + 1); }
		}
		if (d == 1) value = "1" + value;
		return *this;
	}

	Number& operator--()
	{
		int d = 1;
		for (int i = (int)value.length() - 1; 0 <= i && d == 1; --i)
		{
			if (value[i] == '0') { value[i] = '9'; }
			else { d = 0; value[i] = char(int(value[i]) - 1); }
		}
		if (value[0] == '0') value = value.substr(1);
		return *this;
	}

	Number& operator+=(const Number& other)
	{
		int a = 0, b = 0, d = 0;
		string sum;
		auto i = value.rbegin(); auto j = other.value.rbegin();
		for (; i != value.rend() || j != other.value.rend();)
		{
			a = i != value.rend() ? *i - '0' : 0;
			b = j != other.value.rend() ? *j - '0' : 0;
			int s = a + b + d; d = s / 10;
			sum.push_back(char((int)'0' + (s % 10)));
			if (i != value.rend()) ++i;
			if (j != other.value.rend()) ++j;
		}
		if (d > 0) sum.push_back(char((int)'0' + d));
		for (int i = 0, j = (int)sum.length() - 1; i < j; ++i, --j) swap(sum[i], sum[j]);
		value = sum;
		return *this;
	}

	int Length() { return (int)value.length(); }

	Number SubNum(int pos, int len = string::npos)
	{
		return Number( value.substr(pos, len) );
	}
};

class ID : public Number
{
	string link;
	int period = 0;
public:
	ID() : Number(), link("0"), period(1) {}
	ID(const string& id, int period = 1) : link(id), period(period)
	{
		value.clear();
		while (period--) value.append(id);
	}
	ID(const Number& id, int period = 1) : ID(id.String(), period) {}

	// invalid id greater or equal to this
	ID upper_bound_2()
	{
		if (Length() % 2 != 0) {
			string v( Length()/2 + 1, '0' );
			v[0] = '1';
			return ID(v, 2);
		}
		Number l( SubNum(0, Length() / 2) );
		Number r( SubNum(Length() / 2) );
		if (l < r) ++l;
		return ID(l, 2);
	}

	ID upper_bound_chain(int link_len, int period)
	{
		if (Length() % link_len != 0 || Length() < link_len*period) 
		{
			string v(link_len, '0');
			v[0] = '1';
			return ID(v, Length() / link_len + 1);
		}
		Number l(SubNum(0, link_len));
		ID val(l, period);
		if (*this <= val) return val;
		++l;
		return ID(l, period);
	}

	// invalid id less or equal to this
	ID lower_bound_2()
	{
		if (Length() == 1) return ID();
		if (Length() % 2 != 0) {
			string v(Length() / 2, '9');
			return ID(v, 2);
		}
		Number l(SubNum(0, Length() / 2));
		Number r(SubNum(Length() / 2));
		if (l > r) --l;
		return ID(l, 2);
	}

	ID next_invalid_id_2()
	{
		Number l(SubNum(0, Length() / 2));
		++l;
		*this = ID(l, 2);
		return *this;
	}

	ID next_invalid_id_chain()
	{
		Number l(link);
		++l;
		*this = ID(l, period);
		return *this;
	}
};

#include <set>
class InvalidIDs
{
	ID sum;
public:
	void process_range(const string& a, const string& b)
	{
		cout << "Process Range: " << a << " - " << b << endl;
		ID A(a), B(b);
		set<Number> uniq_ids;
		for (int link_len = 1; link_len <= B.Length() / 2; ++link_len)
		{
			int period = max(2, (int)((float)A.Length() / (float)link_len + 0.5f));
			int periodB = B.Length() / link_len;
			
			for (; period <= periodB; ++period)
			{
				ID id = A.upper_bound_chain(link_len, period);
				for (; id <= B; id.next_invalid_id_chain())
				{
					uniq_ids.insert(Number(id.String()));
					//sum += id;
				}
			}
		}
		cout << "  Invalid IDS:";
		for (const auto& id : uniq_ids)
		{
			cout << "  " << id.String();
			sum += id;
		}
		cout << endl;
	}
	void print_sum()
	{
		cout << "\nInvalid numbers sum : " << sum.String() << endl;
	}
};

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		cout << "Puzzle file name: " << argv[1] << '\n';

		InvalidIDs invalid_ids;
		auto invalid_ids_processor = [obj=&invalid_ids](const string& a, const string& b) { obj->process_range(a, b); };
		if (read_pazzle(argv[1], invalid_ids_processor) == 0)
		{
			invalid_ids.print_sum();
		}		
	}
	else
	{
		cout << "Error: input puzzle file is not set.\n";
	}

	return -1;
}

static int read_pazzle(const string& filepath, function<void(const string& a, const string& b)> process_range) 
{
	ifstream inputPuzzle(filepath);

	if (inputPuzzle.is_open())
	{
		string line;
		getline(inputPuzzle, line);
		if (!line.empty())
		{
			int i = 0;
			string a, b;
			for (int j = i; j < line.length(); ++j)
			{
				if (line[j] == '-')
				{
					auto ss = line.substr(i, j - i);
					a = line.substr(i, j - i);
					i = j + 1;
				}
				else if (line[j] == ',' || j == line.length() - 1)
				{
					b = line.substr(i, j - i + (j == line.length() - 1 ? 1 : 0));
					i = j + 1;

					process_range(a, b);
				}
			}
		}
		inputPuzzle.close();

		return 0;
	}
	else
	{
		cout << "Error: Could not open input puzzle file.\n";
	}

	return -1;
}