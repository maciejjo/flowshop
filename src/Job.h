#ifndef JOB_H
#define JOB_H

class Job {

  public:
    static int last_id;
    int id;
    int operation_times[3];
    int ready_time;

    Job(int max, int min, int ready_max, int ready_min);
    Job();

};

#endif
