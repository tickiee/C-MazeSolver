#ifndef COSC_ASS_ONE_PATH_PLANNING
#define COSC_ASS_ONE_PATH_PLANNING

#define LRUD 4

#include "PositionDistance.h"
#include "PDList.h"
#include "Types.h"

class PathPlanning {
public:

   /*                                           */
   /* DO NOT MOFIFY ANY CODE IN THIS SECTION    */
   /*                                           */


   // Initialise a with a given maze of size (x,y)
   PathPlanning(Grid maze, int rows, int cols);

   // Clean-up
   ~PathPlanning();

   // The initial position
   void initialPosition(int x, int y);

   // Method for Milestone 2
   // Return a DEEP COPY of the PDList of all position-distance's
   //    that the robot can reach
   PDList* getReachablePositions();


   // Method for Milestone 3
   // Get the path from the starting position to the given co-ordinate
   //    The path should be a DEEP COPY
   PDList* getPath(int toX, int toY);


   /*                                           */
   /* YOU MAY ADD YOUR MODIFICATIONS HERE       */
   /*                                           */

private:

   // The maze copy
   Grid maze;

   // Number of rows in the maze
   int rows;

   // Number of columns in the maze
   int cols;

   // The robot's initial position
   PDPtr robotInitialPosition;

   // A list that contains all '.' in the maze.
   // Basically, this list contains all reachable positions, including the initial position
   // This list is used in both milestone 2 and milestone 3
   PDList* dotList;

   // A list that contains all '.' in the maze except where the robot is at
   // Basically, this list contains all reachable positions except the initial position
   // It should only have the positions where the robot can go
   // This list is used in milestone 2
   PDList* reachableList;

   // This list that should only contain positions that are the best path scenario the robot should take
   // This list is used in milestone 3, and is also the list that will be returned in the functional prototype
   // This list is known as list A as created in algorithm
   PDList* bestPathList;
};

#endif // COSC_ASS_ONE_PATH_PLANNING
