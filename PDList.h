
#ifndef COSC_ASS_ONE_POSITION_LIST
#define COSC_ASS_ONE_POSITION_LIST

#include "PositionDistance.h"
#include "Types.h"
#define ARRAY_SIZE 100

class PDList {
public:

   /*                                           */
   /* DO NOT MOFIFY ANY CODE IN THIS SECTION    */
   /*                                           */


   // Create a New Empty List
   PDList();

   // Clean-up the list
   ~PDList();

   // Number of items in the list
   int size();

   // Get a pointer to the position-distance at index i
   PDPtr get(int i);

   // Add a position-distance (as a pointer) to the list
   //    This class now has control over the pointer
   //    And should delete the pointer if the position-distance is removed from the list
   void addBack(PDPtr position);

   // Checks if the list contains a position-distance with the same co-ordinate
   //    as the given position.
   bool containsCoordinate(PDPtr position);

   // Remove everything from the list
   // Don't forget to clean-up the memory!
   void clear();

   /*                                           */
   /* YOU MAY ADD YOUR MODIFICATIONS HERE       */
   /*                                           */

   // Copy constructor
   PDList(PDList& other);

   /* This is a suggestion of what you could use. */
   /* You can change this code.                   */
private:
   // An array that contains the list of PositionDistance
   PDPtr positions[ARRAY_SIZE];

   // integer value that counts the number of position in the list
   int numPositions;
};

#endif // COSC_ASS_ONE_POSITION_LIST
