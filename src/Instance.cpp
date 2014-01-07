#include "Instance.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

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

  int number_of_ants = 100;
  int number_of_gens = 100;
  float vapor_level = 0.5;
  float **pheromone_levels[3];
  int best_cmax;

  for(int i = 0; i < 3; i++) {

    pheromone_levels[i] = (float **) malloc(sizeof(float *) * number_of_jobs + 1);

    for(int j = 0; j < number_of_jobs + 1; j++) {
      pheromone_levels[i][j] = (float *) malloc(sizeof(float) * number_of_jobs);
      for(int k = 0; k < number_of_jobs; k++) {
        pheromone_levels[i][j][k] = 0.0;
      }
    }
  }

  vector<Job> old_jobs = this->jobs;


  
  for(int gen = 0; gen < number_of_gens; gen++) {


    
    vector<int> cmax_list;
    
    
    int ***job_lists = (int ***) malloc(number_of_ants * sizeof(int **));
    for(int i = 0; i < number_of_ants; i++) {
      job_lists[i] = (int **) malloc(number_of_jobs * sizeof(int *));
      for(int j = 0; j < number_of_jobs; j++) {
        job_lists[i][j] = (int *) malloc(3 * sizeof(int));
      }
    }
    
    for(int ant = 0; ant < number_of_ants; ant++) {

      this->jobs = old_jobs;
      deque<int> job_list[3];

      SortJobs();

      for(int time = 0;; time++) {

      if(guaranteed_halt) {
        int chance = rand() % (mean_operation_time * guaranteed_halt);
        if((halts < number_of_halts) && (!chance || force_halt)) {

          was_halt = true;
          force_halt = false;
          halt_start = true;
          halts++;
          halt_left = (rand() % max_halt_time) + 1;

        }
      }

      vector <Job> ready_jobs = GetReadyJobs(time);
      //random_shuffle(ready_jobs.begin(), ready_jobs.end());

      for(int i = ready_jobs.size() - 1; i >= 0; i--) {
        machines[0].job_queue.push_back(ready_jobs[i]);
        ready_jobs.pop_back();
              printf("(Instance::AcoScheduling)[%d] Zadanie dodano do pierwszej kolejki:\t\t%d[%d|",
                time, machines[0].job_queue.back().id, machines[0].job_queue.back().ready_time);
              for(int j = 0; j < 3; j++) printf(" %d", machines[0].job_queue.back().operation_times[j]);
              printf("]\n");
      }


      for(int i = 0; i < 3; i++) {
        if((machines[i].state == 0) && (machines[i].job_queue.size() > 0 )) {


          machines[i].state = 1;
          
          //random_shuffle(machines[i].job_queue.begin(), machines[i].job_queue.end());


          if(!machines[i].virgin) {
            int current_job = machines[i].job.id;

            int roulette_sum = 0;
            vector<int> roulette_vector;
            for(int j = 0; j < machines[i].job_queue.size(); j++) {
              float current_pheromone = 1000 * pheromone_levels[i][current_job][machines[i].job_queue[j].id];
      //pheromone_levels[maszyna][skąd][dokąd] = 0.0;
              printf("[RUL %d ]Dodaję do sumy feromonów [%d] %.3f\n", i, machines[i].job_queue[j].id, 10 * pheromone_levels[i][current_job][machines[i].job_queue[j].id]);
              roulette_sum += 10 + (int) current_pheromone;
              roulette_vector.push_back(roulette_sum);
            }
            printf("[RUL %d]Suma do ruletki: %d\n", i, roulette_sum);
            
            int roulette_choice = rand() % roulette_sum;
            for(int k = 0; k < roulette_vector.size(); k++) {
              if(roulette_vector[k] > roulette_choice) {
                printf("[RUL %d]Wybrano przedział %d, %d < %d zad. nr. %d \n", i, roulette_vector[k], 
                    roulette_vector[k],  roulette_choice, machines[i].job_queue[k].id);
                printf("%d ", roulette_vector[k]);
//                job_list[i].push_back(machines[i].job_queue[k].id);

                Job tmp_job = machines[i].job_queue.front();
                machines[i].job_queue.front() = machines[i].job_queue[k];
                machines[i].job_queue[k] = tmp_job;
                break;
              }
            }
            printf("\n");
          }
          else {

            //int current_job = machines[i].job.id;

            int roulette_sum = 0;
            vector<int> roulette_vector;
            for(int j = 0; j < machines[i].job_queue.size(); j++) {
              float current_pheromone = 100 * pheromone_levels[i][number_of_jobs][machines[i].job_queue[j].id];
      //pheromone_levels[maszyna][skąd][dokąd] = 0.0;
              printf("[RUL %d ]Dodaję do sumy feromonów [%d] %.3f\n", i, machines[i].job_queue[j].id, 100 * pheromone_levels[i][number_of_jobs][machines[i].job_queue[j].id]);
              roulette_sum += 100 + (int) current_pheromone;
              roulette_vector.push_back(roulette_sum);
            }
            printf("[RUL %d]Suma do ruletki: %d\n", i, roulette_sum);
            
            int roulette_choice = rand() % roulette_sum;
            for(int k = 0; k < roulette_vector.size(); k++) {
              if(roulette_vector[k] > roulette_choice) {
                printf("[RUL %d]Wybrano przedział %d, %d < %d zad. nr. %d \n", i, roulette_vector[k], 
                    roulette_vector[k],  roulette_choice, machines[i].job_queue[k].id);
                printf("%d ", roulette_vector[k]);
//                job_list[i].push_back(machines[i].job_queue[k].id);

                Job tmp_job = machines[i].job_queue.front();
                machines[i].job_queue.front() = machines[i].job_queue[k];
                machines[i].job_queue[k] = tmp_job;
                break;
              }
            }
            printf("\n");
          }

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
          job_list[i].push_back(machines[i].job_queue.front().id);

          machines[i].job_queue.pop_front();
          if(machines[i].virgin)
            machines[i].virgin = false;

            printf("[JOBLIST %d]", i);
            for(int j = 0; j < job_list[i].size(); j++)
              printf(" %d", job_list[i][j]);
            printf("\n");

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
            printf("(Instance::ACOScheduling)[%d] == Moduł przekładający \
              zadania na kolejki\n", time);
            printf("(Instance::ACOScheduling)[%d] ----------\n", time);

            printf("(Instance::ACOScheduling)[%d] | Zadanie zakończone na \
              maszynie %d:\t\t%d[%d|",
              time, i, machines[i].job.id, machines[i].job.ready_time);
            for(int j = 0; j < 3; j++) printf(" %d", 
                machines[i].job.operation_times[j]);
            printf("]\n");
          }

          machines[i].state = 0;
          if(i < 2) {
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
            printf("(Instance::ACOScheduling)[%d] --------------------------------------------------------------------------------\n", time);
          }
        }

      }
        if(halt_left) {
          if(display)
            printf("(Instance::ACOScheduling)[%d] Na maszynie 0 trwa przestój! Pozostało %d...\n", time, halt_left);
          halt_left--;
        }

  //    if(jobs.empty() && ready_jobs.empty() && machines[0].state == 0 && machines[1].state == 0 && machines[2].state == 0) break;
      int stop = 1;
      for(int i = 0; i < 3; i++) {
        if(!machines[i].job_queue.empty()) stop = 0;
      }

      if(jobs.empty() && ready_jobs.empty() && machines[0].state == 0 && machines[1].state == 0 && machines[2].state == 0 && stop) {

        printf("ACO %d\n", time);
        
        
        if(ant == 0 and gen == 0)
          best_cmax = time;
        else if(time < best_cmax)
          best_cmax = time;

        cmax_list.push_back(time);
        for(int i = 0; i < number_of_jobs; i++) {
          for(int j = 0; j < 3; j++) {
            job_lists[ant][i][j] = job_list[j][i];
            printf("Ant: %d Job: %d Op: %d = %d\n", ant, i, j, job_lists[ant][i][j]);
          }
        }
        
        break;
      }


      }

    }

    for(int i = 0; i < number_of_ants; i++) {
      printf("[%d STATS] Mrówka nr %d, cmax %d, zadania:", gen, i, cmax_list[i]);
      for(int j = 0; j < number_of_jobs; j++)
        for(int k = 0; k < 3; k++)
          printf(" %d", job_lists[i][j][k]);
      printf("\n");
    }


    bool *ant_evaluated = (bool *) malloc(sizeof(bool) *number_of_ants);
    for(int i = 0; i < number_of_ants; i++)
      ant_evaluated[i] = false;

    float pheromone_reward = 99.99;
    float original_pheromone_reward = 99.99;

    printf("[EVAL]GEN %d\n", gen);
    for(int i = 0; i < number_of_ants; i++) {

      int min_cmax;
      int best_ant;
      int elite = 1;
      bool cmax_set = false;

      // Szukanie najlepszej mrówki
      for(int j = 0; j < elite; j++) {
      //for(int j = 0; j < number_of_ants; j++) 

        if(!ant_evaluated[j]) {

            if(!cmax_set) {
              printf("[EVAL]Pierwsza nieoceniona mrówka %d, przyjmuję cmax %d\n", j, cmax_list[j]);
              min_cmax = cmax_list[j];
              best_ant = j;
              cmax_set = true;
            }

            printf("[EVAL %d]Sprawdzam czy %d < %d...\n", j, cmax_list[j], min_cmax);
            if(cmax_list[j] < min_cmax) {
              printf("[EVAL]wygrywa mrówka %d: %d < %d\n", j, cmax_list[j], min_cmax);
              min_cmax = cmax_list[j];
              best_ant = j;
            }
        }
      }

      printf("[EVAL]Oceniam najlepszą mrówkę: %d\n", best_ant);
      //pheromone_levels[maszyna][skąd][dokąd] = 0.0;
      //job_lists[nr_mrówki][nr_zadania][nr_maszyny]
     //float punish = 0.9;
     //int elite = 10;
      for(int j = 0; j < number_of_jobs - 1; j++) {
      //for(int j = 0; j <elite; j++)
        for(int k = 0; k < 3; k++) {
          printf("[EVAL OPUS %d] Skąd: %d, dokąd: %d maszyna: %d nagroda: %.3f\n", i, job_lists[best_ant][j][k], job_lists[best_ant][j+1][k], k, pheromone_reward);
          
          if(cmax_list[best_ant] < best_cmax)
            pheromone_reward *= 200;
          
          pheromone_levels[k][job_lists[best_ant][j][k]][job_lists[best_ant][j+1][k]] += pheromone_reward * (1.0 / (float) cmax_list[best_ant]);
          
          if(j == 0)
            pheromone_levels[k][number_of_jobs][job_lists[best_ant][j+1][k]] += pheromone_reward * (1.0 / (float) cmax_list[best_ant]);
          
        }
      }

      //if(cmax_list[i+1] < cmax_list[i])
      //  pheromone_reward -= original_pheromone_reward / number_of_ants;

      printf("[EVAL OPUS] Nagroda: %.3f %.3f ( / %d) \n", pheromone_reward, original_pheromone_reward / number_of_jobs, number_of_jobs);

      
      ant_evaluated[best_ant] = true;
      if(ant_evaluated[best_ant]) printf("Oceniono\n");

    }

    
    for(int k = 0; k < 3; k++) {
      for(int i = 0; i < number_of_jobs + 1; i++) {
        for(int j = 0; j < number_of_jobs; j++) {
          if(pheromone_levels[k][i][j] >= 0.0)
           pheromone_levels[k][i][j] -= pheromone_levels[k][i][j] * vapor_level;
          else
           pheromone_levels[k][i][j] = 0.0;

        }
      }
    }
  

    printf("[EVAL][MATRIX] ");
    for(int i = 0; i < number_of_ants; i++) {
      if(ant_evaluated[i]) printf("1 ", i);
      else printf("0 ");
    }
    printf("\n");
        
        for(int i = 0; i < 3; i++) {
          for(int j = 0; j < number_of_jobs + 1; j++) {
            for(int k = 0; k < number_of_jobs; k++) {
              printf("%.3f ", pheromone_levels[i][j][k]);
            }
            printf(" [TAB] \n");
          }
          printf(" [TAB] \n");
        }

    
    
  }
  printf("ACO_BEST %d\n", best_cmax);
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
