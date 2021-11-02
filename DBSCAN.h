/*
    DBSCAN unsupervised classification algorithm
    inspired by https://penseeartificielle.fr/clustering-avec-lalgorithme-dbscan/
    https://openclassrooms.com/fr/courses/4379436-explorez-vos-donnees-avec-des-algorithmes-non-supervises/4379571-partitionnez-vos-donnees-avec-dbscan

    (c) 2021 Lesept
    contact: lesept777@gmail.com

*/
#ifndef dbscan_h
#define dbscan_h

#include <Arduino.h>

enum DISTANCE {
  EUCLIDIAN,  // euclidian distance
  MINKOVSKI,  // Minkowski distance (is Euclidian if param = 2)
  MANHATTAN,  // Manhattan distance
  CHEBYCHEV,  // Chebychev distance
  CANBERRA    // Canberra distance
};

enum TYPE {
  NOT_VISITED,
  VISITED,
  NOISE
};

class dbscan
{
  private:
    float    _epsilon = 2.0f;
    float    _mink = 1.0f;
    uint16_t _minPts;
    uint16_t _nNoise = 0;
    uint8_t  _distanceType = 0;
    uint16_t _nData = 0;
    uint16_t _nClusters = 0;
    std::vector<uint8_t> _type;
    std::vector<std::vector<float>> _dataset;
    std::vector<std::vector<uint16_t>> _clusters;

    std::vector<float> computeCentroid (uint16_t, std::vector<uint16_t> const &);
    std::vector<uint16_t> findNeighbours (uint16_t);
    float computeTightness (uint16_t, std::vector<uint16_t> const &, std::vector<float> const &);
    float distance (std::vector<float> const &, std::vector<float> const &);
    int countNeighbours (std::vector<float> const &);
    bool isNeighbour (std::vector<float> const &, std::vector<float> const &);
    void enlargeCluster (std::vector<uint16_t>, std::vector<uint16_t> &);

  public:
    dbscan (float, int, uint8_t, float = 1.0f);
    ~dbscan ();
    void init (std::vector<std::vector<float>> const &);
    void displayStats ();
  };

#endif