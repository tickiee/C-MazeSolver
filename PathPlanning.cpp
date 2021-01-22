#include "PathPlanning.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

PathPlanning::PathPlanning(Grid originalMaze, int rows, int cols) {

   // Creating a copy of the maze, so that the maze that was passed in will not be corrupted
   maze = new char*[rows];
   for(int i = 0; i < rows; ++i) {
      maze[i] = new char[cols];
   }

   for(int i = 0; i < rows; ++i) {
      for(int j = 0; j < cols; ++j) {
         this->maze[i][j] = originalMaze[i][j];
      }
   }

   this->rows = rows;
   this->cols = cols;

   // Initialise robotInitialPosition that is a PDPtr to NULL
   robotInitialPosition = NULL;

   // Initialise dotList that is a PDList* to NULL
   dotList = NULL;

   // Initialise bestPathList that is a PDList* to NULL
   bestPathList = NULL;

   // Delete the maze that was passed in to stop memory leaks
   if(rows >= 0 && cols >= 0) {
      for (int i = 0; i < rows; ++i) {
         delete[] originalMaze[i];
         originalMaze[i] = nullptr;
      }
      delete[] originalMaze;
      originalMaze = nullptr;
   }
}

PathPlanning::~PathPlanning() {

   // Delete the copied maze to stop memory leaks
   if (rows >= 0 && cols >= 0) {
      for (int i = 0; i < rows; ++i) {
         delete[] maze[i];
         maze[i] = nullptr;
      }
      delete[] maze;
      maze = nullptr;
   }

   // Delete dotList
   delete dotList;
   dotList = nullptr;

   // Delete robotInitialPosition
   delete robotInitialPosition;
   robotInitialPosition = nullptr;

   // Delete reachableList
   delete reachableList;
   reachableList = nullptr;

   // Delete bestPathList
   delete bestPathList;
   bestPathList = nullptr;
}

void PathPlanning::initialPosition(int x, int y) {
   robotInitialPosition = new PositionDistance(x, y);
}

PDList* PathPlanning::getReachablePositions() {

   dotList = new PDList();  
   reachableList = new PDList();

   // Add robot's initial position into dotList
   dotList->addBack(new PositionDistance(*robotInitialPosition));
   
   // Creating a temporary list
   // The temporary list is checker used to make sure that all the acceptable positions the robot can go to is checked
   PDList* tempList = new PDList();

   // Creating 2 PDPtr
   // addPos PDPtr is used to hold a position that the robot can go from its position
   // tmp PDPtr is used to store a position that is next to addPos
   // For example, if the robot is currently at (6,3), the robot can go to (6,2) or (6,4) or (7,3) or (5,3)
   // By using a for loop, all positions are dealt with individually
   PDPtr addPos = NULL;
   PDPtr tmpPos = NULL;

   // Declaring 3 variables and initalising them to 0
   // posX and posY variables are used to store the individual values of the positions the robot can go to
   // dist variable is used to store the current position of the robot
   int posX = 0;
   int posY = 0;
   int dist = 0;

   // if tempList is smaller than dotList, then there is a position in list that has not been checked. Therefore, that position has to be checked.
   while((tempList->size()) != (dotList->size())) {
      
      // Assume dotList contains (8,2) while tempList is empty
      // The size of tempList will be 0
      // Therefore, dotList-> get(0) will get us (8,2)
      // And since it is a PDPtr, addPos has to be initialised as a PDPtr.
      // addPos is also known as the currently checking position
      addPos = dotList->get(tempList->size());  

      // Get the distance of the position it is checking at store it in the dist variable
      dist = addPos->getDistance();
      
      // moveHorizontal{ "moving left", "moving right", "moving up", "moving down" }
      // moveVertical{ "moving left", "moving right", "moving up", "moving down" }
      // These 2 arrays are used to check the positions that are next to the currently checking position.
      // The next position has a different x-coordinate, y-coordinate and distance value to the currently checking position.
      // Assume the position that is checked is (6,2), and the position to check is (6,1)
      // Since (6,1) is on the left of (6,2), the value of x-coordinate will change by adding the value of "moving left" and
      // the value of y-coordinate will also change by adding the value of "moving left"
      // dist variable will increase by 1
      // {Left, Right, Up, Down}
      int moveHorizontal[LRUD] = {-1, 1, 0, 0};
      int moveVertical[LRUD] = {0, 0, -1, 1};

      for(int i = 0; i < LRUD; ++i) {
         posX = addPos->getX() + moveHorizontal[i];
         posY = addPos->getY() + moveVertical[i];
   
         // Take a position that is left or right or up or down for checking in the if-statement below
         tmpPos = new PositionDistance(posX, posY);

         // Checks if the next position is a '.' on the maze. 
         // If it is, there is a position that the robot can go to
         // Increase the dist variable by 1 and add the position inside the dotList
         // However, if the next position is not a '.', the values in the position will be deleted
         if(!dotList->containsCoordinate(tmpPos) && maze[posY][posX] == '.') {
            tmpPos->setDistance(dist + 1);
            dotList->addBack(tmpPos);
         } 
         else {
            delete tmpPos;
         }
      }

      // Now that the position has been checked, add this position inside the tempList to show that this position has been checked
      // This is used for the condition of the while loop
      tempList->addBack(new PositionDistance(*addPos));
   }

   // This is used to copy the dotList into reachableList
   // addReachPos will be the variable used
   for(int i = 1; i < dotList->size(); ++i) {
      PDPtr addReachPos = dotList->get(i);
      reachableList->addBack(new PositionDistance(*addReachPos));
   }

   // Once dotList and reachableList has been populated with the right pointers, tempList is no longer of use
   // Therefore, tempList will be deleted
   delete tempList;
   tempList = nullptr;

   // This is used to help get outputs when creating unit tests
   // It displays the(x,y,distance)
   // for(int i = 0; i < reachableList->size(); ++i) {
   //    std::cout << "(" << reachableList->get(i)->getX() << "," << reachableList->get(i)->getY() << "," << reachableList->get(i)->getDistance() << ")" << std::endl; 
   // }

   return reachableList;
}

// THIS IS FOR MILESTONE 3 ONLY
//    ONLY IMPLEMENT THIS IF YOU ATTEMPT MILESTONE 3

/*
   Below is my steps to create the best path

   8, 2, 0 START
   8, 1, 1p
   8, 3, 1
   7, 1, 2 END
   6, 1, 3

   Start from end to robot

   while(distance is not 0) (not the robot position (8, 2, 0))

      1) Get the pointer to be checked from bestPathList.
      End coordinate = 7, 1

      2) Get the coordinates from this pointer.
      x = 7, y = 1

      3) Get Distance of the coordinate
      distance = 2

      4) Get all coordinates of (distance - 1) from dotList and put it in similiarDistanceList.
      Coordinates = 8, 1 & 8, 3

      5) Using a for loop, check if the individual positions in similarDistanceList is 1 x-coordinate/y-coordinate away from the current checking position.
      Only 8, 1 is accepted

      6) Put 8, 1 in the addingPointerToBPList

      7) Add whatever is in addingPointerToBPList to bestPathList

      While End (Loop if distance added in to bestPathList from Step 7 is 0)

      8) Delete similarDistanceList.
      9) Delete addingPointerToBPList.
      10) Display bestPathList. (return this list actually)

      Remember to delete bestPathList after everything is done.
*/
PDList* PathPlanning::getPath(int toX, int toY) {

   bestPathList = new PDList();

   // This variable is used to store the index of the position in the dotList in the for loop below
   int endCoorIndexInList = 0;

   // This variable is created to stop the for loop once the condition of the if-statement is met and executed
   bool endCoorExists = false;

   // This for loop is used to find the end position from the dotList
   // endCoorIndexInList is used to store the index of the position in the dotList
   for(int i = 0; i < dotList->size() && endCoorExists == false; ++i) {
      if((dotList->get(i))->getX() == toX &&
      (dotList->get(i))->getY() == toY) {
         endCoorIndexInList = i;
         endCoorExists = true;
      }
   }

   /* ---------------------------------- */

   // This variable hold the index of coordinate to be checked in the bestPathList
   // It will always increase by 1 until the currently checking position has a distance of 0
   int indexOfCoorToCheckInBPList = 0;

   // Declaring newCoorDistance variable and initialising it to 1
   // The reason why it is 1 and not 0 is because of the while loop below
   // When the value passed into the variable is 0, that means the position is now at where the robot is
   int newCoorDistance = 1;

   // Add end position to bestPathList
   bestPathList->addBack(new PositionDistance( *(dotList->get(endCoorIndexInList)) ) );

   while(newCoorDistance != 0) {

      // Creating 2 new lists
      // similarDistanceList contains positions that are from dotList
      // These positions must be checked if they are beside the currently checking position
      // If it is, the positions will be added to addingPointerToBPList
      // addingPointerToBPList contains a position that is from similarDistanceList
      // This position that will be added in bestPathList
      PDList* similarDistanceList = new PDList();
      PDList* addingPointerToBPList = new PDList();      

      // These 3 variables declared are the coordinates and distance of the currently checking position
      int x = bestPathList->get(indexOfCoorToCheckInBPList)->getX();
      int y = bestPathList->get(indexOfCoorToCheckInBPList)->getY();
      int distance = bestPathList->get(indexOfCoorToCheckInBPList)->getDistance();

      // The next distance to check is distance -1
      newCoorDistance = distance - 1;

      // Add all potential positions to similarDistanceList.
      for(int i = 0; i < dotList->size(); ++i) {
         if( (dotList->get(i))->getDistance() == newCoorDistance) {
            similarDistanceList->addBack(new PositionDistance( *(dotList->get(i)) ) );
         }
      }

      // Check the positions x-coordinate and y-coordinate in similarDistanceList, and only add the correct position to addingPointerToBPList.
      for(int i = 0; i < similarDistanceList->size(); ++i) {
         int checkX = similarDistanceList->get(i)->getX();
         int checkY = similarDistanceList->get(i)->getY();
         
         // {Left, Right, Up, Down} See milestone 2 functional prototype which is above for explanation
         int moveHorizontal[LRUD] = {-1, 1, 0, 0};
         int moveVertical[LRUD] = {0, 0, -1, 1};

         // If position is correct, add to addingPointerToBPList.
         for(int j = 0; j < LRUD; ++j) {
            if(x == checkX + moveHorizontal[j] && y == checkY + moveVertical[j]) {
               addingPointerToBPList->addBack(new PositionDistance( *(similarDistanceList->get(i)) ) );
            }
         }
      }

      // Add Position Distance from addingPointerToBPList to bestPathList.
      bestPathList->addBack(new PositionDistance( *(addingPointerToBPList->get(0)) ) );

      delete similarDistanceList;
      delete addingPointerToBPList;

      // Incrementing the coordinate to be checked in bestPathList by 1.
      indexOfCoorToCheckInBPList++;
   }

   // This is used to help get outputs when creating unit tests
   // It displays the(x,y,distance)
   // for(int i = bestPathList->size() - 1; i >= 0; i = i - 1) {
   //    std::cout << "(" << bestPathList->get(i)->getX() << "," << bestPathList->get(i)->getY() << "," << bestPathList->get(i)->getDistance() << ")" << std::endl; 
   // }

   return bestPathList;
}
