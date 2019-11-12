/* Matricule - Nom Prenom
 * (Matricule2 - Nom2 Prenom2)
 *
 */
#include "Solver.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;
const char WALL = '#';
const int POS = 0;
const int SIZE = 1;
const int ORIENTATION = 2;
const int LINES_PER_RECORD = 4;
enum Orientation { Horizontal, Vertical };
struct Block {
  string id;
  int x;
  int y;
  int width;
  int height;
  Orientation orientation;
};

/**
 * Pretty print the list of blocks
 * @param blocks: vector of Blocks
 */
void pretty_print(vector<Block> &blocks) {
  for (unsigned i = 0; i < blocks.size(); i++) {
    if (blocks[i].id == "RE") {
      std::cout << "GOAL CAR" << std::endl;
    }
    std::cout << blocks[i].id << std::endl;
    std::cout << "at: " << blocks[i].x << ", " << blocks[i].y << std::endl;
    std::cout << "size: " << blocks[i].width << ", " << blocks[i].height
              << std::endl;
    std::cout << "orientation: "
              << (blocks[i].orientation == Horizontal ? "horizontal"
                                                      : "vertical")
              << std::endl;
  }
}

/**
 * Solves the given rush hour instance.
 * @param n: height and width of the instance
 * @param vehicles: vector of movable blocks (i.e., vehicles)
 * @param fixed: vector of immovable blocks
 * @param k: number of solutions to find
 */
void solve(int n, vector<Block> &vehicles, vector<Block> fixed, int k) {
  pretty_print(vehicles);
  pretty_print(fixed);

  // A COMPLETER
  Solver s;
  int time_bound = 100;
  int vehicles_number = vehicles.size();
  int action = 2;
  // horizontal 0 is left, 1 is right
  // vertical   0 is bot, 1 is top 
  int Vehicles[n][n][time_bound][vehicles_number];
  int Moves[n][n][time_bound][action];

  vec<Lit> lits;

  // ajout des propositions
  for (int i = 0 ; i < n ; i++) {
    for (int j = 0 ; j < n ; j++) {
      for (int t = 0 ; t < time_bound ; t++) {
        for (int v = 0 ; v < vehicles_number ; v++) {
          Vehicles[i][j][t][v] = s.newVar();
        }
        for (int a = 0 ; a < action ; a++) {
            Moves[i][j][t][a] = s.newVar();
          }
      }
    }
  }

  // initial configuration
  int startBoard[n][n];
  for (int i  = 0 ; i < n ; i++) {for (int j = 0 ; j < n ; j++){startBoard[i][j]=-1;}}

  for (int v = 0 ; v < vehicles_number ; v++) {
    startBoard[vehicles[v].x][vehicles[v].y] = v;
  }

  for (int i = 0 ; i < n ; i++) {
    for (int j = 0 ; j < n ; j++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (startBoard[i][j] == v) {
          s.addUnit(Lit(Vehicles[i][j][0][v]));
        }
        else {
          s.addUnit(~Lit(Vehicles[i][j][0][v]));
        }
      }
    }
  }

  // at most one move per round
  for (int t = 0 ; t < time_bound ; t++) {
    for (int i = 0 ; i < n ; i++) {
      for (int j = 0 ; j < n ; j++) {
        for (int a = 0 ; a < action ; a++) {
          for (int i_ = 0 ; i_ < n ; i_++) {
            for (int j_ = 0 ; j_ < n ; j_++) {
              for (int a_ = 0 ; a_ < action ; a_++) {
                if (i != i_ || j != j_ || a != a_) {
                  s.addBinary(~Lit(Moves[i][j][t][a]),~Lit(Moves[i_][j_][t][a_]));
                }
              }
            }
          }
        }
      }
    }
  }

  // at most one vehicle per case
  for (int t = 0 ; t < time_bound ; t++) {
    for (int i = 0 ; i < n ; i++) {
      for (int j = 0 ; j < n ; j++) {
        for (int v = 0 ; v < vehicles_number ; v++) {
          for (int i_ = 0 ; i_ < n ; i_++) {
            for (int j_ = 0 ; j_ < n ; j_++) {
              for (int v_ = 0 ; v_ < vehicles_number ; v_++) {
                if (i != i_ || j != j_ || v != v_) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i_][j_][t][v_]));
                }
              }
            }
          }
        }
      }
    }
  }

  // Vérifier no collision

  // Moves
  for (int t = 0 ; t < time_bound-1 ; t++) {
    for (int i = 0 ; i < n ; i++) {
      for (int j = 0 ; j < n ; j++) {
        // Vérifier que le move a bien été fait
      }
    }
  }

  // Finish
  lits.clear();
  for (int t = 0 ; t < time_bound ; t++) {
    lits.push(Lit(Vehicles[vehicles[0].x][n-2][t][0]));
  }
  s.addClause(lits);

  s.solve();
  if (!s.okay()) {
    cout << "pas de solution" << endl;
  }
  else {

  }
}

/**
 * Prints program help message
 * @param program: program name
 */
void print_help(char *program) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << program << " [-k NOMBRE]" << std::endl;
}

int main(int argc, char **argv) {
  int k = 1;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-k") == 0) {
      try {
        k = std::stoi(argv[i + 1]);
      } catch (const std::logic_error &e) {
        print_help(argv[0]);
        return EXIT_FAILURE;
      }
    } else if ((strcmp(argv[i], "--help") == 0) ||
               (strcmp(argv[i], "-h") == 0)) {
      print_help(argv[0]);
      return EXIT_SUCCESS;
    }
  }

  // read instance on standard input
  string line;
  int n;
  std::cin >> n;
  std::getline(std::cin, line); // skip endline after dimensions

  vector<Block> vehicles; // vector of movable vehicles
  vector<Block> fixed;    // vector of immovable walls
  while (std::getline(std::cin, line)) {
    string id = line;
    int x, y, width, height;
    Orientation orientation;
    for (int i = 0; i < LINES_PER_RECORD - 1; i++) {
      std::getline(std::cin, line);
      std::istringstream ss(line);

      switch (i) {
      case POS:
        ss >> x >> y;
        break;
      case SIZE:
        ss >> width >> height;
        break;
      case ORIENTATION:
        orientation = line[0] == 'h' ? Horizontal : Vertical;
        break;
      }
    }

    Block new_block = {id, x, y, width, height, orientation};
    if (id[0] == WALL) {
      fixed.push_back(new_block);
    } else {
      vehicles.push_back(new_block);
    }
  }

  solve(n, vehicles, fixed, k);

  return EXIT_SUCCESS;
}

CNF ((H) && (~C && (~VH) && (~VHL3) && (~VV) && (~VVL3))) || ((V) && (~C2 && (~VV2) && (~VVL32) && (~VH2) && (~VHL32)))
CNF ((A) && (~B && (~C) && (~D) && (~E) && (~F))) || ((G) && (~H && (~I) && (~J) && (~K) && (~L)))