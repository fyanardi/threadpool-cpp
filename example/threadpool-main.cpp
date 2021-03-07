#include "threadpool.h"

#include <random>

#include "threadpool.h"
#include "debug.h"

int main() {
    Threadpool threadpool(8);
    std::mutex *mutex = new std::mutex;
    std::condition_variable *cv = new std::condition_variable;
    int num_jobs = 100;
    int count = num_jobs;
    for (int i = 0; i < num_jobs; i++) {
        debug("-> Submitting job #%d\n", i);
        int j = i;
        std::default_random_engine generator(time(0));
        threadpool.execute([j, &generator, &count, cv, mutex]() {
            std::uniform_int_distribution<int> distribution(1, 10);
            int rand = distribution(generator);
            debug("+Executing (%d)\n", j);
            std::this_thread::sleep_for(std::chrono::milliseconds(rand * 1000));
            debug("-Executed (%d)\n", j);
            std::unique_lock<std::mutex> lock(*mutex);
            count = count - 1;
            
            cv->notify_all();
        });
        debug("-> Job #%d submitted\n", i);
    }
    std::unique_lock<std::mutex> lock(*mutex);
    while (count != 0) {
        debug("*** Count: %d\n", count);
        cv->wait(lock); 
    }

    debug("Shutting down threadpool ...\n");
    threadpool.shutdown();
    debug("Threadpool shutdown.\n");

    return 0;
}

