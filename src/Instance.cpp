#include "Instance.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

//Instance::Instance(queue<Range> *ranges, bool display) {
Instance::Instance(queue<Range> *ranges, bool display, int number_of_halts, int number_of_jobs, int max_halt_time) {

  int sum_of_first_jobs = 0;
  while(!ranges->empty()) {
    if(display) 
      printf("(Instance::Instance) Dodaję przedział [quantity: %d, time: <%d, %d>, ready: <%d, %d>]\n", ranges->front().size, ranges->front().min_duration, ranges->front().max_duration, ranges->front().min_ready_time, ranges->front().max_ready_time);
    for(int i = 0; i < ranges->front().size; i++) {
      jobs.push_back(Job(ranges->front().max_duration, ranges->front().min_duration,ranges->front().max_ready_time,ranges->front().min_ready_time));
      sum_of_first_jobs += jobs.back().operation_times[0];
    }
    ranges->pop();
  }
  original_jobs = jobs;
  this->display = display;
  this->number_of_halts = number_of_halts;
  this->number_of_jobs = number_of_jobs;
  this->max_halt_time = max_halt_time;
  this->mean_operation_time = sum_of_first_jobs / number_of_jobs;

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

  int task_count = 0;
  int guaranteed_halt = 0;
  if(number_of_halts > 0) {
  guaranteed_halt = number_of_jobs / number_of_halts;
  }
  bool force_halt = false;
  bool was_halt = false;
  int halts = 0;
  int halt_left = 0;
  bool halt_start = 0;

  if(display) {
    printf("(Instance::RandomScheduling) Welcome!\n");
    printf("(Instance::RandomScheduling) Sortuję zadania...\n");
  }
  SortJobs();

  if(display) {
    printf("(Instance::RandomScheduling) Rozpoczynam pętlę czasu\n");
  }

    for(int time = 0;; time++) {


    if(display) {
      printf("(Instance::RandomScheduling) ======================== \n");
    }

    if(guaranteed_halt) {
      int chance = rand() % ( mean_operation_time * guaranteed_halt);
      if((halts < number_of_halts) && (!chance || force_halt)) {

        was_halt = true;
        force_halt = false;
        halt_start = true;
        halts++;
        halt_left = (rand() % max_halt_time) + 1;
      }
    }

    vector <Job> ready_jobs = GetReadyJobs(time);
    random_shuffle(ready_jobs.begin(), ready_jobs.end());

    for(int i = ready_jobs.size() - 1; i >= 0; i--) {

      machines[0].job_queue.push_back(ready_jobs[i]);
      ready_jobs.pop_back();

      if(display) {
        printf("(Instance::RandomScheduling)[%d] Zadanie dodano do pierwszej kolejki:\t\t%d[%d|",
          time, machines[0].job_queue.back().id, machines[0].job_queue.back().ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[0].job_queue.back().operation_times[j]);
        printf("]\n");
      }

    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {

      if(display) {
        printf("(Instance::RandomScheduling)[%d] == Moduł wrzucania zadań na wolną maszynę\n", time);
        printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

        printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi na %d maszynę:\t\t%d[%d|",
          time, i, machines[i].job_queue.front().id, machines[i].job_queue.front().ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job_queue.front().operation_times[j]);
        printf("]\n");

        printf("(Instance::RandomScheduling)[%d] | kolejka %d:\t[ ", time, i);
        for(int j = 0; j < machines[i].job_queue.size(); j++) printf("%d ", machines[i].job_queue[j].id);
        printf("]\n");
      }

        machines[i].state = 1;
        machines[i].job = machines[i].job_queue.front();
        machines[i].job_queue.pop_front();
        if(i == 0)
        {
          task_count++;
          if(guaranteed_halt > 0) {
          if(!(task_count % guaranteed_halt)) {
            if(!was_halt) {
              force_halt = true;
            }
            else
              was_halt = false;
            }
          }
        }

        if(display) {
          printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }
      }
    }

    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] == 0)) {

        if(display) {
          printf("(Instance::RandomScheduling)[%d] == Moduł przekładający zadania na kolejki\n", time);
          printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

          printf("(Instance::RandomScheduling)[%d] | Zadanie zakończone na maszynie %d:\t\t%d[%d|",
            time, i, machines[i].job.id, machines[i].job.ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
          printf("]\n");
        }

        machines[i].state = 0;
        if(i < 2) {
          machines[i+1].job_queue.push_back(machines[i].job);


          if(display) {
            printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi do kolejki %d:\t\t%d[%d|",
              time, i+1, machines[i+1].job_queue.front().id, machines[i+1].job_queue.front().ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", machines[i+1].job_queue.front().operation_times[j]);
            printf("]\n");
            printf("(Instance::RandomScheduling)[%d] | kolejka %d:\t[ ", time, i+1);
            for(int j = 0; j < machines[i+1].job_queue.size(); j++) printf("%d ", machines[i+1].job_queue[j].id);
            printf("]\n");
          }

        }

        if(display) {
          printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }

      }
    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] > 0)) {

        if(display) {
          printf("(Instance::RandomScheduling)[%d] == Moduł wykonujący zadania\n", time);
          printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);

          printf("(Instance::RandomScheduling)[%d] | Zadanie wykonuje się na maszynie %d:\t\t%d[%d|",
            time, i, machines[i].job.id, machines[i].job.ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
          printf("]\n");
        }

        if(i > 0) {
          machines[i].job.operation_times[i]--;
        }
        else {
          if(!halt_left) {
            machines[i].job.operation_times[i]--;
          }
          if(halt_start) {
            machines[0].job.operation_times[0] = original_jobs[task_count].operation_times[0];
            halt_start = false;
            machines[0].state = 1;
          }
        }


        if(display) {
          printf("(Instance::RandomScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }
      }

    }
      if(halt_left) {
        if(display)
          printf("(Instance::RandomScheduling)[%d] Na maszynie 0 trwa przestój! Pozostało %d...\n", time, halt_left);
        halt_left--;
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




int Instance::ShortestJobScheduling(Machine machines[3]) {

  int task_count = 0;
  int guaranteed_halt = 0;
  if(number_of_halts > 0) {
  guaranteed_halt = number_of_jobs / number_of_halts;
  }
  bool force_halt = false;
  bool was_halt = false;
  int halts = 0;
  int halt_left = 0;
  bool halt_start = 0;

  if(display) {
    printf("(Instance::ShortestJobScheduling) Welcome!\n");
    printf("(Instance::ShortestJobScheduling) Sortuję zadania...\n");
  }
  SortJobs();

  if(display) {
    printf("(Instance::ShortestJobScheduling) Rozpoczynam pętlę czasu\n");
  }

    for(int time = 0;; time++) {


    if(display) {
      printf("(Instance::ShortestJobScheduling) ======================== \n");
    }

    if(guaranteed_halt) {
      int chance = rand() % ( mean_operation_time * guaranteed_halt);
      if((halts < number_of_halts) && (!chance || force_halt)) {

        was_halt = true;
        force_halt = false;
        halt_start = true;
        halts++;
        halt_left = (rand() % max_halt_time) + 1;
      }
    }

    vector <Job> ready_jobs = GetReadyJobs(time);

    for(int i = ready_jobs.size() - 1; i >= 0; i--) {

      machines[0].job_queue.push_back(ready_jobs[i]);
      ready_jobs.pop_back();

      if(display) {
        printf("(Instance::ShortestJobScheduling)[%d] Zadanie dodano do pierwszej kolejki:\t\t%d[%d|",
          time, machines[0].job_queue.back().id, machines[0].job_queue.back().ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[0].job_queue.back().operation_times[j]);
        printf("]\n");
      }

    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {

      sort(machines[i].job_queue.begin(), machines[i].job_queue.end(), compareReadyTime);

      if(display) {
        printf("(Instance::ShortestJobScheduling)[%d] == Moduł wrzucania zadań na wolną maszynę\n", time);
        printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);

        printf("(Instance::ShortestJobScheduling)[%d] | Zadanie przechodzi na %d maszynę:\t\t%d[%d|",
          time, i, machines[i].job_queue.front().id, machines[i].job_queue.front().ready_time);
        for(int j = 0; j < 3; j++) printf(" %d", machines[i].job_queue.front().operation_times[j]);
        printf("]\n");

        printf("(Instance::ShortestJobScheduling)[%d] | kolejka %d:\t[ ", time, i);
        for(int j = 0; j < machines[i].job_queue.size(); j++) printf("%d ", machines[i].job_queue[j].id);
        printf("]\n");
      }

        machines[i].state = 1;
        machines[i].job = machines[i].job_queue.front();
        machines[i].job_queue.pop_front();
        if(i == 0)
        {
          task_count++;
          if(guaranteed_halt > 0) {
          if(!(task_count % guaranteed_halt)) {
            if(!was_halt) {
              force_halt = true;
            }
            else
              was_halt = false;
            }
          }
        }

        if(display) {
          printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }
      }
    }

    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] == 0)) {

        if(display) {
          printf("(Instance::ShortestJobScheduling)[%d] == Moduł przekładający zadania na kolejki\n", time);
          printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);

          printf("(Instance::ShortestJobScheduling)[%d] | Zadanie zakończone na maszynie %d:\t\t%d[%d|",
            time, i, machines[i].job.id, machines[i].job.ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
          printf("]\n");
        }

        machines[i].state = 0;
        if(i < 2) {
          machines[i+1].job_queue.push_back(machines[i].job);


          if(display) {
            printf("(Instance::ShortestJobScheduling)[%d] | Zadanie przechodzi do kolejki %d:\t\t%d[%d|",
              time, i+1, machines[i+1].job_queue.front().id, machines[i+1].job_queue.front().ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", machines[i+1].job_queue.front().operation_times[j]);
            printf("]\n");
            printf("(Instance::ShortestJobScheduling)[%d] | kolejka %d:\t[ ", time, i+1);
            for(int j = 0; j < machines[i+1].job_queue.size(); j++) printf("%d ", machines[i+1].job_queue[j].id);
            printf("]\n");
          }

        }

        if(display) {
          printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }

      }
    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] > 0)) {

        if(display) {
          printf("(Instance::ShortestJobScheduling)[%d] == Moduł wykonujący zadania\n", time);
          printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);

          printf("(Instance::ShortestJobScheduling)[%d] | Zadanie wykonuje się na maszynie %d:\t\t%d[%d|",
            time, i, machines[i].job.id, machines[i].job.ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
          printf("]\n");
        }

        if(i > 0) {
          machines[i].job.operation_times[i]--;
        }
        else {
          if(!halt_left) {
            machines[i].job.operation_times[i]--;
          }
          if(halt_start) {
            machines[0].job.operation_times[0] = original_jobs[task_count].operation_times[0];
            halt_start = false;
            machines[0].state = 1;
          }
        }


        if(display) {
          printf("(Instance::ShortestJobScheduling)[%d] --------------------------------------------------------------------------------\n", time);
        }
      }

    }
      if(halt_left) {
        if(display)
          printf("(Instance::ShortestJobScheduling)[%d] Na maszynie 0 trwa przestój! Pozostało %d...\n", time, halt_left);
        halt_left--;
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
