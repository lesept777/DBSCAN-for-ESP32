/*
 * This sketch shows a basic example of DBSCAN classification
 * It creates a dataset made of 100 points with x,y coordinates, grouped in 3 clusters around 3
 * centroid points, plus a few points located far away (labelled as noise)
 * The sketch compares the results of 4 different distance types
 */


#include "DBSCAN.h"

void setup() {
  Serial.begin(115200);
  std::vector<std::vector<float>> V;
  float x;
  int N = 100;
  uint16_t n0 = 0;
  uint16_t n1 = 0;
  uint16_t n2 = 0;
  uint16_t n3 = 0;
  for (int i = 0; i < 200; i++) {
    std::vector<float> v;
    int r = random(110);
    if (r > 100) { // noise
      ++n0;
      v.push_back(random(N + 1) * 10.0f / N); // random [0 - 10]
      v.push_back(random(N + 1) * 10.0f / N);
      v.push_back(random(N + 1) * 10.0f / N);
    } else if (r > 66) { // first cluster around 3,3,3
      ++n1;
      x = 2.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 2.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 2.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
    } else if (r > 33) { // first cluster around 5,5,5
      ++n2;
      x = 4.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 4.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 4.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
    } else { // second cluster around 7,7,7
      ++n3;
      x = 6.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 6.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
      x = 6.5 + random(N + 1) * 1.0f / N;
      v.push_back(x);
    }
    V.push_back(v);
  }
  Serial.print ("V size : "); Serial.println(V.size());
  Serial.printf ("Set 1 size : %d\n", n1);
  Serial.printf ("Set 2 size : %d\n", n2);
  Serial.printf ("Set 3 size : %d\n", n3);
  Serial.printf ("Noise size : %d\n", n0);

  // dbscan(epsilon, minPts, distance, mink)
  Serial.println("\nEUCLIDIAN");
  dbscan DB(.6, 4, EUCLIDIAN);
  DB.init(V);
//  Serial.println("\nMINKOVSKI");
//  dbscan DB2(.2, 4, MINKOVSKI, .5);
//  DB2.init(V);
  Serial.println("\nMANHATTAN");
  dbscan DB3(.75, 4, MANHATTAN);
  DB3.init(V);
  Serial.println("\nCHEBYCHEV");
  dbscan DB4(1.6, 4, CHEBYCHEV);
  DB4.init(V);
  Serial.println("\nCANBERRA");
  dbscan DB5(.1, 4, CANBERRA);
  DB5.init(V);
}

void loop() { }
