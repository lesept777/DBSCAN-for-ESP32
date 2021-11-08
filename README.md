# DBSCAN classification for ESP32
This library is designed to be used with the Arduino IDE.

## Dependencies
* no dependency

## DBSCAN instance creation
First declare an instance of your [DBSCAN](https://en.wikipedia.org/wiki/DBSCAN):
```
dbscan DB(.6, 4, EUCLIDIAN);
```
The arguments are:
* epsilon: size of a local neighbourhood
* minimum number of points in a neighbourhood
* type of distance chosen:
	*  EUCLIDIAN,  // [Euclidian](https://en.wikipedia.org/wiki/Euclidean_space#Distance_and_length) distance
	*  MINKOVSKI,  // [Minkowski](https://en.wikipedia.org/wiki/Minkowski_distance) distance (is Euclidian if param = 2)
	*  MANHATTAN,  // [Manhattan](https://en.wikipedia.org/wiki/Taxicab_geometry) distance
	*  CHEBYCHEV,  // [Chebychev](https://en.wikipedia.org/wiki/Chebyshev_distance) distance
	*  CANBERRA    // [Canberra](https://en.wikipedia.org/wiki/Canberra_distance) distance

## Dataset creation
A dataset is a vector of vectors of floats
```
std::vector<std::vector<float>> dataset
```
See the example on how to create a dataset. Basically, you need to declare the dataset as above and push all your vectors fo float data into the dataset. if `v` is a vector of data of dimension 3:
```
std::vector<float> v;
float myV[] = {3, 3, 3};
v.assign (myV, myV + 3);
dataset.push_back(v);
```
Or more simply, if you have the data values
```
dataset.push_back([3,3,3]);
```

when the dataset is ready, just call the `init` method:
```
DB.init(dataset);
```
`init` returns the vector of clusters. The first vector is always the noise (even if empty), so the number of clusters created is equal to `clusters.size() - 1`
```
    std::vector<std::vector<uint16_t>> clusters;
    clusters = DB.init(Dataset);
```
The method `displayStats` profvides usuful information about the clusters.

## Data prediction
To predict in which cluster a data is best located, use the `predict`method. Create the vector, send it to `predict`. The method returns the number of the best cluster or 65535 if your data is noise.
```
std::vector<float> v;
float myV[] = {3, 3, 3};
v.assign (myV, myV + 3);
uint16_t n = DB.predict(v);
Serial.printf("[3,3,3] in cluster %d\n", n);
```

## Examples
Two examples are proposed, a `simple`one to show the basics, and a more complex one with graphics that you can use on a `TTGO` T-display device. This example groups 2D points into 3 clusters (one in a circle in the center of the screen, and 2 around 2 circle arcs). You can use the buttons to change the values of epsilon and the tolerance of the points around the arcs, and GPIO15 to change the number of points in the dataset.
  