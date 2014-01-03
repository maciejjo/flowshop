#ifndef MACHINE_H
#define MACHINE_H

#include <queue>

#include "Job.h"

using namespace std;

class Machine {

  public:
    bool state;
    bool virgin;
    Job job;
    deque <Job> job_queue;

    Machine();

};

#endif
