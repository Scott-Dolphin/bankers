#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <pthread.h>

#include "semaphore_layer.h"
#include "vector_.h"
#include "bank.h"
#include "process.h"

#define  N_PROCESSES  20

semaphore_t mutex;


std::string yes_or_no(const std::string& msg, bool condition) { 
  std::string result = msg;
  return result + std::string((condition ? "YES" : "no")); 
}

void* run_simulation(void* x) { 
  Process* p = (Process*) x;
  Bank* bank = p->bank();
  int pid = p->pid();

  semaphore_wait(&mutex);
  std::cout << ".......... .......... .......... starting run simulation... for process #" << pid << "\n\n";
  semaphore_signal(&mutex);
  
  vector_<int> request;
  bool finished = false;
  int count = 20;

  while (!finished && count-- > 0) { 
    semaphore_wait(&mutex);
    request = p->make_request();
    bool request_granted = bank->grant_request(pid, request);
    if (request_granted) { 
      if (p->has_all_resources()) { 
        bank->resources() += p->alloc(); 
        p->release_all_resources();
        std::cout << "    Process " << pid << ": all resources acquired, releasing them and shutting down...\n";
        finished = true;
      }
      std::cout << *bank << "\n";
    }
    semaphore_signal(&mutex);
  }
  pthread_exit(NULL);
}


int main(int argc, const char* argv[]) { 
  int fd = open("tst1.txt", O_RDONLY);   // needed for debugging using vsCoce
  dup2(fd, fileno(stdin));               //    don't forget to clean up at bottom

  pthread_attr_t attr;
  mutex = semaphore_create(&mutex, 1);       // sem_init(&mutex, 0, 1);
  
  int n_proc = 0, n_res = 0;
  std::cin >> n_proc >> n_res;
  std::cout << n_proc << " processes  and  " << n_res << " resources\n";

  pthread_t threads[N_PROCESSES];

  Bank* bank = new Bank(n_proc, n_res);   // create bank
  bank->add_resources();       // add resources to bank by reading std::cin
  bank->add_processes();

  std::cout << *bank << "\n";

  pthread_attr_init(&attr);
  for (int i = 0; i < n_proc; ++i) { 
    pthread_create(&threads[i], &attr, run_simulation, (void*)(long)bank->process(i));
  }

  for (int i = 0; i < n_proc; ++i) { 
    pthread_join(threads[i], NULL);
  }
  delete bank;   // clean up memory 
  semaphore_destroy(&mutex);

  close(fd);                          // needed for debugging with vsCode
  freopen("/dev/tty/", "r", stdin);

  return 0; 
}

