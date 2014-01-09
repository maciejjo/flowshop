#include "Generation.h"

#include <cstdlib>

  Generation::Generation(int number_of_ants, int number_of_jobs) {

    job_lists = (int ***) malloc(number_of_ants * sizeof(int **));
    for(int i = 0; i < number_of_ants; i++) {
      job_lists[i] = (int **) malloc(number_of_jobs * sizeof(int *));
      for(int j = 0; j < number_of_jobs; j++) {
        job_lists[i][j] = (int *) malloc(3 * sizeof(int));
      }
    }

  }

