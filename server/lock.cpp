#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <semaphore.h>
#include <map>
#include <string>

using namespace std;

class Lock {
    map<string, sem_t> semMap ;

public:
    Lock() {
        
        // sem_init(&sem1_, 0, 1); // Initialize sem1 to 1
        // sem_init(&sem2_, 0, 1); // Initialize sem2 to 1
    }

    void acquire(string dbname) {
        sem_wait(&semMap[dbname]) ;
    }

    void release(string dbname) {
        sem_post(&semMap[dbname]) ;
    }

};