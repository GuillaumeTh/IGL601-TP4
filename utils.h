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

bool epsilon(float f, float eps = 1e-7)
{
	return abs(f) < eps;
}

bool equals(const vector<float>& v1, const vector<float>& v2)
{
	if (v1.size() != v2.size()) return false;

	return equal(v1.begin(), v1.end(), v2.begin(), [](float f1, float f2) { return epsilon(f1 - f2); });
}

vector<float>& add(vector<float>& v1, const vector<float>& v2)
{
	transform(v1.begin(), v1.end(), v2.begin(), v1.begin(), plus<float>());
	return v1;
}

vector<float>& mult(vector<float>& v, float t)
{
	transform(v.begin(), v.end(), v.begin(), [&](float vi) { return vi * t; });
	return v;
}

vector<float> mult(const vector<float>& v1, const vector<float>& v2)
{
	vector<float> out(v1.size());
	transform(v1.begin(), v1.end(), v2.begin(), out.begin(), [](float vi, float vj) {
		return vi * vj;
	});
	return out;
}

float sum(const vector<float>& v)
{
	return accumulate(v.begin(), v.end(), 0.0);
}

float length(const vector<float>& v)
{
	return sqrtf(sum(mult(v, v)));
}

vector<float> average(const vector<int>& docIds, const vector<vector<float>>& freq)
{
	vector<float> init(freq[0].size(), 0.0f);
	vector<float> avg = accumulate(docIds.begin(), docIds.end(), init, [&](auto& sum, int id) {
		return add(sum, freq[id]);
	});
	mult(avg, 1.0f / float(docIds.size()));

	return avg;
}

vector<float> variance(const vector<int>& docIds, const vector<vector<float>>& freq, const vector<float>& averageCluster)
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