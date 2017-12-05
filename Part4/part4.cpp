#include "../utils.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

using sstream = stringstream;

template <class T, class U>
using umap = unordered_map<T, U>;

template <class T>
void loadClusterData(const string& path, vector<vector<T>>& out)
{
	ifstream clustersFile(path);
	string line;
	int clusterId;
	T in;
	while (getline(clustersFile, line))
	{
		sstream ss(line);
		ss >> clusterId;
		//cout << "\rLoading... " << clusterId;
		vector<T> data;

		while (ss >> in)
		{
			data.push_back(in);
		}
		out.push_back(data);
	}
	cout << "\n";
}

template <class T>
void load(const string& path, vector<T>& data)
{
	int id;
	T in;
	ifstream file(path);
	while (file >> id)
	{
		//cout << "\rLoading... " << id;
		file >> in;
		data.push_back(in);
	}
	cout << "\n";
}

void load2(const string& path, umap<int, int>& data)
{
	int id;
	int in;
	ifstream file(path);
	while (file >> id)
	{
		//cout << "\rLoading... " << id;
		file >> in;
		data[id - 1] = in;
	}
	cout << "\n";
}

int main()
{
	string clusterPath = "..\\Data\\clusters_tree_w_256_600";
	int nbDocs;
	int nbWords;
	int nbRawWords;
	ifstream counts("..\\Data\\counts.txt");
	counts >> nbRawWords >> nbWords >> nbDocs;
	counts.close();

	vector<vector<int>> clusters;
	loadClusterData(clusterPath + ".txt", clusters);
	const int nbClusters = clusters.size();

	vector<string> words;
	load("..\\Data\\words2.txt", words);

	umap<int, int> programIds;
	for (int i = 0; i < nbDocs; ++i)
	{
		programIds[i] = i;
	}
	load2("..\\Data\\docid_progid2.txt", programIds);

	vector<vector<float>> weights;
	loadClusterData(clusterPath + "_weights.txt", weights);

	ofstream purityOut(clusterPath + "_stats.txt");
	int minNb = numeric_limits<int>::max();
	int maxNb = 0;
	int minSize = minNb;
	int maxSize = 0;
	vector<float> ratios;
	for (int c = 0; c < nbClusters; ++c)
	{
		const auto& clusterDocIds = clusters[c];
		const int clusterSize = clusterDocIds.size();
		set<int> programs;
		for (int d : clusterDocIds)
		{
			programs.insert(programIds[d]);
		}
		const int nbPrograms = programs.size();
		const float ratio = float(nbPrograms) / float(clusterSize);
		purityOut << c << " " << clusterSize << " " << nbPrograms << " " << ratio << "\n";
		minNb = min(minNb, nbPrograms);
		maxNb = max(maxNb, nbPrograms);
		minSize = min(minSize, clusterSize);
		maxSize = max(maxSize, clusterSize);
		if (clusterSize > 0)
		{
			ratios.push_back(ratio);
		}
	}
	auto p = minmax_element(ratios.begin(), ratios.end());
	float minRatio = *(p.first);
	float maxRatio = *(p.second);
	float avgRatio = accumulate(ratios.begin(), ratios.end(), 0.f) / float(ratios.size());
	purityOut << "\n" << "MinSize : " << minSize << " MaxSize : " << maxSize << "\n";
	purityOut << "MinPrograms : " << minNb << " MaxPrograms : " << maxNb << "\n";
	purityOut << "MinRatio : " << minRatio << " MaxRatio : " << maxRatio << " AverageRatio : " << avgRatio << "\n";
	purityOut.close();

	bool hasWeights = weights.size();
	vector<vector<float>> var;
	if (!hasWeights)
	{
		vector<vector<float>> freq(nbDocs, vector<float>(nbWords, 0.0f));
		loadWordFreq("..\\Data\\docwords_full2.txt", freq);
		var = vector<vector<float>>(nbClusters, vector<float>(nbWords));
		for (int c = 0; c < nbClusters; ++c)
		{
			cout << "\rNo weights, computing variance... " << c << " / " << nbClusters;
			const auto& clusterDocIds = clusters[c];
			const auto avg = average(clusterDocIds, freq);
			auto temp = variance(clusterDocIds, freq, avg);
			var[c] = mult(temp, -1.0f);
		}
	}
	cout << "\n";

	string type = hasWeights ? "_weights" : "_var";
	ofstream wordsOut(clusterPath + type + "_words.txt");
	const auto& values = (hasWeights ? weights : var);
	const int nbWordsDisplay = 20;
	for (int c = 0; c < nbClusters; ++c)
	{
		//cout << "\rSaving words... " << c << " / " << nbClusters;
		wordsOut << c << " ";
		const auto& v = values[c];
		if (v.size() > 0)
		{
			vector<int> indices(nbWords);
			iota(indices.begin(), indices.end(), 0);
			sort(indices.begin(), indices.end(), [&](int a, int b) { return v[a] > v[b]; });
			for (int i = 0; i < min(nbWordsDisplay, int(nbWords)); ++i)
			{
				wordsOut << words[indices[i]] << " ";
			}
		}
		wordsOut << "\n";
	}
	wordsOut.close();
	cout << "\n";
}