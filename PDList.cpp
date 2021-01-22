#include "PDList.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>


PDList::PDList() {

   // Initalising the list size to 0 when it is created
   numPositions = 0;
}

PDList::~PDList() {
   clear();
}

PDList::PDList(PDList& other) {

   // Shallow copy
   numPositions = other.numPositions;

   // Deep copy
   for(int i = 0; i < other.numPositions; ++i) {
      positions[i] = new PositionDistance(*other.positions[i]);
   }  
}

int PDList::size() {
   return this->numPositions;
}

PDPtr PDList::get(int i) {

   // Getting a position eg. (8,2,0) from the array
   PDPtr getPtr = positions[i];
   return getPtr;
}

void PDList::addBack(PDPtr position) {

   // If list is full, position cannot be added, error message will be displayed and it will be deleted 
   // Else, add position to the list
   if(numPositions != ARRAY_SIZE) {  
      positions[numPositions] = position;
      numPositions++;
   } 
   else {
      std::cout << "Error! Array is currently full!" << std::endl;
      delete position;
      position = nullptr;
   }
}

bool PDList::containsCoordinate(PDPtr position) {

   // This variable is created to stop the for loop once the condition of the if-statement is met and executed
   bool boolChecker = false;

   // Check if there the position passed into the parameter exists in the list
   // If the position's x-coordinate and y-coordinate is the same as one of the position in the list, return true
   for(int i = 0; i < numPositions && boolChecker == false; ++i) {
      if(positions[i]->getX() == position->getX() && 
         positions[i]->getY() == position->getY()) {
         
         boolChecker = true;
      }
   }

   return boolChecker;
}

void PDList::clear() {

   // Delete the values in positions[]
   for(int i = 0; i < numPositions; ++i) {
      delete[] positions[i];
      positions[i] = nullptr;
   }
}