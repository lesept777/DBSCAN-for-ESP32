# DBSCAN classification for ESP32
This library is designed to be used with the Arduino IDE.

## Dependencies
* no dependency

## DBSCAN instance creation
First declare an instance of you DBSCAN:
```dbscan DB(.6, 4, EUCLIDIAN);```
The arguments are:
* epsilon
* minimum number of points in a neighbourhood
* type of distance chosen

## Dataset creation
A dataset is a vector of vectors of floats
```std::vector<std::vector<float>> dataset```
See the example on how to create a dataset. Basiccally, you need to declare the dataset as above and push all your vectors fo float data into the dataset. if `v` is a vector of data of dimension 3:
```std::vector<float> v;
float myV[] = {3, 3, 3};
v.assign (myV, myV + 3);
dataset.push_back(v);```
Or more simply, if you have the data values
```dataset.push_back([3,3,3]);```

when the dataset is ready, just call the `init` method:
```DB.init(dataset);```

## Data prediction
To predict in which cluster a data is best located, use the `predict`method. Create the vector, send it to `predict`. The method returns the number of the best cluster or 65535 if your data is noise.
```std::vector<float> v;
float myV[] = {3, 3, 3};
v.assign (myV, myV + 3);
uint16_t n = DB.predict(v);
Serial.printf("[3,3,3] in cluster %d\n", n);```
  