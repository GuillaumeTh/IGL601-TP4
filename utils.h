#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <string>
#include <chrono>
#include <sstream>
#include <iterator>
#include <thread>
#include <algorithm>

using namespace std;
using namespace std::chrono;

using sstream = stringstream;

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

void loadWordFreq(const string& path, vector<vector<float>>& freq)
{
	int count = 0;
	int docId;
	int wordId;
	int wordCount;
	float wordFreq;
	ifstream docwords(path);
	auto t0 = high_resolution_clock::now();
	while (docwords >> docId)
	{
		if (!(count % 10000))
		{
			cout << "\rLoading documents... " << count << " / " << 9498679;
		}
		docwords >> wordId >> wordCount >> wordFreq;
		freq[docId - 1][wordId - 1] = wordFreq;
		count++;
	}
	auto t1 = high_resolution_clock::now();
	cout << "    " << duration_cast<seconds>(t1 - t0).count() << " sec\n";
}

inline bool epsilon(float f, float eps = 1e-7)
{
	return abs(f) < eps;
}

template <class T>
inline bool equals(const vector<T>& v1, const vector<T>& v2)
{
	if (v1.size() != v2.size()) return false;

	return equal(v1.begin(), v1.end(), v2.begin(), [](T f1, T f2) { return epsilon(f1 - f2); });
}

inline vector<float>& add(vector<float>& v1, const vector<float>& v2)
{
	transform(v1.begin(), v1.end(), v2.begin(), v1.begin(), plus<float>());
	return v1;
}

inline vector<float>& mult(vector<float>& v, float t)
{
	transform(v.begin(), v.end(), v.begin(), [&](float vi) { return vi * t; });
	return v;
}

inline vector<float> mult(const vector<float>& v1, const vector<float>& v2)
{
	vector<float> out(v1.size());
	transform(v1.begin(), v1.end(), v2.begin(), out.begin(), [](float vi, float vj) {
		return vi * vj;
	});
	return out;
}

inline float sum(const vector<float>& v)
{
	return accumulate(v.begin(), v.end(), 0.0);
}

inline float length(const vector<float>& v)
{
	return sqrtf(sum(mult(v, v)));
}

inline vector<float> average(const vector<int>& docIds, const vector<vector<float>>& freq)
{
	vector<float> init(freq[0].size(), 0.0f);
	vector<float> avg = accumulate(docIds.begin(), docIds.end(), init, [&](auto& sum, int id) {
		return add(sum, freq[id]);
	});
	mult(avg, 1.0f / float(docIds.size()));

	return avg;
}

inline vector<float> variance(const vector<int>& docIds, const vector<vector<float>>& freq, const vector<float>& averageCluster)
{
	const int nbWords = averageCluster.size();
	vector<float> var(nbWords);
#pragma omp parallel for
	for (int i = 0; i < nbWords; ++i)
	{
		float v = 0.0f;
		for (int d : docIds)
		{
			v += pow(averageCluster[i] - freq[d][i], 2);
		}
		var[i] = v;
	}
	mult(var, 1.0f / float(docIds.size()));
	return var;
}

int nearestClusterId(const vector<float>& doc,
	const vector<vector<float>>& averageCluster,
	const vector<vector<float>>& weights)
{
	const int nbClusters = averageCluster.size();
	const int nbWords = averageCluster[0].size();
	int minC = 0;
	double minLen = numeric_limits<double>::max();
	for (int c = 0; c < nbClusters; ++c)
	{
		const auto& avg = averageCluster[c];
		const auto& w = weights[c];
		double len2 = 0.0f;
		for (int j = 0; j < nbWords; ++j)
		{
			len2 += pow(avg[j] - doc[j], 2) * w[j];
		}
		if (len2 < minLen)
		{
			minLen = len2;
			minC = c;
		}
	}
	return minC;
}

void updateClusters(const int begin, const int end,
	const vector<int>& docIds,
	const vector<vector<float>>& freq,
	const vector<vector<float>>& averageCluster,
	vector<int>& docToCluster,
	const vector<vector<float>>& weights,
	bool display,
	int thread = 0)
{
	const int dispEnd = end - begin;
#pragma omp parallel for
	for (int i = begin; i < end; ++i)
	{
		const int dispBegin = i - begin;
		if (!(dispBegin % 5000) && display)
		{
			sstream ss;
			ss << "Thread " << thread << " : " << dispBegin << " / " << dispEnd << "\n";
			cout << ss.str();
		}
		docToCluster[i] = nearestClusterId(freq[docIds[i]], averageCluster, weights);
	}
	if (display)
	{
		sstream ss;
		ss << "Thread " << thread << " : DONE\n";
		cout << ss.str();
	}
}

void updateWeights(vector<vector<float>>& weights,
	const vector<vector<float>>& freq,
	const vector<vector<int>>& clusterToDoc,
	const vector<vector<float>>& averageCluster)
{
	const int nbClusters = clusterToDoc.size();
	const int nbWords = freq[0].size();
//#pragma omp parallel for
	for (int c = 0; c < nbClusters; ++c)
	{
		const auto& clusterDocIds = clusterToDoc[c];
		auto var = variance(clusterDocIds, freq, averageCluster[c]);

		auto& w = weights[c];
		for (int i = 0; i < nbWords; ++i)
		{
			w[i] /= (1.0f + var[i]);
		}
	}
}

template <class T>
void save(const string& path, const vector<vector<T>>& data)
{
	const int nb = data.size();
	ofstream out(path);
	for (int c = 0; c < nb; ++c)
	{
		cout << "\rSaving... " << c << " / " << nb;
		out << c << " ";
		const auto& docs = data[c];
		for (T d : docs)
		{
			out << d << " ";
		}
		out << "\n";
	}
}

void kMean(const int nbClusters, const int nbIterations,
	const vector<vector<float>>& freq,
	const vector<int>& docIds,
	vector<vector<int>>& clusterToDoc,
	vector<vector<float>>& weights,
	const bool weight,
	const bool display,
	const int nbThreads = thread::hardware_concurrency())
{
	const int nbDocs = docIds.size();
	const int nbWords = freq[0].size();
	vector<int> docToCluster(nbDocs);
	clusterToDoc = vector<vector<int>>(nbClusters);
	weights = vector<vector<float>>(nbClusters, vector<float>(nbWords, 1.0f));
	const float weightNorm = 100.0f;
	mt19937 engine;
	uniform_int_distribution<int> distr(0, nbClusters - 1);
	for (int i = 0; i < nbDocs; ++i)
	{
		//if (!(i % 1000))
		//{
		//	cout << "\rRandom initialization... " << i << " / " << nbDocs;
		//}
		int clusterId = distr(engine);
		docToCluster[i] = clusterId;
		clusterToDoc[clusterId].push_back(docIds[i]);
	}
	//cout << "\n";

	vector<vector<float>> averageCluster(nbClusters, vector<float>(nbWords));
	for (int i = 0; i < nbIterations; ++i)
	{
		cout << "\nIteration : " << i + 1 << " / " << nbIterations << "\n";
		//Compute average vector for each cluster
		cout << "Computing averages... \n";
		auto t0 = high_resolution_clock::now();
#pragma omp parallel for
		for (int c = 0; c < nbClusters; ++c)
		{
			const auto& clusterDocIds = clusterToDoc[c];
			averageCluster[c] = average(clusterDocIds, freq);
		}
		auto t1 = high_resolution_clock::now();

		//Updating weights
		if (weight)
		{
			cout << "Updating weights...";
			updateWeights(weights, freq, clusterToDoc, averageCluster);
			auto t2 = high_resolution_clock::now();
			cout << "    " << duration_cast<seconds>(t2 - t1).count() << " sec\n";

			cout << "Normalizing weights...\n";
			for (int c = 0; c < nbClusters; ++c)
			{
				float len = weightNorm / length(weights[c]);
				mult(weights[c], len);
			}
		}
		auto t2 = high_resolution_clock::now();
		//Compute distance for each cluster and vector
		cout << "Computing distances...\n";
		if (nbThreads > 1)
		{
			int split = nbDocs / nbThreads;
			vector<thread> th;
			for (int t = 0; t < nbThreads; ++t)
			{
				int begin = t * split;
				int end = (t == nbThreads - 1) ? nbDocs : (t + 1) * split;
				th.push_back(thread([&]() {
					updateClusters(begin, end, docIds, freq, averageCluster, docToCluster, weights, display, t);
				}));
			}
			for (auto& t : th)
			{
				t.join();
			}
		}
		else
		{
			updateClusters(0, nbDocs, docIds, freq, averageCluster, docToCluster, weights, display);
		}

		auto t3 = high_resolution_clock::now();
		cout << "    " << duration_cast<seconds>(t3 - t2).count() << " sec\n";

		//Reassign new cluster to min dist
		cout << "\n";
		for (auto& cl : clusterToDoc)
		{
			cl.clear();
		}
		for (int i = 0; i < nbDocs; ++i)
		{
			if (!(i % 1000))
			{
				cout << "\rReassigning clusters... " << i << " / " << nbDocs;
			}
			clusterToDoc[docToCluster[i]].push_back(docIds[i]);
		}
		auto t4 = high_resolution_clock::now();
		cout << "    " << duration_cast<seconds>(t4 - t3).count() << " sec\n";
		sstream ss;
		ss << "Iteration time : " << duration_cast<milliseconds>(t4 - t0).count() << "\n";
		cout << ss.str();
	}
}

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
				recursiveKMean(nbClusters / 2, nbIterations, layer + 1, id + "0", freq, localClusterToDoc[0], nextC1, nextW1, weighted);
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