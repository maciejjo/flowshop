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
    vector<Job> original_jobs;
    bool display;
    int number_of_halts;
    int number_of_jobs;
    int max_halt_time;
    int mean_operation_time;

    Instance(queue<Range> *ranges, bool display, int number_of_halts, int number_of_jobs, int max_halt_time);

    int RandomScheduling(Machine machines[3]);
    int ShortestJobScheduling(Machine machines[3]);
    vector<Job> GetReadyJobs(int moment);
    void SortJobs();

};

int compareReadyTime(Job a, Job b);
int compareReadyTime(Job a, Job b);
int compareOpTimes0(Job a, Job b);
int compareOpTimes1(Job a, Job b);
int compareOpTimes2(Job a, Job b);

#endif
