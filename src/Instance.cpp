#include "Instance.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

/*
  for(int i = 0; i < number_of_intervals; i++) {
    for(int j = 0; j < intervals[i][0]; j++) {
      this->jobs_array.push_back(new Job(intervals[i][1], intervals[i][2], ready_max));
    }
  }
*/

Instance::Instance(queue<Range> *ranges) {

  while(!ranges->empty()) {
    printf("(Instance::Instance) Dodaję przedział [quantity: %d, time: <%d, %d>, ready: <%d, %d>]\n", ranges->front().size, ranges->front().min_duration, ranges->front().max_duration, ranges->front().min_ready_time, ranges->front().max_ready_time);
    for(int i = 0; i < ranges->front().size; i++) {
      jobs.push_back(Job(ranges->front().max_duration, ranges->front().min_duration,ranges->front().max_ready_time,ranges->front().min_ready_time));
    }
    ranges->pop();
  }

}

vector<Job> Instance::GetReadyJobs(int moment) {

  vector <Job> ready_jobs;

  for(int i = jobs.size() - 1; i >= 0; i--) {
    if(jobs[i].ready_time <= moment) {
      ready_jobs.push_back(jobs[i]);
      jobs.pop_back();
    }
  }
  return ready_jobs;
}

void Instance::SortJobs() {

  sort(jobs.begin(), jobs.end(), compareReadyTime);
}

int compareReadyTime(Job a, Job b) {
  return (a.ready_time > b.ready_time);
}


int Instance::RandomScheduling(Machine machines[3]) {

  printf("(Instance::RandomScheduling) Welcome!\n");
  printf("(Instance::RandomScheduling) Sortuję zadania...\n");
  SortJobs();

  printf("(Instance::RandomScheduling) Rozpoczynam pętlę czasu\n");
  for(int time = 0; time < 100; time++) {
  printf("(Instance::RandomScheduling) ======================== \n");

    printf("(Instance::RandomScheduling)[%d] Pobieram listę gotowych zadań\n", time);
    vector <Job> ready_jobs = GetReadyJobs(time);

    printf("(Instance::RandomScheduling)[%d] Mieszam listę gotowych zadań\n", time);
    random_shuffle(ready_jobs.begin(), ready_jobs.end());

    for(int i = ready_jobs.size() - 1; i >= 0; i--) {
      printf("(Instance::RandomScheduling)[%d] Wrzucam na 1. maszynę zadanie %d[%d|",
        time, ready_jobs[i].id, ready_jobs[i].ready_time);
      for(int j = 0; j < 3; j++) printf(" %3d", ready_jobs[i].operation_times[j]);
      printf("]\n");
      machines[0].job_queue.push(&ready_jobs[i]);
      ready_jobs.pop_back();
    }

  if(jobs.size() == 0)
    break;

  }

  return 0;

}
