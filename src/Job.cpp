#include <cstdlib>

#include "Job.h"

Job::Job(int max, int min, int ready_max, int ready_min) {
  
  for(int i = 0; i < 3; i++)
    this->operation_times[i] = rand() % (max - min) + min;
  this->ready_time = rand() % (ready_max - ready_min) + ready_min;
  this->id = last_id++;

}

Job::Job() {
}
