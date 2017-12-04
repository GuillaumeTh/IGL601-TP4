#include "..\utils.h"

void recursiveKMean(const int nbClusters, const int nbIterations, const int layer,
	const string& id,
	const vector<vector<float>>& freq,
	const vector<int>& docIds,
	vector<vector<int>>& clusterToDoc,
	vector<vector<float>>& weights,
	const bool weighted)
{

	auto t0 = high_resolution_clock::now();
	vector<vector<int>> localClusterToDoc;
	vector<vector<float>> localWeights;
	kMean(2, nbIterations, freq, docIds, localClusterToDoc, localWeights, weighted, false, 0);
	auto t1 = high_resolution_clock::now();
	
	sstream ss;
	ss << layer << " " << id << "    " << docIds.size();
	ss << " time : " << duration_cast<seconds>(t1 - t0).count() << " sec\n";
	cout << ss.str();

	if (nbClusters > 2)
	{
		vector<vector<int>> nextC1;
		vector<vector<float>> nextW1;
		vector<vector<int>> nextC2;
		vector<vector<float>> nextW2;
		if (layer < 4)
		{
			thread th1([&]() {
				recursiveKMean(nbClusters / 2, nbIterations, layer + 1, id + "0",freq, localClusterToDoc[0], nextC1, nextW1, weighted);
			});
			thread th2([&]() {
				recursiveKMean(nbClusters / 2, nbIterations, layer + 1, id + "1", freq, localClusterToDoc[1], nextC2, nextW2, weighted);
			});
			th1.join();
			th2.join();
		}
		else
		{
			recursiveKMean(nbClusters / 2, nbIterations, layer + 1, id + "0", freq, localClusterToDoc[0], nextC1, nextW1, weighted);
			recursiveKMean(nbClusters / 2, nbIterations, layer + 1, id + "1", freq, localClusterToDoc[1], nextC2, nextW2, weighted);
		}

		nextC1.insert(nextC1.end(), nextC2.begin(), nextC2.end());
		nextW1.insert(nextW1.end(), nextW2.begin(), nextW2.end());

		clusterToDoc = nextC1;
		weights = nextW1;
	}
	else
	{
		clusterToDoc = localClusterToDoc;
		weights = localWeights;
	}
}

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