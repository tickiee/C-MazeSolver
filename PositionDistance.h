
#ifndef COSC_ASS_ONE_POSITION
#define COSC_ASS_ONE_POSITION

#include "Types.h"

class PositionDistance {
public:

   /*                                           */
   /* DO NOT MOFIFY ANY CODE IN THIS SECTION    */
   /*                                           */

   // x-co-ordinate
   int getX();

   // y-co-ordinate
   int getY();

   // Distance
   int getDistance();


   /*                                           */
   /* YOU MAY ADD YOUR MODIFICATIONS HERE       */
   /*                                           */

   // Constructor that takes in x-coordinate and y-coordinate
   PositionDistance(int x, int y);

   // Copy constructor
   PositionDistance(PositionDistance& other);

   // Setter method, to set distance
   void setDistance(int dist);

private:

   // integer variable to store x-coordinate of position
   int x;

   // integer variable to store y-coordinate of position
   int y;

   // integer variable to store distance of position
   int distance;
};

/*                                           */
/* DO NOT MOFIFY THIS TYPEDEF                */
/*                                           */
// Pointer to a Position-Distance
typedef PositionDistance* PDPtr;

#endif // COSC_ASS_ONE_POSITION
