#ifndef RANGE_H
#define RANGE_H

class Range {
  public:
    int size;
    int min_duration;
    int max_duration;
    int min_ready_time;
    int max_ready_time;
    Range(int range, int min, int max, int min_ready, int max_ready);
};


#endif
