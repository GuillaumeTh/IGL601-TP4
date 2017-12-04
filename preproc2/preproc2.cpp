#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <numeric>
#include <algorithm>

using namespace std;

void dumpToFile(map<int, int>& m, ofstream& file, int id)
{
	int maxFreq = accumulate(m.begin(), m.end(), 0, [](int total, const auto& p) {
		return max(total, p.second);
	});
	for (const auto& p : m)
	{
		float freq = 0.5f + 0.5f * float(p.second) / float(maxFreq);
		file << id << " " << p.first << " " << p.second << " " << freq <<"\n";
	}
	m.clear();
}

int main()
{
	int nbRawWords;
	ifstream icounts("..\\Data\\counts.txt");
	icounts >> nbRawWords;
	icounts.close();

	vector<int> oldToNew(nbRawWords);
	ifstream newIds("..\\Data\\newIds.txt");
	int oldId;
	int newId;
	while (newIds >> oldId)
	{
		newIds >> newId;
		oldToNew[oldId] = newId;
	}

	ifstream docwordsIn("..\\Data\\docwords_full.txt");
	ofstream docwordsOut("..\\Data\\docwords_full2.txt");
	int docId;
	int wordId;
	int nb;
	int prevDocId = 0;
	map<int, int> newCount;
	while (docwordsIn >> docId)
	{
		if (!(docId % 1000))
		{
			cout << "\r" << docId << " / " << 128000;
		}
		docwordsIn >> wordId >> nb;
		if (prevDocId != docId)
		{
			dumpToFile(newCount, docwordsOut, prevDocId);
			prevDocId = docId;
		}
		
		int newId = oldToNew[wordId];
		if (newId)
		{
			newCount[newId] += nb;
		}
	}
	dumpToFile(newCount, docwordsOut, prevDocId);

	ofstream ocounts("..\\Data\\counts.txt", ios::app);
	ocounts << " " << docId;
}