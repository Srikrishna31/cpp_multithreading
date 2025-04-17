//
// Created by coolk on 17-04-2025.
//

#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <utility>
#include <iostream>

std::mutex m;
std::deque<std::packaged_task<void()>> tasks;
std::atomic<bool> stop;

bool gui_shutdown_message_received() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return stop.load();
};

void get_and_process_gui_messages() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void gui_thread() {             // <--- (1)
    while (!gui_shutdown_message_received()) {      // <--- (2)
        get_and_process_gui_messages();     // <--- (3)
        std::packaged_task<void()> task;
        {
            std::lock_guard lock(m);
            if (tasks.empty()) { continue; }    // <--- (4)
            task = std::move(tasks.front());        // <--- (5)
            tasks.pop_front();
        }
        task();         // <--- (6)
    }
}


template <typename Func>
std::future<void> post_task_for_gui_thread(Func f) {
    std::packaged_task<void()> task(f);   // <--- (7)
    std::future<void> future = task.get_future();   // <--- (8)
    std::lock_guard lock(m);
    tasks.emplace_back(std::move(task));    // <--- (9)
    return future;      // <--- (10)
}

/**
 *
 *      Associating a task with a future
 *  std::packaged_task<> ties a future to a function or callable object. When the std::packaged_task<> object is invoked,
 *  it calls the associated function or callable object and makes the future ready, with the return value stored as the
 *  associated data. This can be used as a building block for thread pools or other task management schemes, such as running
 *  each task on its own thread, or running them all sequentially on a particular background thread. If a large operation
 *  can be divided into self-contained sub-tasks, each of these can be wrapped in a std::packaged_task<> instance, and
 *  then that instance passed to the task scheduler or thread pool. This abstracts out the details of the tasks; the
 *  scheduler just deals with std::packaged_task<> instances rather than individual functions.
 *
 *  The template parameter for the std::packaged_task<> class template is a function signature, like void() for a function
 *  taking no parameters with no return value, or int(std::string&, double*) for a function that takes a non-const reference
 *  to a std::string and a pointer to a double and returns an int. When you construct an instance of std::packaged_task,
 *  you must pass in a function or callable object that can accept the specified parameters and that returns a type
 *  convertible to the specified return type. The types don't have to match exactly; you can construct a
 *  std::packaged_task<double(double)> from a function that takes an int and returns a float because the types are implicitly
 *  convertible.
 *
 *  The return type of the specified function signature identifies the type of the std::future<> returned from the get_future()
 *  member function, whereas the argument list of the function signature is used to specify the signature of the packaged
 *  task's function call operator. An example is shown below:
 *
 *  template<>
 *  class packaged_task<std::string(std::vector<char>*, int)>
 *  {
 *  public:
 *      template<typename Callable>
 *      explicit packaged_task(Callable&& f);
 *      std::future<std::string> get_future();
 *      void operator() (std::vector<char>*, int);
 *  };
 *
 *  The std::packaged_task object is thus a callable object, and it can be wrapped in a std::function object, passed to a
 *  std::thread as the thread function, passed to another function that requires a callable object, or even invoked directly.
 *  When the std::packaged_task is invoked as a function object, the arguments supplied to the function call operator are
 *  passed on to the contained function, and the return value is stored as the asynchronous result in the std::future
 *  obtained from get_future(). You can thus wrap a task in a std::packaged_task and retrieve the future before passing the
 *  std::packaged_task object elsewhere to be invoked in due course. When you need the result, you can wait for the future
 *  to become ready.
 *
 *  The following code is very simple: the GUI thread (1) loops until a message has been received telling the GUI to
 *  shut down (2), repeatedly polling for GUI messages to handle (3), such as user clicks, and for tasks on the task queue.
 *  If there are no tasks on the queue (4), it loops again; otherwise it extracts the task from the queue (5), releases
 *  lock on the queue, and then runs the task (6). The future associated with the task will then be made ready when the
 *  task completes.
 *
 *  Posting a task on the queue is equally simple: a new packaged task is created from the supplied function (7), the
 *  future is obtained from that task (8) by calling the get_future() member function, and the task is put on the list (9)
 *  before the future is returned to the caller (10). The code that posted the message to the GUI thread can then wait for
 *  the future if it needs to know the task has been completed, or it can discard the future if it doesn't need to know.
 */
int main() {
    std::thread gui_bg_thread(gui_thread);

    auto f1 = post_task_for_gui_thread([]() {
        std::cout << "Hello from task 1" << std::endl;
    });

    auto f2 = post_task_for_gui_thread([]() {
    std::cout << "Hello from task 2" << std::endl;
    });

    auto f3 = post_task_for_gui_thread([]() {
    std::cout << "Hello from task 3" << std::endl;
    });

    f1.get();
    f2.get();
    f3.get();

    stop.store(true);
    gui_bg_thread.join();
}