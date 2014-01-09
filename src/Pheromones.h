#ifndef PHEROMONES_H
#define PHEROMONES_H

#include "Instance.h"
#include "Machine.h"
#include "Generation.h"
#include "Ant.h"

class Pheromones {
  public:

    Pheromones(int number_of_jobs);
    int SpinRoulette(Machine machine, int number_of_jobs, int machine_no, int alpha, int beta); 
    void Evaluate(Generation gen, int number_of_ants, int number_of_jobs, float pec, float q); 

    float **pheromone_levels[3];
    int best_cmax;

};

#endif
