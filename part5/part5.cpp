#include "../utils.h"

int main()
{
	int nbDocs;
	int nbWords;
	int nbRawWords;
	ifstream counts("..\\Data\\counts.txt");
	counts >> nbRawWords >> nbWords >> nbDocs;
	counts.close();

	vector<vector<float>> freq(nbDocs, vector<float>(nbWords, 0.0f));
	loadWordFreq("..\\Data\\docwords_full2.txt", freq);

	const bool weighted = false;
	const int nbClusters = 256;
	const int nbIterations = 600;
	vector<vector<int>> clusterToDoc;
	vector<vector<float>> weights;
	vector<int> docIds(nbDocs);
	iota(docIds.begin(), docIds.end(), 0);
	auto t0 = high_resolution_clock::now();
	recursiveKMean(nbClusters, nbIterations, 1, ".", freq, docIds, clusterToDoc, weights, weighted);
	auto t1 = high_resolution_clock::now();

	stringstream clustersPath;
	clustersPath << "..\\Data\\clusters_tree_" << (weighted ? "w_" : "") << nbClusters << "_" << nbIterations;

	save(clustersPath.str() + ".txt", clusterToDoc);

	if (weighted)
	{
		save(clustersPath.str() + "_weights.txt", weights);
	}

	ofstream time(clustersPath.str() + "_time.txt");
	time << duration_cast<seconds>(t1 - t0).count();
}