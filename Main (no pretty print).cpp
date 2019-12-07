/* 445166 - Bakkali Yahya
 * 461714 - Hauwaert Maxime
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
 * Solves the given rush hour instance.
 * @param n: height and width of the instance
 * @param vehicles: vector of movable blocks (i.e., vehicles)
 * @param fixed: vector of immovable blocks
 * @param k: number of solutions to find
 */
void solve(int n, vector<Block> &vehicles, vector<Block> fixed, int k) {

  int solutions_found = 0;
  int time_bound = 1;

  while (time_bound <= 100 && solutions_found < k) {

    Solver s;
    int vehicles_number = vehicles.size();
    int walls_number = fixed.size();
    int actions_number = 2;
    int Vehicles[n][n][time_bound+1][vehicles_number];
    int Moves[vehicles_number][time_bound][actions_number];

    vec<Lit> lits;

    // ajout des propositions
    for (int i = 0 ; i < n ; i++) {
      for (int j = 0 ; j < n ; j++) {
        for (int t = 0 ; t < time_bound+1 ; t++) {
          for (int v = 0 ; v < vehicles_number ; v++) {
            Vehicles[i][j][t][v] = s.newVar();
          }
        }
      }
    }

    for (int v = 0 ; v < vehicles_number ; v++) {
      for (int t = 0 ; t < time_bound ; t++) {
        for (int a = 0 ; a < actions_number ; a++) {
          Moves[v][t][a] = s.newVar();
        }
      }
    }


    // Initialisation du parking
    int startBoard[n][n];
    for (int i  = 0 ; i < n ; i++) {
      for (int j = 0 ; j < n ; j++) {
        startBoard[i][j]=-1;
        }
      }

    for (int v = 0 ; v < vehicles_number ; v++) {
      startBoard[vehicles[v].y][vehicles[v].x] = v;
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

    // À chaque instant, on ne peut avoir qu’au maximum un mouvement
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int a = 0 ; a < actions_number ; a++) {
          for (int v_ = 0 ; v_ < vehicles_number ; v_++) {
            for (int a_ = 0 ; a_ < actions_number ; a_++) {
              if (v != v_ || a != a_) {
                s.addBinary(~Lit(Moves[v][t][a]),~Lit(Moves[v_][t][a_]));
              }
            }
          }
        }
      }
    }

    // À chaque instant, il faut y avoir un mouvement
    for (int t = 0 ; t < time_bound ; t++) {
      lits.clear();
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int a = 0 ; a < actions_number ; a++) {
          lits.push(Lit(Moves[v][t][a]));
        }
      }
      s.addClause(lits);
    }


    // Au maximum une voiture à une position
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Horizontal) {
                for (int l = 0 ; j+l < n && l < vehicles[v].width ; l++) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i][j+l][t][v_]));
                }
              }
            }
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Vertical) {
                for (int l = 0 ; i+l < n && l < vehicles[v].height ; l++) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i+l][j][t][v_]));
                }
              }
            }
          }
        }
      }
    }


    // Toutes les voitures doivent être dans le parking

    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        lits.clear();
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            lits.push(Lit(Vehicles[i][j][t][v]));
          }
        }
        s.addClause(lits);
      }
    }

    // Il n’y a pas plusieurs fois la même voiture

    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int i_ = 0 ; i_ < n ; i_++) {
              for (int j_ = 0 ; j_ < n ; j_++) {
                if ((i_ != i) || (j_ != j)) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i_][j_][t][v]));
                }
              }
            }
          }
        }
      }
    }

    // Pas de collision entre les voitures et les murs du parking
    
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Horizontal) {
          for (int i = 0 ; i < n ; i++) {
            for (int l = 1 ; l < vehicles[v].width ; l++) {
              s.addUnit(~Lit(Vehicles[i][n-l][t][v]));
            }
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Vertical) {
          for (int j = 0 ; j < n ; j++) {
            for (int l = 1 ; l < vehicles[v].height ; l++) {
              s.addUnit(~Lit(Vehicles[n-l][j][t][v]));
            }
          }
        }
      }
    }

    // Pas de collision entre les voitures
    //  Voitures Horizontales — Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Horizontal && vehicles[v_].orientation == Horizontal) {
                for (int l = 1 ; j-l >= 0 && l < vehicles[v_].width ; l++) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i][j-l][t][v_]));
                }
              }
            }
          }
        }
      }
    }

    //  Voitures Horizontales — Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Horizontal && vehicles[v_].orientation == Vertical) {
                for (int l = 1 ; i-l >= 0 && l < vehicles[v_].height ; l++) {
                  for (int l_ = 0 ; j+l_ < n && l_ < vehicles[v].width ; l_++) {
                    s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i-l][j+l_][t][v_]));
                  }
                }
              }
            }
          }
        }
      }
    }

    //  Voitures Verticales — Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Vertical && vehicles[v_].orientation == Horizontal) {
                for (int l = 1 ; j-l >= 0 && l < vehicles[v_].width ; l++) {
                  for (int l_ = 0 ; i+l_ < n && l_ < vehicles[v].height ; l_++) {
                    s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i+l_][j-l][t][v_]));
                  }
                }
              }
            }
          }
        }
      }
    }

    //  Voitures Verticales — Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            for (int v_ = v+1 ; v_ < vehicles_number ; v_++) {
              if (vehicles[v].orientation == Vertical && vehicles[v_].orientation == Vertical) {
                for (int l = 1 ; i-l >= 0 && l < vehicles[v_].height ; l++) {
                  s.addBinary(~Lit(Vehicles[i][j][t][v]),~Lit(Vehicles[i-l][j][t][v_]));
                }
              }
            }
          }
        }
      }
    }

    // Pas de collision entre les voitures et les blocs fixes du parking
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int w = 0 ; w < walls_number ; w++) {
        for (int i = 0 ; i < fixed[w].height && fixed[w].y+i < n ; i++) {
          for (int j = 0 ; j < fixed[w].width && fixed[w].x+j < n; j++) {
            for (int v = 0 ; v < vehicles_number ; v++) {
              if (vehicles[v].orientation == Horizontal) {
                for (int j_ = 0 ; j_ < vehicles[v].width && fixed[w].x+j-j_ >= 0 ; j_++) {
                  s.addUnit(~Lit(Vehicles[fixed[w].y+i][fixed[w].x+j-j_][t][v]));
                }
              }
            }
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int w = 0 ; w < walls_number ; w++) {
        for (int i = 0 ; i < fixed[w].height && fixed[w].y+i < n ; i++) {
          for (int j = 0 ; j < fixed[w].width && fixed[w].x+j < n; j++) {
            for (int v = 0 ; v < vehicles_number ; v++) {            
              if (vehicles[v].orientation == Vertical) {
                for (int i_ = 0 ; i_ < vehicles[v].height && fixed[w].y+i-i_ >= 0 ; i_++) {
                  s.addUnit(~Lit(Vehicles[fixed[w].y+i-i_][fixed[w].x+j][t][v]));
                }
              }
            }
          }
        }
      }
    }

    // Les voitures ne doivent pas être en dehors de leur axe
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {
            if (vehicles[v].orientation == Horizontal && i != vehicles[v].y) {
              s.addUnit(~Lit(Vehicles[i][j][t][v]));
            }
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound+1 ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int i = 0 ; i < n ; i++) {
          for (int j = 0 ; j < n ; j++) {          
            if (vehicles[v].orientation == Vertical && j != vehicles[v].x) {
              s.addUnit(~Lit(Vehicles[i][j][t][v]));
            }
          }
        }
      }
    }

    // Vérifier que les actions /avancer/ ont été faites
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Horizontal) {
          for (int j = 0 ; j < n-1 ; j++) {
            lits.clear();
            lits.push(~Lit(Moves[v][t][1]));
            lits.push(~Lit(Vehicles[vehicles[v].y][j][t][v]));
            lits.push(Lit(Vehicles[vehicles[v].y][j+1][t+1][v]));
            s.addClause(lits);
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Vertical) {
          for (int i = 0 ; i < n-1 ; i++) {
            lits.clear();
            lits.push(~Lit(Moves[v][t][1]));
            lits.push(~Lit(Vehicles[i][vehicles[v].x][t][v]));
            lits.push(Lit(Vehicles[i+1][vehicles[v].x][t+1][v]));
            s.addClause(lits);
          }
        }
      }
    }

    // Vérifier que les actions /reculer/ ont été faites
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Horizontal) {
          for (int j = 1 ; j < n ; j++) {
            lits.clear();
            lits.push(~Lit(Moves[v][t][0]));
            lits.push(~Lit(Vehicles[vehicles[v].y][j][t][v]));
            lits.push(Lit(Vehicles[vehicles[v].y][j-1][t+1][v]));
            s.addClause(lits);
          }
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Vertical) {
          for (int i = 1 ; i < n ; i++) {
            lits.clear();
            lits.push(~Lit(Moves[v][t][0]));
            lits.push(~Lit(Vehicles[i][vehicles[v].x][t][v]));
            lits.push(Lit(Vehicles[i-1][vehicles[v].x][t+1][v]));
            s.addClause(lits);
          }
        }
      }
    }

    // Interdire les actions /reculer/ qui font sortir les voitures du parking
    //  Voitures Horizontales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Horizontal) {
          s.addBinary(~Lit(Moves[v][t][0]),~Lit(Vehicles[vehicles[v].y][0][t][v]));
        }
      }
    }

    //  Voitures Verticales
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        if (vehicles[v].orientation == Vertical) {
          s.addBinary(~Lit(Moves[v][t][0]),~Lit(Vehicles[0][vehicles[v].x][t][v]));
        }
      }
    }


    // Vérifier que les voitures non-affectées par l’action effectuée ne changent pas
    for (int t = 0 ; t < time_bound ; t++) {
      for (int v = 0 ; v < vehicles_number ; v++) {
        for (int a = 0 ; a < actions_number ; a++) {
          for (int i = 0 ; i < n ; i++) {
            for (int j = 0 ; j < n ; j++) {
              for (int v_ = 0 ; v_ < vehicles_number ; v_++) {
                lits.clear();
                lits.push(~Lit(Moves[v][t][a]));
                lits.push(Lit(Vehicles[i][j][t][v]));
                lits.push(Lit(Vehicles[i][j][t+1][v]));
                lits.push(~Lit(Vehicles[i][j][t][v_]));
                lits.push(Lit(Vehicles[i][j][t+1][v_]));
                s.addClause(lits);

                lits.clear();
                lits.push(~Lit(Moves[v][t][a]));
                lits.push(Lit(Vehicles[i][j][t][v]));
                lits.push(Lit(Vehicles[i][j][t+1][v]));
                lits.push(Lit(Vehicles[i][j][t][v_]));
                lits.push(~Lit(Vehicles[i][j][t+1][v_]));
                s.addClause(lits);
              }
            }
          }
        }
      }
    }

    // La voiture rouge doit se trouver à côté de la sortie du parking à la fin de la limite de temps
    s.addUnit(Lit(Vehicles[vehicles[0].y][n-2][time_bound][0]));

    bool can_continue = true;
    while (solutions_found < k && can_continue) {
      s.solve();
      if (!s.okay()) {
        can_continue = false;
      }
      else {
        lits.clear();
        for (int t = 0 ; t < time_bound ; t++) {
          for (int v = 0 ; v < vehicles_number ; v++) {
            for (int a = 0 ; a < actions_number ; a++) {
              if (s.model[Moves[v][t][a]] == l_True) {
                lits.push(~Lit(Moves[v][t][a]));
                std::cout << vehicles[v].id << " ";
                if (!a) {
                  if (vehicles[v].orientation == Horizontal) {
                    std::cout << "L" << std::endl;
                  }
                  else {
                    std::cout << "U" << std::endl;
                  }
                }
                else {
                  if (vehicles[v].orientation == Horizontal) {
                    std::cout << "R" << std::endl;
                  }
                  else {
                    std::cout << "D" << std::endl;
                  }
                }
              }
            }
          }
        }
        std::cout << std::endl;
        s.addClause(lits);
        solutions_found++;
      }
    }
    time_bound++;
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
