#include "Instance.h"
#include "Pheromones.h"
#include "Generation.h"
#include "Ant.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

Instance::Instance() {};

Instance::Instance(queue<Range> *ranges, bool display, int number_of_halts, 
    int number_of_jobs, int max_halt_time) {

  int sum_of_first_jobs = 0;
  while(!ranges->empty()) {
    if(display) 

    for(int i = 0; i < ranges->front().size; i++) {
      jobs.push_back(Job(ranges->front().max_duration, 
            ranges->front().min_duration, ranges->front().max_ready_time,
            ranges->front().min_ready_time));
      sum_of_first_jobs += jobs.back().operation_times[0];
    }
    ranges->pop();
  }
  original_jobs = jobs;
  this->display = display;
  this->number_of_halts = number_of_halts;
  this->number_of_jobs = number_of_jobs;
  this->max_halt_time = max_halt_time;
  mean_operation_time = sum_of_first_jobs / number_of_jobs;

}

vector<Job> Instance::GetReadyJobs(int time) {

  vector <Job> ready_jobs;

  for(int i = jobs.size() - 1; i >= 0; i--) {
    if(jobs[i].ready_time <= time) {
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

int compareOpTimes0(Job a, Job b) {
 return (a.operation_times[0] < b.operation_times[0]);
}

int compareOpTimes1(Job a, Job b) {
  return (a.operation_times[1] < b.operation_times[1]);
}

int compareOpTimes2(Job a, Job b) {
  return (a.operation_times[2] < b.operation_times[2]);
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

  SortJobs();

    for(int time = 0;; time++) {

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
    }


    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {

        machines[i].state = 1;
        random_shuffle(machines[i].job_queue.begin(), machines[i].job_queue.end());
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
      }
    }

    for(int i = 0; i < 3; i++) {
      if((machines[i].state == 1) && (machines[i].job.operation_times[i] == 0)) {

        if(display) {
          printf("(Instance::RandomScheduling)[%d] == Moduł przekładający zadania na kolejki\n", time);
          printf("(Instance::RandomScheduling)[%d] ----------\n", time);

          printf("(Instance::RandomScheduling)[%d] | Zadanie zakończone na maszynie %d:\t\t%d[%d|",
            time, i, machines[i].job.id, machines[i].job.ready_time);
          for(int j = 0; j < 3; j++) printf(" %d", 
              machines[i].job.operation_times[j]);
          printf("]\n");
        }

        machines[i].state = 0;
        if(i < 2) {
          machines[i+1].job_queue.push_back(machines[i].job);


          if(display) {
            printf("(Instance::RandomScheduling)[%d] | Zadanie przechodzi do \
                kolejki %d:\t\t%d[%d|",
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

      printf("RND %d\n", time);
      break;
    }


  }

  return 0;

}

int Instance::ACOScheduling(Machine machines[3]) {

  // ACO Parameters
  int number_of_ants                = 5;
  int number_of_gens                = 2000;
  /*
  float vapor_level                 = 0.01;
  float pheromone_reward            = 0.9;
  float better_than_best_multiplier = 2.0;
  float pheromone_degradation       = 0.1;
  int prob_rul                      = 100;
  int start_rul                     = 100;
  int elite                         = 10;
  */
  // ----

  Pheromones pheromone_levels(number_of_jobs);

  vector<Generation> generations;
  for(int i = 0; i < number_of_gens; i++)
    generations.push_back(Generation(number_of_ants, number_of_jobs));

  for(int gen = 0; gen < number_of_gens; gen++) {
    printf("[TAB %d GEN]\n", gen);

    vector<Ant> ants_vector;
    for(int i = 0; i < number_of_ants; i++) {
      ants_vector.push_back(Ant(this, number_of_ants, number_of_jobs));
    }
    
    for(int ant = 0; ant < number_of_ants; ant++) {

      ants_vector[ant].instance.SortJobs();

      for(int time = 0;; time++) {

      // In every moment get list of ready jobs
      vector <Job> ready_jobs = ants_vector[ant].instance.GetReadyJobs(time);

      // Add every ready job to first queue
      for(int i = ready_jobs.size() - 1; i >= 0; i--) {
        machines[0].job_queue.push_back(ready_jobs[i]);
        ready_jobs.pop_back();
              printf("(Instance::AcoScheduling)[%d] Zadanie dodano do pierwszej kolejki:\t\t%d[%d|",
                time, machines[0].job_queue.back().id, machines[0].job_queue.back().ready_time);
              for(int j = 0; j < 3; j++) printf(" %d", machines[0].job_queue.back().operation_times[j]);
              printf("]\n");
      }


      // For every machine, check if free and has jobs waiting
      for(int i = 0; i < 3; i++) {
        if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {

          // Set machine i to busy
          machines[i].state = 1;

          // Choose job with roulette 
          int roulette_job = pheromone_levels.SpinRoulette(machines[i], ants_vector[ant].instance.number_of_jobs, i, 1, 0.5);
          Job tmp_job = machines[i].job_queue.front();
          machines[i].job_queue.front() = machines[i].job_queue[roulette_job];
          machines[i].job_queue[roulette_job] = tmp_job;
          
          if(display) {
            printf("(Instance::AcoScheduling)[%d] == Moduł wrzucania zadań na wolną maszynę\n", time);
            printf("(Instance::AcoScheduling)[%d] --------------------------------------------------------------------------------\n", time);

            printf("(Instance::AcoScheduling)[%d] | Zadanie przechodzi na %d maszynę:\t\t%d[%d|",
              time, i, machines[i].job_queue.front().id, machines[i].job_queue.front().ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", machines[i].job_queue.front().operation_times[j]);
            printf("]\n");

            printf("(Instance::AcoScheduling)[%d] | kolejka %d:\t[ ", time, i);
            for(int j = 0; j < machines[i].job_queue.size(); j++) printf("%d ", machines[i].job_queue[j].id);
            printf("]\n");
          }

          machines[i].job = machines[i].job_queue.front();
          ants_vector[ant].job_list[i].push_back(machines[i].job_queue.front().id);

          machines[i].job_queue.pop_front();
          if(machines[i].virgin)
            machines[i].virgin = false;

        }
      }

      // For every machine, check if busy and job ended
      for(int i = 0; i < 3; i++) {
        if((machines[i].state == 1) && (machines[i].job.operation_times[i] == 0)) {

          if(display) {
            printf("(Instance::ACOScheduling)[%d] == Moduł przekładający zadania na kolejki\n", time);
            printf("(Instance::ACOScheduling)[%d] --------------------------------------------------------------------------------\n", time);
            printf("(Instance::ACOScheduling)[%d] | Zadanie zakończone na maszynie %d:\t\t%d[%d|",
              time, i, machines[i].job.id, machines[i].job.ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", 
                machines[i].job.operation_times[j]);
            printf("]\n");
          }

          // Set to free
          machines[i].state = 0;


          // For every machine except for last
          if(i < 2) {

            //Add job to next queue
            machines[i+1].job_queue.push_back(machines[i].job);

            if(display) {
              printf("(Instance::ACOScheduling)[%d] | Zadanie przechodzi do \
                  kolejki %d:\t\t%d[%d|",
                time, i+1, machines[i+1].job_queue.front().id, machines[i+1].job_queue.front().ready_time);
              for(int j = 0; j < 3; j++) printf(" %d", machines[i+1].job_queue.front().operation_times[j]);
              printf("]\n");
              printf("(Instance::ACOScheduling)[%d] | kolejka %d:\t[ ", time, i+1);
              for(int j = 0; j < machines[i+1].job_queue.size(); j++) printf("%d ", machines[i+1].job_queue[j].id);
              printf("]\n");
            }

          }

          if(display) {
            printf("(Instance::ACOScheduling)[%d] --------------------------------------------------------------------------------\n", time);
          }

        }
      }


      // For every machine check if busy and operation in progress
      for(int i = 0; i < 3; i++) {
        if((machines[i].state == 1) && (machines[i].job.operation_times[i] > 0)) {

          if(display) {
            printf("(Instance::ACOScheduling)[%d] == Moduł wykonujący zadania\n", time);
            printf("(Instance::ACOScheduling)[%d] --------------------------------------------------------------------------------\n", time);

            printf("(Instance::ACOScheduling)[%d] | Zadanie wykonuje się na maszynie %d:\t\t%d[%d|",
              time, i, machines[i].job.id, machines[i].job.ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", machines[i].job.operation_times[j]);
            printf("]\n");
          }


          // Shrink remaining operation by one
          machines[i].job.operation_times[i]--;

          if(display) {
            printf("(Instance::ACOScheduling)[%d] --------------------------------------------------------------------------------\n", time);
          }
        }

      }


      // check if every queue empty
      bool stop = 1;
      for(int i = 0; i < 3; i++) {
        if(!machines[i].job_queue.empty()) stop = 0;
      }

      // if every queue empty and all machines idle, end scheduling
      if(ants_vector[ant].instance.jobs.empty() && ready_jobs.empty() && machines[0].state == 0 && machines[1].state == 0 && machines[2].state == 0 && stop) {

        //printf("ACO %d ", time);


        if(ant == 0 and gen == 0)
          pheromone_levels.best_cmax = time;

        else if(time < pheromone_levels.best_cmax)
          pheromone_levels.best_cmax = time;

        generations[gen].cmax_list.push_back(time);
        for(int i = 0; i < ants_vector[ant].instance.number_of_jobs; i++) {
          for(int j = 0; j < 3; j++) {
            generations[gen].job_lists[ant][i][j] = ants_vector[ant].job_list[j][i];
            //printf("Ant: %d Job: %d Op: %d = %d\n", ant, i, j, generations[gen].job_lists[ant][i][j]);
          }
        }
        
        break;
      }


      }
    }

    //pheromone_levels.Evaluate(generations[gen], number_of_ants, number_of_jobs, elite, pheromone_reward, pheromone_degradation, better_than_best_multiplier, vapor_level);
//  void Pheromones::Evaluate(Generation gen, int number_of_ants, int number_of_jobs, float pec, float q) {
    pheromone_levels.Evaluate(generations[gen], number_of_ants, number_of_jobs, .987, 0.000013);
    printf("[%d] ACO", gen);
    for(int i = 0; i < generations[gen].cmax_list.size(); i++) {
      printf( " %d", generations[gen].cmax_list[i]);
    }
    printf("\n");
    
    
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

        if(i == 0)
      sort(machines[i].job_queue.begin(), machines[i].job_queue.end(), compareOpTimes0);
        if(i == 1)
      sort(machines[i].job_queue.begin(), machines[i].job_queue.end(), compareOpTimes1);
        if(i == 2)
      sort(machines[i].job_queue.begin(), machines[i].job_queue.end(), compareOpTimes2);

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

      printf("SJF %d\n", time);
      break;
    }


  }

  return 0;

}
