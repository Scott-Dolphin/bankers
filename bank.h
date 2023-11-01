#ifndef __bank_h__
#define __bank_h__

#include <iostream>
#include "process.h"
#include "vector_.h"
#include "semaphore_layer.h"

class Bank {
private:
  int n_proc_;
  int n_res_;
  vector_<int> resources_;  
  vector_<Process*> processes_;

public:
  Bank() : Bank(0, 0) { }
  Bank(int n_proc, int n_res) 
  : n_proc_(n_proc), n_res_(n_res) { }

  ~Bank() { 
    for (int i = 0; i < n_proc_; ++i) { 
      delete processes_[i]; 
    }
  }

  vector_<int>& resources() { return resources_; }

  Process* process(int i) { 
    if (i > n_proc_) { 
      std::cout << "Invalid index: " << i << " out of " << n_proc_ << "\n";  
      throw new std::out_of_range("out of range\n"); 
    }

    return processes_[i]; 
  }

  bool grant_request(int pid, const vector_<int>& request) { 
    Process* p = processes_[pid];
    std::cout << "P #" << pid << "  request: [" << request << "] is: ";

    if (p->wants() < request + p->alloc()) { std::cout << "  rejected\n";  return false; }
    if (request.is_zero()) { std::cout << "  rejected\n";  return false; }

    bool result = is_safe(pid, request);

    if (result) { 
      resources_ -= request;
      p->request_granted(request);
    }
    std::cout << (result ? "  GRANTED" : "  rejected") << "\n";
    return result;
  }

  bool is_safe(int pid, const vector_<int>& request) { 
    return true;        // TODO -- only grant safe requests... 
  }

  void add_resources() { 
    resources_.resize(n_res_);
    std::cin >> resources_;
  }

  void add_processes() { 
    for (int i = 0; i < n_proc_; ++i) {          // create resources and add them to bank
      Process* p = new Process(i, n_res_, this);
      processes_.push_back(p);
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const Bank& b) { 
    os << "+---------------------------------------------------------\n";
    os << "|   Bank avail: [" << b.resources_ << "]\n";
    os << "+---------------------------------------------------------\n";
    for (int i = 0; i < b.n_proc_; ++i) { 
      os << "| " << *b.processes_[i] << "\n"; 
      if (i % 5 == 4) { os << "|\n"; }
    }
    os << "+---------------------------------------------------------\n";
    return os << "\n"; 
  }
};

#endif
