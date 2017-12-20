#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>

#include "../utils.h"

using namespace std;

pair<int, int> t;

int main()
{
	ifstream f("..\\Data\\words.txt");
	vector<pair<int, string>> rawWords;
	vector<pair<int, string>> words;

	int id;
	string s;
	while (f >> id)
	{
		f >> s;
		rawWords.push_back(make_pair(id, s));
	}
	f.close();

	vector<int> newIds(rawWords.size() + 1, 0);

	rawWords.erase(remove_if(rawWords.begin(), rawWords.end(), [](const auto& p) {
		const auto& sp = p.second;
		return sp.length() <= 3 || maxRepeatingChar(sp) >= 3;
	}), rawWords.end());

	shuffle(rawWords.begin(), rawWords.end(), mt19937());

	const double threshold = 0.425;
	int i = 0;
	for (const auto& rw : rawWords)
	{
		auto pos = find_if(words.begin(), words.end(), [&](const auto& w) {
			return normLevenshteinDist(w.second, rw.second) < threshold;
		});
		if (pos == words.end())
		{
			words.push_back(rw);
			newIds[rw.first] = words.size();
		}
		else
		{
			newIds[rw.first] = newIds[pos->first];
		}

		if (!(i % 500))
		{
			cout << "\r" << i << " / " << rawWords.size() << " , " << words.size();
		}
		i++;
	}
	cout << "\nTotal words: " << words.size() << "\n";

	ofstream wordsOut("..\\Data\\words2.txt");
	for (const auto& p : words)
	{
		wordsOut << newIds[p.first] << " " << p.second << "\n";
	}

	ofstream newIdOut("..\\Data\\newIds.txt");
	for (size_t i = 1; i < newIds.size(); ++i)
	{
		newIdOut << i << " " << newIds[i] << "\n";
	}

	ofstream counts("..\\Data\\counts.txt");
	counts << newIds.size() << " " << words.size();
}