#ifndef THREADPOOL_H
#define THREADPOOL_H

#if !defined(__cplusplus) || (__cplusplus < 201103L)
#error The threadpool library requires a compiler supporting C++11.
#endif

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "debug.h"

/**
 * Threadpool class implementation in C++11
 *
 * @author Fredy Yanardi
 */
class Threadpool {
public:
    Threadpool(int maxThreads) :
        maxThreads(maxThreads),
        running(true)
    {
        dispatcher = std::thread([this]() {
            while (this->running) {
                std::function<void()> f;
                {
                    debug("Checking job q (size: %d)\n", this->jobQ.size());
                    std::unique_lock<std::mutex> lock1(this->mutexJobQ);
                    while (this->jobQ.empty()) {
                        this->cvJobQ.wait(lock1);
                        if (!this->running) {
                            break;
                        }
                    }

                    if (!this->jobQ.empty()) {
                        f = this->jobQ.front();
                        this->jobQ.erase(this->jobQ.begin());
                        debug("Job retrieved\n");
                    }
                    else {
                        break;
                    }
                }
                {
                    debug("Checking worker idle q (size: %d)\n", this->workersQ.size());
                    std::unique_lock<std::mutex> lock2(this->mutexIdleQ);
                    while (this->workersQ.empty()) {
                        if (this->workers.size() < this->maxThreads) {
                            this->newWorker();
                            break;
                        }
                        this->cvIdleQ.wait(lock2); 
                    }
                    this->workersQ.front()->submit(f);
                    debug("Job submitted to worker #%d\n", this->workersQ.front()->id());
                    this->workersQ.erase(this->workersQ.begin());
                }
            }
            debug("Dispatcher thread terminated\n");
        });
    }

    ~Threadpool()
    {
        for (auto worker : workers) {
            delete worker;
        }
    }

    void execute(const std::function<void()> &f)
    {
        std::unique_lock<std::mutex> lock(mutexJobQ);
        jobQ.push_back(f);
        cvJobQ.notify_all();
        debug("Job submitted (job queue size: %d)\n", jobQ.size()); 
    }

    void shutdown()
    {
        running = false;
        cvJobQ.notify_all();

        for (auto worker : workers) {
            worker->stop();
        }

        dispatcher.join();
        for (auto &thread : threads) {
            thread.join();
        }

        debug("Shutdown completed\n");
    }

private:
    class Worker {
    public:
        Worker(int id, const std::function<void()> &callback) :
            workerId(id),
            callback(callback),
            running(true),
            f(NULL) {}

        ~Worker() {}

        void operator()()
        {
            while (running.load()) {
                std::unique_lock<std::mutex> lock(mutex);
                while (f == NULL) {
                    cv.wait(lock);
                    if (!running.load()) {
                        break;
                    }
                }
                if (f != NULL) {
                    (*f)();
                    delete f;
                    f = NULL;
                    callback();
                }
            }
            debug("Thread (%d) terminated\n", workerId);
        }

        void submit(const std::function<void()> &f1)
        {
            std::unique_lock<std::mutex> lock(mutex);
            f = new std::function<void()>(f1);
            debug("Job submitted to thread (%d)\n", workerId);
            cv.notify_all();
        }

        void stop()
        {
            running = false;
            cv.notify_all();
        }

        int id()
        {
            return workerId;
        }

    private:
        int workerId;
        std::function<void()> callback;
        std::atomic<bool> running;
        std::function<void()> *f;
        std::mutex mutex;
        std::condition_variable cv;
    };

    void newWorker()
    {
        int id = workers.size();
        Worker *worker = new Worker(id, [this, id]() {
            debug("Worker (%d) is back to idle pool\n", id);
            std::unique_lock<std::mutex> lock(this->mutexIdleQ);
            this->workersQ.push_back(workers[id]);
            this->cvIdleQ.notify_all();
        });
        workers.push_back(worker);
        workersQ.push_back(worker);
        threads.push_back(std::thread(std::ref(*worker)));
    }

    unsigned int maxThreads;
    std::atomic<bool> running;
    std::thread dispatcher;
    std::vector<std::thread> threads;
    std::vector<Worker *> workers;
    std::vector<Worker *> workersQ;
    std::vector<std::function<void()>> jobQ;
    std::mutex mutexJobQ;
    std::mutex mutexIdleQ;
    std::condition_variable cvJobQ;
    std::condition_variable cvIdleQ;
};

#endif // THREADPOOL_H

