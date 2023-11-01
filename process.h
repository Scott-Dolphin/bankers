#ifndef __process_h__
#define __process_h__

#include <iostream>
#include <iomanip>
#include "semaphore_layer.h"
#include "bank.h"
#include "vector_.h"
#include "time.h"      // need to make rand() mostly random

class Bank;    // forward declare pointer to Bank


class Process {
private:
  int pid_;
  int resources_;
  Bank* bank_;
  vector_<int> wants_;
  vector_<int> alloc_;
  vector_<int> needs_;


public:
  Process() : Process(0, 0, nullptr) { }       // must exist to add to vector_ (which is added to std::vector)
  Process(int pid, int resources, Bank* bank) 
  : pid_(pid), resources_(resources), bank_(bank) { 
    wants_.resize(resources_);  // DO NOT REMOVE these three lines
    alloc_.resize(resources_);
    needs_.resize(resources_);

    std::cin >> wants_;        // std::cin >> depends on above three lines
    std::cin >> alloc_;
    needs_ = wants_ - alloc_;

    srand(time(NULL));         // make rand() mostly random

  }

  void set_bank(Bank* bank) { bank_ = bank; }
  Bank* bank() { return bank_; }

  int pid() const { return pid_; }            // accessor
  vector_<int>& alloc() { return alloc_; }    // rw accessors
  vector_<int>& wants() { return wants_; }
  vector_<int>& needs() { return needs_; }

  void request_granted(const vector_<int>& request) { 
    alloc_ += request;
    needs_ -= request;
  }

  vector_<int> make_request() {       // e.g., if needs_ == [ 1 2 3], .e.g., [ 1 1 0 ] for Request
    vector_<int> request(resources_); //                                     [ 1 2 4 ] not ok
    if (needs_.is_zero()) { return needs_; }

    while (request.is_zero()) { 
      for (int i = 0; i < resources_; ++i) {
        // todo  random number up to the size of needs_[i]
        int num = (rand() % (needs_[i] + 1)); 
        request[i] = num;
      }
   }

    return request;
  }

  bool has_all_resources() { 
    for (auto i = 0; i < resources_; ++i) {    // simplify using std::algorithms
      if (wants_[i] != alloc_[i]) { return false; }
    }
    return true;
  }

  void release_all_resources() { 
    for (auto i = 0; i < resources_; ++i) {   // simplify using std::fill
      wants_[i] = alloc_[i] = needs_[i] = 0;
    }
  }
  friend std::ostream& operator<<(std::ostream& os, const Process& p) { 
    os << "P#" << std::setw(2) << p.pid_ << "    " << p.wants_ << "    " << p.alloc_ << "    " << p.needs_;
    return os;
  }
};

#endif
