#include "DBSCAN.h"

/* Constructor
	Arguments : 
		epsilon : maximum distance between neighbours
		minPts  : minimum number of points in a cluster
		type    : the type of distance
		mink    : the exponent in case of the Minkovski distance (optionnal)
*/
dbscan::dbscan(float epsilon, int minPts, uint8_t distance, float mink)
{
	_epsilon  = epsilon;
	_minPts   = minPts;
	_distanceType = distance;
	_mink     = mink;
}

dbscan::~dbscan() {
}

/* Process the dataset */
std::vector<std::vector<uint16_t>> dbscan::init(std::vector<std::vector<float>> const &dataset)
{
	_nData = dataset.size();
	for (uint16_t i=0; i < _nData; ++i) {
		_dataset.push_back(dataset[i]);
		_type.push_back(NOT_VISITED);
	}

// Average distance
	float averageDistance = 0.0f;
	for (uint16_t i = 0; i < _nData; ++i)
		for (uint16_t j = i + 1; j < _nData; ++j)
			averageDistance += distance(_dataset[i], _dataset[j]);
	averageDistance /= ((_nData - 1) * _nData / 2);
	Serial.printf ("Average distance : %f\n", averageDistance);

// Process dataset
	std::vector<uint16_t> noise;
	for (uint16_t i = 0; i < _nData; ++i) {
		if (_type[i] == NOT_VISITED) {
			_type[i] = VISITED;
			std::vector<uint16_t> currentCluster;
			std::vector<uint16_t> neighbours = findNeighbours(i);
			// If the point has too few neighbours : set to noise
			if (neighbours.size() < _minPts) {
				_type[i] = NOISE;
				noise.push_back(i);
				++_nNoise;
				// Serial.println ("Noise!");
			} else {
				// create	 a cluster with this point
				currentCluster.push_back(i);
				enlargeCluster (neighbours, currentCluster);
				// Mark all points in the cluster as VISITED
				for (uint16_t j = 0; j < currentCluster.size(); ++j)
					_type[currentCluster[j]] = VISITED;
				// Add current cluster to clusters list
				_clusters.push_back(currentCluster);
				++_nClusters;
			}
		}
	}
	// Noise cluster is inserted at position 0 (first cluster) even if empty
	 _clusters.insert(_clusters.begin(), noise);
   displayStats ();
   return _clusters;
}

void dbscan::displayStats ()
{
	// Print statistics about the clusters
	uint16_t nFeatures = _dataset[_clusters[0][0]].size();
   std::vector<std::vector<float>> centroid;
   std::vector<float> tightness;
	Serial.printf ("Created %d clusters.\n", _nClusters);
	for (uint16_t i = 0; i < _nClusters; ++i) {
		Serial.printf ("Cluster %d : %d points\n",i, _clusters[i + 1].size() - 1);

// Centroid
      std::vector<float> c(nFeatures, 0);
      c = computeCentroid(nFeatures, _clusters[i + 1]);
		Serial.print("\tCentroid: ");
		for (uint16_t k = 0; k < nFeatures; ++k)
			Serial.printf("%f ",c[k]);
		Serial.println();
      centroid.push_back(c);

// Tightness (mean distance to centroid)
      float t = computeTightness(nFeatures, _clusters[i + 1], c);
		Serial.printf ("\tTightness = %.3f\n", t);
      tightness.push_back(t);
	}

// Separation of clusters (mean distance of centroids)
   float separation = 0.0f;
   float indexDB = 0.0;
   for (uint16_t i = 0; i < _nClusters; ++i) {
      for (uint16_t j = i+1; j < _nClusters; ++j) {
         separation += distance(centroid[i],centroid[j]);
         float index = (tightness[i] + tightness[j]) / separation;
         indexDB = max(indexDB, index);
      }
   }
   separation = separation * 2.0f / _nClusters / (_nClusters - 1.0);
   Serial.printf ("\nSeparation = %.3f", separation);

// Davies-Bouldin index (max ratio tightness over separation)
   Serial.printf ("\nDavies-Bouldin index = %.3f\n", indexDB);

	Serial.printf("%d noise points\n", _nNoise);
}

/* Compute the coordinates of the centroid of a cluster */
std::vector<float> dbscan::computeCentroid (uint16_t nFeatures, std::vector<uint16_t> const &cluster) 
{
	std::vector<float> centroid(nFeatures, 0);
	for (uint16_t j = 0; j < cluster.size(); ++j) {
		for (uint16_t k = 0; k < nFeatures; ++k) {
			centroid[k] += _dataset[cluster[j]][k] / cluster.size();
		}
	}
	return centroid;
}

/* Compute the tightness of a cluster */
float dbscan::computeTightness (uint16_t nFeatures, std::vector<uint16_t> const &cluster, std::vector<float> const &centroid) 
{
	float tightness = 0.0f;
	for (uint16_t j = 0; j < cluster.size(); ++j)
		tightness += distance(_dataset[cluster[j]], centroid) / cluster.size();
   return tightness;
}

/* Enlarge an existing cluster */
void dbscan::enlargeCluster (std::vector<uint16_t> neighbours, std::vector<uint16_t> &currentCluster) 
{
	uint16_t i = 0;
	while (i < neighbours.size()) {
		uint16_t index = neighbours[i++];
		if (_type[index] == NOT_VISITED) {
			std::vector<uint16_t> neighbours2 = findNeighbours(index);
			if (neighbours2.size() > _minPts) {
				// make union of both neighbourhoods
				for (uint16_t j = 0; j < neighbours2.size(); ++j) {
					bool isInNeighbours = false;
					for (uint16_t k = 0; k < neighbours.size(); ++k) {
						if (neighbours2[j] == neighbours[k]) {
							isInNeighbours = true;
							break;
						}
					}
					if (!isInNeighbours) neighbours.push_back(neighbours2[j]);
				}
			}
		}
		// add current point to current cluster is not already part of a cluster
		bool isInCluster = false;
		for (uint16_t j = 1; j < _nClusters; ++j)
			for (uint16_t k = 0; k < _clusters[j].size(); ++k)
				if (_clusters[j][k] == index) {
					isInCluster = true;
					break;
				}
		if (!isInCluster) currentCluster.push_back(index);
	}
}

/* Find the neighbours of a point in the dataset */
std::vector<uint16_t> dbscan::findNeighbours (uint16_t n)
{
	std::vector<uint16_t> neighbours;
	for (uint16_t i=0; i < _nData; ++i)
		if (isNeighbour(_dataset[n], _dataset[i])) neighbours.push_back(i);
	return neighbours;
}

/*
	Compute the distance between 2 vectors 
*/
float dbscan::distance(std::vector<float> const &vector1, std::vector<float> const &vector2)
{
	if (vector1.size() != vector2.size()) {
		Serial.printf("Vector size problem ! (%d != %d)\n",vector1.size(), vector2.size());
		return 1.0e10;
	}
	float distance = 0.0f;
	switch (_distanceType) {
		case EUCLIDIAN:
			for (uint8_t i=0; i<vector1.size(); ++i) distance += pow(vector1[i] - vector2[i], 2);
			distance = sqrt(distance);
			break;
		case MINKOVSKI:
			for (uint8_t i=0; i<vector1.size(); ++i)
				distance += pow(abs(vector1[i] - vector2[i]), _mink);
			distance = pow(distance, 1./_mink);
			break;
		case MANHATTAN:
			for (uint8_t i=0; i<vector1.size(); ++i) distance += abs(vector1[i] - vector2[i]);
			break;
		case CHEBYCHEV:
			for (uint8_t i=0; i<vector1.size(); ++i) distance += max(distance, abs(vector1[i] - vector2[i]));
			break;
		case CANBERRA:
			for (uint8_t i=0; i<vector1.size(); ++i) 
				distance += abs(vector1[i] - vector2[i]) / (abs(vector1[i]) + abs(vector2[i]));
			break;
		default:
			Serial.println ("Distance type problem !");
			distance = 2.0e10;
	}
	return distance;
}

int dbscan::countNeighbours(std::vector<float> const &vector)
{
	int neighbours = 0.0;
	for (uint8_t i=0; i < _nData; ++i)
		if (isNeighbour(vector, _dataset[i])) ++neighbours;
	return neighbours;
}

bool dbscan::isNeighbour(std::vector<float> const &vector1, std::vector<float> const &vector2)
{
	return (distance(vector1, vector2) <= _epsilon);
}

uint16_t dbscan::predict (std::vector<float> const &vector)
{
	uint16_t number = 65535;
	for (uint16_t i = 0; i < _nData; ++i)
		if (distance(vector, _dataset[i]) < _epsilon) {
			for (uint16_t j = 0; j < _nClusters; ++j)
				for (uint16_t k = 0; k < _clusters[j + 1].size(); ++k)
					if (_clusters[j + 1][k] == i) {
						return j;
					}
		}
	return number;
}