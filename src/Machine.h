#ifndef MACHINE_H
#define MACHINE_H

#include <queue>

#include "Job.h"

using namespace std;

class Machine {

  public:
    int state;
    Job job;
    queue <Job*> job_queue;

    Machine();

};

#endif
