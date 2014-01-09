#ifndef GENERATION_H
#define GENERATION_H

#include <vector>

using namespace std;

class Generation {
  public:

    Generation(int number_of_ants, int number_of_jobs);

    int ***job_lists;
    vector<int> cmax_list;

};

#endif
