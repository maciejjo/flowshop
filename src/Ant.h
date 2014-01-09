#ifndef ANT_H
#define ANT_H

#include "Instance.h"
#include "Pheromones.h"

class Ant {
  public:

    Ant(Instance *instance, int number_of_ants, int number_of_jobs);

    Instance instance;
    deque<int> job_list[3];

};

#endif
