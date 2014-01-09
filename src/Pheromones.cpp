#include "Pheromones.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <vector>

using namespace std;

  Pheromones::Pheromones(int number_of_jobs) {
    for(int i = 0; i < 3; i++) {
      pheromone_levels[i] = (float **) malloc(sizeof(float *) * number_of_jobs + 1);

      for(int j = 0; j < number_of_jobs + 1; j++) {
        pheromone_levels[i][j] = (float *) malloc(sizeof(float) * number_of_jobs);
        for(int k = 0; k < number_of_jobs; k++) {
          pheromone_levels[i][j][k] = 0.0;
        }
      }
    }
  }

  int Pheromones::SpinRoulette(Machine machine, int number_of_jobs, int machine_no, int alpha, int beta) {
    int roulette_sum = 0;
    float current_pheromone, tau, ni;
    vector<int> roulette_vector;
    for(int j = 0; j < machine.job_queue.size(); j++) {

      if(!machine.virgin) {
          tau = pow(pheromone_levels[machine_no][machine.job.id][machine.job_queue[j].id], alpha);
          ni = pow(machine.job_queue[j].operation_times[machine_no], beta);
      }
      else {  
          tau = pow(pheromone_levels[machine_no][number_of_jobs][machine.job_queue[j].id], alpha);
          ni = pow(machine.job_queue[j].operation_times[machine_no], beta);
      }

      current_pheromone = tau + 1 / ni;

      roulette_sum += 1 + (int) (100 * current_pheromone);
      roulette_vector.push_back(roulette_sum);
      printf("[RUL %d ]Dodaję do sumy feromonów [%d] tau = %.3f ni = %.3f cf=%.5f przedzial = %d \n", machine_no, machine.job_queue[j].id, tau, ni, current_pheromone, roulette_vector.back());
    }
    printf("[RUL %d]Suma do ruletki: %d\n", machine_no, roulette_sum);
    
    int roulette_choice = rand() % roulette_sum;
    int k;
    for(k = 0; k < roulette_vector.size(); k++) {
      if(roulette_vector[k] > roulette_choice) {
        printf("[RUL %d]Wybrano przedział %d, %d < %d zad. nr. %d \n", machine_no, roulette_vector[k], 
            roulette_vector[k],  roulette_choice, machine.job_queue[k].id);
        printf("%d ", roulette_vector[k]);

        break;
      }
    }
    printf("\n");
    return k;
  }


  void Pheromones::Evaluate(Generation gen, int number_of_ants, int number_of_jobs, float pec, float q) {

    bool *ant_evaluated = (bool *) malloc(sizeof(bool) *number_of_ants);
    for(int i = 0; i < number_of_ants; i++)
      ant_evaluated[i] = false;

    //float original_ph_reward = ph_reward;

    for(int i = 0; i < number_of_ants; i++) {

      int min_cmax;
      int best_ant;
      bool cmax_set = false;

      // Szukanie najlepszej mrówki
      for(int j = 0; j < number_of_ants; j++) {
      //for(int j = 0; j < number_of_ants; j++) 

        if(!ant_evaluated[j]) {

            if(!cmax_set) {
              printf("[EVAL]Pierwsza nieoceniona mrówka %d, przyjmuję cmax %d\n", j, gen.cmax_list[j]);
              min_cmax = gen.cmax_list[j];
              best_ant = j;
              cmax_set = true;
            }

            printf("[EVAL %d]Sprawdzam czy %d < %d...\n", j, gen.cmax_list[j], min_cmax);
            if(gen.cmax_list[j] < min_cmax) {
              printf("[EVAL]wygrywa mrówka %d: %d < %d\n", j, gen.cmax_list[j], min_cmax);
              min_cmax = gen.cmax_list[j];
              best_ant = j;
            }
        }
      }

      printf("[EVAL]Oceniam najlepszą mrówkę: %d\n", best_ant);
      for(int j = 0; j < number_of_jobs - 1; j++) {
        for(int k = 0; k < 3; k++) {
          float ph_reward = ((1.0 - pec) * pheromone_levels[k][gen.job_lists[best_ant][j][k]][gen.job_lists[best_ant][j+1][k]]) + (q / gen.cmax_list[best_ant]);
          printf("[EVAL OPUS %d] Skąd: %d, dokąd: %d maszyna: %d nagroda: %.3f\n", i, gen.job_lists[best_ant][j][k], gen.job_lists[best_ant][j+1][k], k, ph_reward);
          
//          if(gen.cmax_list[best_ant] < best_cmax)
//            ph_reward *= ph_multiplier;
           
          
          if(j == 0) {
            if(pheromone_levels[k][number_of_jobs][gen.job_lists[best_ant][j+1][k]] <= 1000) 
              pheromone_levels[k][number_of_jobs][gen.job_lists[best_ant][j+1][k]] += ph_reward;
          }
          else
          {
            if(pheromone_levels[k][gen.job_lists[best_ant][j][k]][gen.job_lists[best_ant][j+1][k]] <= 1000) 
              pheromone_levels[k][gen.job_lists[best_ant][j][k]][gen.job_lists[best_ant][j+1][k]] += ph_reward;
          }
          
        }
      }

//      if(gen.cmax_list[i+1] < gen.cmax_list[i])
//        ph_reward -= ph_degradation;

//      printf("[EVAL OPUS] Nagroda: %.3f %.3f ( / %d) \n", ph_reward, original_ph_reward / number_of_jobs, number_of_jobs);

      
      ant_evaluated[best_ant] = true;
      if(ant_evaluated[best_ant]) printf("Oceniono\n");

    }

 /*   
    for(int k = 0; k < 3; k++) {
      for(int i = 0; i < number_of_jobs + 1; i++) {
        for(int j = 0; j < number_of_jobs; j++) {
          if(pheromone_levels[k][i][j] >= 0.0)
           pheromone_levels[k][i][j] -= pheromone_levels[k][i][j] * ph_vapor;
          else
           pheromone_levels[k][i][j] = 0.0;

        }
      }
    }
    */
  
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < number_of_jobs + 1; j++) {
        for(int k = 0; k < number_of_jobs; k++) {
          printf("%.9f ", pheromone_levels[i][j][k]);
        }
        printf(" [TAB] \n");
      }
      printf(" [TAB] \n");
    }


  }

  
