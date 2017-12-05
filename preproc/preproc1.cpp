#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

int main()
{
	ofstream out("..\\Data\\docwords_full.txt");
	for (int i = 1; i <= 3; ++i)
	{
		string path("..\\Data\\nsfabs_part" + to_string(i) + "_out\\docwords.txt");
		ifstream in(path);
		string line;
		while (getline(in, line))
		{
			out << line << "\n";
		}
	}
}