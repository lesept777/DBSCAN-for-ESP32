/*
   Classification of 2D data around 3 shapes
   (2 portions of circles and a center point)
   The example shows the impact of the number of data
   and the dispersion of the point around the shapes
*/

#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#define TFT_WIDTH 135
#define TFT_HEIGHT 240
#define BUTTON_1 35
#define BUTTON_2 0

#include "DBSCAN.h"

int nData = 200;
const int nFeatures = 2; // X and Y coordinates
uint8_t nClasses = 3;    // the 3 shapes
float tol = 0.01;
uint16_t n0, n1, n2, n3;
std::vector<std::vector<float>> Dataset;
float epsilon = 0.1f;

TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
int rad = TFT_WIDTH / 2;
int xc = rad;
int yc = TFT_HEIGHT / 2 - 2;
int my = (yc - 2) / 2;
float radius[3] = {0.8f, 0.4f, 0.2f};
bool calc = true;

std::vector<float> pickData(int c) {
  float r, t;
  int classe;
  t = (random(101) / 50.0f - 1.0f); // between -1 & 1
  t *= PI / 4.0f; // between -PI/4 & PI/4
  if (c > 100) { // noise : random coordinates (can be inside a cluster)
    t = (random(100) / 100.0f) * 2.0f * PI;
    r = random(100) / 100.0f;
    ++n0;
    classe = 0;
  } else if (c > 66) { // First cluster
    t += PI;
    r = radius[0];
    ++n1;
    classe = 1;
  } else if (c > 33) { // Second cluster
    t *= 2.0f;
    r = radius[1];
    ++n2;
    classe = 2;
  } else { // Third cluster
    t = (random(100) / 100.0f) * 2.0f * PI;
    r = random(100) / 100.0f * radius[2];
    ++n3;
    classe = 3;
  }
  // Add some noise around the shape
  float dx = (random(101) / 50.0f - 1.0f) * tol;
  float dy = (random(101) / 50.0f - 1.0f) * tol;
  float x = r * cos(t) + dx;
  float y = r * sin(t) + dy;
  std::vector<float> point;
  point.push_back(x);
  point.push_back(y);
  point.push_back(classe);
  return point;
}

void prepareDataset (float tol) {
  n0 = 0;
  n1 = 0;
  n2 = 0;
  n3 = 0;
  for (int i = 0; i < nData; i++) {
    int c = random(110);
    std::vector<float> point = pickData(c);
    Dataset.push_back(point);
  }
  Serial.printf("Cluster 1: %d points\n", n1);
  Serial.printf("Cluster 2: %d points\n", n2);
  Serial.printf("Cluster 3: %d points\n", n3);
  Serial.printf("%d noise\n", n0);
}

int mymap (float x, float xmin, float xmax, int ymin, int ymax) {
  float delta = (x - xmin) / (xmax - xmin);
  float y = ymin + delta * (ymax - ymin);
  return (int)y;
}

void initTFT(int nbClass) {
  // Prepare the display
  char text[30];
  tft.fillScreen (TFT_BLACK);
  sprintf (text, "%d data", nData);
  tft.drawString(text, xc, 0, 4);
  sprintf (text, "%d clusters", nbClass - 1);
  tft.drawString(text, xc, 25, 4);
  sprintf (text, "Tol: %.2f", tol);
  tft.drawString(text, xc, TFT_HEIGHT - 50, 4);
  sprintf (text, "Eps: %.2f", epsilon);
  tft.drawString(text, xc, TFT_HEIGHT - 25, 4);
  // Draw the external circle
  tft.drawCircle (xc, yc, rad, TFT_BLUE);
  // Draw the clusters
  int nPoints = 30;
  float x, y, xp, yp, t, dt, r;
  dt = PI / 2.0f / nPoints;
  // cluster 1
  t = 3.0f * PI / 4.0f;
  r = radius[0];
  xp = r * cos(t);
  yp = r * sin(t);
  int xxp = mymap(xp, -1.0, 1.0, 0, TFT_WIDTH);
  int yyp = mymap(yp, -1.0, 1.0, 0, TFT_HEIGHT / 2) + my;
  for (int i = 0; i < nPoints; ++i) {
    t += dt;
    x = r * cos(t);
    y = r * sin(t);
    int xx = mymap(x, -1.0, 1.0, 0, TFT_WIDTH);
    int yy = mymap(y, -1.0, 1.0, 0, TFT_HEIGHT / 2) + my;
    tft.drawLine (xxp, yyp, xx, yy, TFT_BLUE);
    xxp = xx;
    yyp = yy;
  }
  // cluster 2
  t = -PI / 2.0f;
  r = radius[1];
  xp = r * cos(t);
  yp = r * sin(t);
  xxp = mymap(xp, -1.0, 1.0, 0, TFT_WIDTH);
  yyp = mymap(yp, -1.0, 1.0, 0, TFT_HEIGHT / 2) + my;
  for (int i = 0; i < nPoints; ++i) {
    t += dt * 2.0f;
    x = r * cos(t);
    y = r * sin(t);
    int xx = mymap(x, -1.0, 1.0, 0, TFT_WIDTH);
    int yy = mymap(y, -1.0, 1.0, 0, TFT_HEIGHT / 2) + my;
    tft.drawLine (xxp, yyp, xx, yy, TFT_BLUE);
    xxp = xx;
    yyp = yy;
  }
  // cluster 3
  r = radius[2];
  int rr = rad * r;
  tft.drawCircle (xc, yc, rr, TFT_BLUE);
}

void displayDot(std::vector<float> point, uint32_t color) {
  // Draw a colored point at given coordinates
  float x = point[0];
  float y = point[1];
  int xx = mymap(x, -1.0, 1.0, 0, TFT_WIDTH);
  int yy = mymap(y, -1.0, 1.0, 0, TFT_HEIGHT / 2) + my;
  tft.drawPixel(xx, yy, color);
  tft.drawPixel(xx - 1, yy, color);
  tft.drawPixel(xx, yy - 1, color);
  tft.drawPixel(xx - 1, yy - 1, color);
}

void setup() {
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  Serial.begin(115200);
  tft.begin();
  tft.fillScreen (TFT_BLACK);
  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.setTextDatum(TC_DATUM);
}

void loop() {
  // Button 1 : increase tolerance
  if (digitalRead(BUTTON_1) == LOW) {
    tol += 0.02f;
    if (tol > .15) tol = 0.01f;
    calc = true;
  }

  // Button 2 : increase epsilon
  if (digitalRead(BUTTON_2) == LOW) {
    epsilon += 0.02f;
    if (epsilon > .25) epsilon = 0.06f;
    calc = true;
  }

  const int threshold = 90;
  // Touch GPIO 15 to change the size of the dataset
  const int touchPin15 = 15;
  int T15 = touchRead(touchPin15);
  if (T15 < threshold) {
    nData += 25;
    if (nData > 300) nData = 50;
    calc = true;
  }

  if (calc) {
    dbscan DB(epsilon, 5, EUCLIDIAN);
    prepareDataset (tol);
    std::vector<std::vector<uint16_t>> clusters;
    clusters = DB.init(Dataset);
    int nbClass = clusters.size();
    initTFT(nbClass);

    for (int classe = 0; classe < nbClass; ++classe) {
      std::vector<uint16_t> cluster = clusters[classe];
      for (uint16_t i : cluster) {
        std::vector<float> x = Dataset[i];
        //       int classe = (int)x[2];
        uint32_t color[6] = {TFT_WHITE, TFT_GREEN, TFT_RED, TFT_PURPLE, TFT_YELLOW, TFT_CYAN};
        if (classe < 6) displayDot(x, color[classe]);
        else displayDot(x, color[5]);
      }
    }
    Dataset.clear();
    calc = false;
    delay(200);
  }
}
