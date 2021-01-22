
#include "PathPlanning.h"
#include "Types.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <tuple>
#include <string>
#include <vector>

/*
 * Running a unit test requires three to five text-files:
 *    1. The maze
 *    2. The initial position
 *    3. The reachable positions of the robot (un-sorted)
 *    4. The goal position (optional)
 *    5. The path (optional)
 *
 * Full command
 *    ./unit_tests <testname>
 *
 * For example:
 *   ./unit_tests sampleTest/sample01
 *
 */

#define ARGV_TEST    1
#define EXT_MAZE     std::string(".maze")
#define EXT_INT      std::string(".initial")
#define EXT_POS      std::string(".pos")
#define EXT_GOAL     std::string(".goal")
#define EXT_PATH     std::string(".path")
#define REQ_ARGS     1
#define TUPLE_X      0
#define TUPLE_Y      1
#define TUPLE_DIST   2

#define DEBUG        1

// Useful typedef for positions
typedef std::tuple<int,int,int> MyPosition;

// Data contens
class Data {
public:
   Data() :
      maze(NULL),
      rows(0),
      cols(0),
      initial(),
      goal(),
      m3(false)
   {};

   Grid maze;
   int rows;
   int cols;
   MyPosition initial;
   MyPosition goal;
   std::vector<MyPosition> positions;
   std::vector<MyPosition> path;

   bool m3;
};
typedef Data* DataPtr;

// Helper methods for making and deleting grids
Grid make_grid(const int rows, const int cols);
void delete_grid(Grid grid, int rows, int cols);

// Functions
void check_args(int argc, char** argv);
void load_data(char** argv, DataPtr data);
void load_data_maze(char** argv, DataPtr data);
void load_data_initial(char** argv, DataPtr data);
void load_data_pos(char** argv, DataPtr data);
void load_data_goal(char** argv, DataPtr data);
void load_data_path(char** argv, DataPtr data);
void load_lines(std::ifstream& in, std::vector<std::string>& lines);
bool match_positions(MyPosition& posTest, PDPtr posrp);
bool run_unit_test(DataPtr data);

int main(int argc, char** argv) {

   try {
      // Check args
      check_args(argc, argv);

      // Load file contents into data structure
      DataPtr data(new Data());
      load_data(argv, data);

      // Run actual test
      if (DEBUG) {
         std::cout << "Running Unit Test" << std::endl;
      }
      bool testPassed = run_unit_test(data);
      if (testPassed) {
         std::cout << "Test Passed" << std::endl;
      } else {
         std::cout << "Test Failed" << std::endl;
      }
      // PathPlanning* rp = new PathPlanning(data->maze, data->rows, data->cols);
      // rp->initialPosition(8, 2);
      //rp->initialPosition(8, 2);
      // delete rp;
      delete data;
   } catch (std::runtime_error &exception) {
      std::cout << "ERROR: Caught Error: "
                << exception.what()
                << std::endl;
   }

   return 0;
}

void check_args(int argc, char** argv) {
   if (argc != REQ_ARGS + 1) {
      throw std::runtime_error("Not enough file names provided");
   }

   std::string mazeFilename = argv[ARGV_TEST] + EXT_MAZE;
   std::string initialFilename = argv[ARGV_TEST] + EXT_INT;
   std::string posFilename = argv[ARGV_TEST] + EXT_POS;

   if (DEBUG) {
      std::cout << "Maze filename: " << mazeFilename << std::endl;
      std::cout << "Initial filename: " << initialFilename << std::endl;
      std::cout << "Positions filename: " << posFilename << std::endl;
   }

   // Check files exist
   auto checkFile = [](std::string &filename) {
      std::ifstream in;
      in.open(filename);
      if (in.fail()) {
         throw std::runtime_error("Could not open file '" + filename + "'");
      }
      in.close();
   };

   checkFile(mazeFilename);
   checkFile(initialFilename);
   checkFile(posFilename);
}

void load_data(char** argv, DataPtr data) {
   load_data_maze(argv, data);
   load_data_initial(argv, data);
   load_data_pos(argv, data);

   std::string filename = argv[ARGV_TEST] + EXT_GOAL;
   std::ifstream in(filename);
   if (in.good()) {
      data->m3 = true;
      load_data_goal(argv, data);
      load_data_path(argv, data);
   } else {
      data->m3 = false;
   }
   in.close();
}

void load_data_maze(char** argv, DataPtr data) {
   std::string filename = argv[ARGV_TEST] + EXT_MAZE;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);
   if (lines.size() == 0) {
      throw std::runtime_error("No Maze in file");
   }

   // Create data
   int width = lines.front().size();
   int height = lines.size();
   data->maze = make_grid(height, width);

   // Load Maze
   for (unsigned int row = 0; row != lines.size(); ++row) {
      std::string& line = lines[row];

      // Check width
      if (line.size() != (unsigned int) width) {
         throw std::runtime_error("Maze dimensions not consistent");
      }

      // Load data
      for (unsigned int col = 0; col != line.size(); ++col) {
         data->maze[row][col] = (char) line[col];
      }
   }
   data->rows = height;
   data->cols = width;
   if (DEBUG) {
      std::cout << "Loaded Maze: " << std::endl;
      for (int row = 0; row != data->rows; ++row) {
         for (int col = 0; col != data->cols; ++col) {
            std::cout << data->maze[row][col];
         }
         std::cout << std::endl;
      }
   }
}

void load_data_initial(char** argv, DataPtr data) {
   std::string filename = argv[ARGV_TEST] + EXT_INT;
   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);

   // Load position
   std::string line = lines[0];
   std::regex regex("^[(]([0-9]+),([0-9]+)[)]$");
   std::smatch match;
   bool ok = std::regex_match(line, match, regex);

   if (ok) {
      MyPosition position;
      std::get<TUPLE_X>(position) = std::stoi(match[1].str());
      std::get<TUPLE_Y>(position) = std::stoi(match[2].str());

      data->initial = position;

      if (DEBUG) {
         std::cout << "Loaded Initial Position: "
                   << "("
                   << std::get<TUPLE_X>(data->initial)
                   << ","
                   << std::get<TUPLE_Y>(data->initial)
                   << ")"
                   << std::endl;
      }
   } else {
      throw std::runtime_error("Position format incorrect");
   }
}

void load_data_pos(char** argv, DataPtr data) {
   std::string filename = argv[ARGV_TEST] + EXT_POS;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);
   if (DEBUG) {
      std::cout << "Got: " << lines.size() << " positions" << std::endl;
   }

   // Process each position
   for (std::string& line : lines) {
      std::regex regex("^[(]([0-9]+),([0-9]+),([0-9]+)[)]$");
      std::smatch match;
      bool ok = std::regex_match(line, match, regex);

      if (ok) {
         MyPosition position;
         std::get<TUPLE_X>(position) = std::stoi(match[1].str());
         std::get<TUPLE_Y>(position) = std::stoi(match[2].str());
         std::get<TUPLE_DIST>(position) = std::stoi(match[3].str());

         data->positions.push_back(position);
      } else {
         throw std::runtime_error("Position format incorrect");
      }
   }
}

void load_data_goal(char** argv, DataPtr data) {
   std::string filename = argv[ARGV_TEST] + EXT_GOAL;
   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);

   // Load position
   std::string line = lines[0];
   std::regex regex("^[(]([0-9]+),([0-9]+)[)]$");
   std::smatch match;
   bool ok = std::regex_match(line, match, regex);

   if (ok) {
      MyPosition position;
      std::get<TUPLE_X>(position) = std::stoi(match[1].str());
      std::get<TUPLE_Y>(position) = std::stoi(match[2].str());

      data->goal = position;

      if (DEBUG) {
         std::cout << "Loaded Initial Position: "
                   << "("
                   << std::get<TUPLE_X>(data->initial)
                   << ","
                   << std::get<TUPLE_Y>(data->initial)
                   << ")"
                   << std::endl;
      }
   } else {
      throw std::runtime_error("Position format incorrect");
   }
}

void load_data_path(char** argv, DataPtr data) {
   std::string filename = argv[ARGV_TEST] + EXT_PATH;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);
   if (DEBUG) {
      std::cout << "Got Path with: " << lines.size() << " positions" << std::endl;
   }

   // Process each position
   for (std::string& line : lines) {
      std::regex regex("^[(]([0-9]+),([0-9]+),([0-9]+)[)]$");
      std::smatch match;
      bool ok = std::regex_match(line, match, regex);

      if (ok) {
         MyPosition position;
         std::get<TUPLE_X>(position) = std::stoi(match[1].str());
         std::get<TUPLE_Y>(position) = std::stoi(match[2].str());
         std::get<TUPLE_DIST>(position) = std::stoi(match[3].str());

         data->path.push_back(position);
      } else {
         throw std::runtime_error("Position format incorrect");
      }
   }
}

void load_lines(std::ifstream& in, std::vector<std::string>& lines) {
   while (!in.eof()) {
      std::string line;
      in >> line;

      // Drop newline
      if (line.back() == '\n') {
         line.pop_back();
      }

      // Add
      lines.push_back(line);
   }
   if (lines.back().size() == 0) {
      lines.pop_back();
   }
}

bool match_positions(MyPosition& posTest, PDPtr posrp) {
   bool match = false;

   match = std::get<TUPLE_X>(posTest) == posrp->getX()
           && std::get<TUPLE_Y>(posTest) == posrp->getY()
           && std::get<TUPLE_DIST>(posTest) == posrp->getDistance();

   return match;
}

bool run_unit_test(DataPtr data) {
   bool testPassed = false;

   // Create ReachablePositions
   if (DEBUG) {
      std::cout << "Create Position filter" << std::endl;
   }
   PathPlanning* rp =
      new PathPlanning(data->maze, data->rows, data->cols);

   // Set Initial Position
   rp->initialPosition(std::get<TUPLE_X>(data->initial),
                       std::get<TUPLE_Y>(data->initial));

   // Get final positions
   PDList* finalPositions = rp->getReachablePositions();
   int numPositions = finalPositions->size();
   if (numPositions > 0) {
      if (DEBUG) {
         std::cout << "Final Positions:" << std::endl;
         for (int i = 0; i != numPositions; ++i) {
            std::cout << "("
                    << finalPositions->get(i)->getX()
                    << ","
                    << finalPositions->get(i)->getY()
                    << ")"
                    << std::endl;
         }
      }

      // Test all Positions are expected
      if ((unsigned int) numPositions == data->positions.size()) {
         std::map<int,bool> checked;
         for(MyPosition& posTest : data->positions) {
            for (int i = 0; i != numPositions; ++i) {
               if (match_positions(posTest, finalPositions->get(i))) {
                  checked[i] = true;
               }
            }
         }
         
         // If not enough true items in the map, test failed
         testPassed = checked.size() == data->positions.size();

      }
   } else {
      if (DEBUG) {
         std::cout << "Reachable returned zero Positions" << std::endl;
      }
   }

   // Delete Position list
   delete finalPositions;
   
   // Do Milestone 3 tests
   if (data->m3) {
      std::cout << "Testing Milestone 3" << std::endl;
      PDList* path = rp->getPath(std::get<TUPLE_X>(data->goal),
                                 std::get<TUPLE_Y>(data->goal));
      numPositions = path->size();
      if (DEBUG) {
         std::cout << "Path:" << std::endl;
         for (int i = 0; i != numPositions; ++i) {
            std::cout << "("
                      << path->get(i)->getX()
                      << ","
                      << path->get(i)->getY()
                      << ")"
                      << std::endl;
        }
      }

      // Test all Positions are expected
      if ((unsigned int) numPositions == data->path.size()) {
         std::map<int,bool> checked;
         for(MyPosition& posTest : data->path) {
            for (int i = 0; i != numPositions; ++i) {
               if (match_positions(posTest, path->get(i))) {
                  checked[i] = true;
               }
            }
         }

        // If not enough true items in the map, test failed
        testPassed = checked.size() == data->path.size();
      }
       delete path;
   }

   // Delete Path Planner
   delete rp;
  
   return testPassed;
}

Grid make_grid(const int rows, const int cols) {
   Grid grid = NULL;

   if (rows >= 0 && cols >= 0) {
      grid = new char*[rows];
      for (int i = 0; i != rows; ++i) {
         grid[i] = new char[cols];
      }
   }

   return grid;
}


void delete_grid(Grid grid, int rows, int cols) {
   if (rows >= 0 && cols >= 0) {
      for (int i = 0; i != rows; ++i) {
         delete[] grid[i];
      }
      delete[] grid;
   }

   return;
}
