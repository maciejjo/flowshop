#ifndef INSTANCE_H
#define INSTANCE_H

#include "Job.h"
#include "Range.h"
#include "Machine.h"

#include <vector>
#include <queue>


using namespace std;

class Instance {
  public:
    vector<Job> jobs;
    Instance(queue<Range> *ranges);

    int RandomScheduling(Machine machines[3]);
    vector<Job> GetReadyJobs(int moment);
    void SortJobs();

};

int compareReadyTime(Job a, Job b);

#endif
