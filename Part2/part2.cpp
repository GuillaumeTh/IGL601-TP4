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
	const int nbClusters = 200;
	const int nbIterations = 600;
	vector<vector<int>> clusterToDoc;
	vector<vector<float>> weights;
	vector<int> docIds(nbDocs);
	iota(docIds.begin(), docIds.end(), 0);
	kMean(nbClusters, nbIterations, freq, docIds, clusterToDoc, weights, weighted, true);

	stringstream clustersPath;
	clustersPath << "..\\Data\\clusters_" << (weighted ? "w_" : "") << nbClusters << "_" << nbIterations;

	save(clustersPath.str() + ".txt", clusterToDoc);

	if (weighted)
	{
		save(clustersPath.str() + "_weights.txt", weights);
	}
}