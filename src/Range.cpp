#include "Range.h"

Range::Range(int range, int min, int max, int min_ready, int max_ready) {

  this->size= range;
  this->min_duration = min;
  this->max_duration = max;
  this->min_ready_time = min_ready;
  this->max_ready_time = max_ready;
}

