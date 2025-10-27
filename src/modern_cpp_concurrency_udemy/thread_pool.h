#pragma once

/**
 *    * Thread Creation Overhead
 *        - Creating a thread requires a lot of work
               *  Create an execution stack for the thread
               * Call a system API
               * The operating system creates internal data to manage the thread.
               * The scheduler executes the thread.
               * A context switch occurs to run the thread.
          - Creating a new thread can take 10,000 times as long as calling a function directly.

      * Thread Pool Motivation
          - We want to make full use of all our processor cores
          - Every core should be running one of our threads
              * Except perhaps for main()
              * And the operating system.
          - Difficult to achieve with std::sync()
              * Need to keep track of number of threads started.
    *  Thread pool structure
          - Container of C++ thread objects.
              * Has a fixed size.
              * Usually matched to the number of cores on the machine.
              * Found by calling std::thread::hardware_concurrency()
              * Subtract 2 from the result (main thread + OS)
          - A queue of tasks
              * A thread takes a task off the queue.
              * It performs the task.
              * Then it takes the next task from the queue.
          - Tasks represented as callable objects.
    * Advantages of Thread Pool
          - No scaling concerns: It will automatically use all the available resources (cores).
          - Makes efficient use of resources.
              * Threads are always busy.
              * Provided there is work for them to do.
          - Works best with short, simple tasks where
              * The time taken to create a thread causes a significant delay.
              * The task does not block.
    * Disadvantages of Thread Pool
         - Requires a concurrent queue or similar, which is not directly supported in C++.
         - Overhead: Must add and remove task functions in a thread-safe way.
    *
 */

 #include "concurrent_queue_cv.h"
 #include <thread>
 #include <functional>

 // All the task functions will have this type.
 using Func = std::function<void()>;

 class thread_pool {
     // Queue of task functions
     concurrent_queue_cv<Func> work_queue;

     // Vector of thread objects which make up the pool
     std::vector<std::thread> threads;

     // Entry point function for the threads
     auto worker() -> void;

     bool exit_requested = false;

public:
    thread_pool();
    ~thread_pool();

    // Add a task to the queue.
    auto submit(Func func) -> void;

    // Exit the thread pool
    auto exit(bool exit) -> void;

 };
