#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>

using namespace std;

pair<int, int> t;

int maxRepeatingChar(const string& s)
{
	int res = 0;
	size_t l = s.length();
	for (size_t i = 0; i < l;)
	{
		int consec = 1;
		for (size_t j = i + 1; j < l && s[i] == s[j]; ++j, ++consec) {}
		i += consec;
		res = max(res, consec);
	}
	return res;
}

int levenshteinDist(const string& w1, const string& w2)
{
	int dist;
	size_t l1 = w1.length();
	size_t l2 = w2.length();

	if (!l1)
	{
		dist = l2;
	}
	else if (!l2)
	{
		dist = l1;
	}
	else
	{
		vector<vector<int>> matrix(l2 + 1, vector<int>(l1 + 1));
		for (size_t c = 0; c <= l1; c++)
		{
			matrix[0][c] = c;
		}
		for (size_t l = 0; l <= l2; l++)
		{
			matrix[l][0] = l;
		}
		for (size_t c = 1; c <= l1; c++)
		{
			for (size_t l = 1; l <= l2; l++)
			{
				int cost = w1[c - 1] == w2[l - 1] ? 0 : 1;
				int top = matrix[l - 1][c] + 1;
				int left = matrix[l][c - 1] + 1;
				int diag = matrix[l - 1][c - 1] + cost;
				matrix[l][c] = min({ top, left, diag });
			}
		}
		dist = matrix[l2][l1];
	}
	return dist;
}

double normLevenshteinDist(const string& w1, const string& w2)
{
	return double(levenshteinDist(w1, w2)) / double(max(w1.length(), w2.length()));
}

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