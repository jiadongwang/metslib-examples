#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>

#include <metslib/mets.hh>

#include "qap_model.hpp"

using namespace std;

void usage()
{
  cerr << "qap qaplib.dat" << endl;
  ::exit(1);
}

typedef mets::swap_neighborhood<std::tr1::mt19937> swap_neighborhood_t;

int main(int argc, char* argv[]) 
{

  if(argc != 2) usage();
  ifstream in(argv[1]);
  if(!in.is_open()) usage();

  // random number generator from C++ TR1 extension
  std::tr1::mt19937 rng(time(NULL));

  // user defined problem
  qap_model problem_instance;
  in >> problem_instance;
  unsigned int N = problem_instance.size();

  // best solution instance used to record the best known solution.
  qap_model incumbent_solution(problem_instance);
  mets::best_ever_solution incumbent_recorder(incumbent_solution);

  // A neighborhood made of 2N random swaps
  swap_neighborhood_t neighborhood(rng, sqrt(N)*N);

  // generate a random starting point
  mets::random_shuffle(problem_instance, rng);

  // use framework provided strategies
  mets::simple_tabu_list tabu_list(N*sqrt(N));
  mets::best_ever_criteria aspiration_criteria;
      
  // fixed number of non improving moves before termination
  mets::forever never_stop;
  mets::exponential_cooling cooler(0.999);
	  
  // the search algorithm
  mets::simulated_annealing<swap_neighborhood_t> algorithm(problem_instance, 
							   incumbent_recorder, 
							   neighborhood, 
							   never_stop,
							   cooler,
							   7500,
							   100);
  
  // log to standard error
  mets::iteration_logger<swap_neighborhood_t> g(clog);
  algorithm.attach(g);
  algorithm.search();
	  
  // write solution to standard output
  cout << fixed << N << " " <<  incumbent_solution.cost_function() << endl
       << incumbent_solution << endl;
}
