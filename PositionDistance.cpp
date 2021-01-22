#include "PositionDistance.h"

PositionDistance::PositionDistance(int x, int y) {

   // Setting values of x-coordinate and y-coordinate
   this->x = x;
   this->y = y;

   // Distance is set to 0
   this->distance = 0;
}

PositionDistance::PositionDistance(PositionDistance& other) {

   //Shallow copy
   this->x = other.x;
   this->y = other.y;
   this->distance = other.distance;
}

int PositionDistance::getX() {
   return this->x;
}

int PositionDistance::getY() {
   return this->y;
}

int PositionDistance::getDistance() {
   return this->distance;
}

void PositionDistance::setDistance(int dist) {
   this->distance = dist;
}