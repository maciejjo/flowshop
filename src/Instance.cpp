#include "Instance.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

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

//  printf("Na liście zadań pozostało %lu zadań\n", jobs.size());
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
  for(int time = 0;; time++) {
    printf("(Instance::RandomScheduling) ======================== \n");

//    printf("(Instance::RandomScheduling)[%d] Pobieram listę gotowych zadań\n", time);
    vector <Job> ready_jobs = GetReadyJobs(time);

//    printf("(Instance::RandomScheduling)[%d] Mieszam listę gotowych zadań\n", time);
    random_shuffle(ready_jobs.begin(), ready_jobs.end());


    for(int i = ready_jobs.size() - 1; i >= 0; i--) {

      /*
      printf("(Instance::RandomScheduling)[%d] Zadanie osiąga gotowość:\t\t%d[%d|",
        time, ready_jobs[i].id, ready_jobs[i].ready_time);
      for(int j = 0; j < 3; j++) printf(" %d", ready_jobs[i].operation_times[j]);
      printf("]\n");
      */

      machines[0].job_queue.push_back(ready_jobs[i]);
      ready_jobs.pop_back();
      printf("(Instance::RandomScheduling)[%d] Zadanie dodano do pierwszej kolejki:\t\t%d[%d|",
        time, machines[0].job_queue.back().id, machines[0].job_queue.back().ready_time);
      for(int j = 0; j < 3; j++) printf(" %d", machines[0].job_queue.back().operation_times[j]);
      printf("]\n");

    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {
        printf("(Instance::RandomScheduling)[%d] == Moduł wrzucania zadań na wolną maszynę\n", time);
        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

        printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi na %d maszynę:\t\t%d[%d|",
          time, i, machines[i].job_queue.front().id, machines[i].job_queue.front().ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job_queue.front().operation_times[j]);
        printf("]\n");
        
        printf("(Instance::RandomScheduling)[%d] | kolejka %d:\t[ ", time, i);
        for(int j = 0; j < machines[i].job_queue.size(); j++) printf("%d ", machines[i].job_queue[j].id);
        printf("]\n");

        machines[i].state = 1;
        machines[i].job = machines[i].job_queue.front();
        machines[i].job_queue.pop_front();

        /*
        printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi na %d[%d] maszynę:\t%d[%d|",
          time, i, machines[i].state, machines[i].job.id, machines[i].job.ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
        printf("]\n");
        */

        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
      }
    }

    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] == 0)) {
        printf("(Instance::RandomScheduling)[%d] == Moduł przekładający zadania na kolejki\n", time);
        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

        printf("(Instance::RandomScheduling)[%d] | Zadanie zakończone na maszynie %d:\t\t%d[%d|",
          time, i, machines[i].job.id, machines[i].job.ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
        printf("]\n");

        machines[i].state = 0;
        if(i < 2) {
          machines[i+1].job_queue.push_back(machines[i].job);

          printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi do kolejki %d:\t\t%d[%d|",
            time, i+1, machines[i+1].job_queue.front().id, machines[i+1].job_queue.front().ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", machines[i+1].job_queue.front().operation_times[j]);
          printf("]\n");
          printf("(Instance::RandomScheduling)[%d] | kolejka %d:\t[ ", time, i+1);
          for(int j = 0; j < machines[i+1].job_queue.size(); j++) printf("%d ", machines[i+1].job_queue[j].id);
          printf("]\n");

        }

        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
      }
    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] > 0)) {

        printf("(Instance::RandomScheduling)[%d] == Moduł wykonujący zadania\n", time);
        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

        printf("(Instance::RandomScheduling)[%d] | Zadanie wykonuje się na maszynie %d:\t\t%d[%d|",
          time, i, machines[i].job.id, machines[i].job.ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
        printf("]\n");

        machines[i].job.operation_times[i]--;

        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
      }
    }
    
//    if(jobs.empty() && ready_jobs.empty() && machines[0].state == 0 && machines[1].state == 0 && machines[2].state == 0) break;
    int stop = 1;
    for(int i = 0; i < 3; i++) {
      if(!machines[i].job_queue.empty()) stop = 0;
    }

    if(jobs.empty() && ready_jobs.empty() && machines[0].state == 0 && machines[1].state == 0 && machines[2].state == 0 && stop) {
      
      printf("Cmax = %d\n", time);
      break;
    }
      

  }

  return 0;

}
