#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
  double prevIdle_ = 0;
  double prevTotal_ = 0;
  //double prevActive_ = 0;
};

#endif
