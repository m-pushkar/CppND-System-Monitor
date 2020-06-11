#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor() { cpu_ = 0; }
  Processor(std::vector<long int> &x) {
    long usertime = x[0];
    long nice = x[1];
    long system = x[2];
    long idle = x[3];
    long iowait = x[4];
    long irq = x[5];
    long softirq = x[6];
    long steal = x[7];
    long guest = x[8];
    long guest_nice = x[9];

    // https://stackoverflow.com/a/23376195 - Formula found in this thread
    usertime = usertime - guest;
    nice = nice - guest_nice;
    idle = idle + iowait;
    long systemalltime = system + irq + softirq;
    long virtualTime = guest + guest_nice;
    long total = usertime + nice + systemalltime + idle + steal + virtualTime;

    cpu_ = ((float)(total - idle)) / ((float)idle);
  }

  float Utilization();

 private:
  float cpu_;
};

#endif
