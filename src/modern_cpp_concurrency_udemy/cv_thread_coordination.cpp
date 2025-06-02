//
// Created by coolk on 02-06-2025.
//
// Simulation of a program which performs a download
//
// One thread fetches the data
// Another thread displays a progress bar
// A third thread processes the data when the download is complete.
//
// Implemented using a condition variable to communicate between the treads
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>

using namespace std::literals;

std::string sdata;
bool update_progress = false;
bool completed = false;

std::mutex data_mutex;
std::mutex completed_mutex;

std::condition_variable data_condition;
std::condition_variable completed_condition;

// Data fetching thread
auto fetch_data() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Fetcher thread waiting for data..." << std::endl;
        std::this_thread::sleep_for(2s);

        // Update sdata, then notify the progress bar thread.
        std::unique_lock<std::mutex> lock(data_mutex);
        sdata += "Block" + std::to_string(i+1);
        std::cout << "Fetched sdata: " << sdata << std::endl;
        update_progress = true;
        lock.unlock();
        data_condition.notify_all();
    }

    std::cout << "Fetch sdata has ended" << std::endl;

    // Tell the progress bar thread to exit and wake up the processing thread
    std::lock_guard<std::mutex> lock(completed_mutex);
    completed = true;
    completed_condition.notify_all();
}

// Progress bar thread
auto progress_bar() {
    size_t len = 0;

    while (true) {
        std::cout << "Progress bar thread waiting for data..." << std::endl;

        std::unique_lock<std::mutex> lock(data_mutex);
        data_condition.wait(lock, [] {return update_progress;});

        len = sdata.size();

        update_progress = false;
        lock.unlock();

        std::cout << "Received " << len << " bytes so far" << std::endl;

        std::unique_lock<std::mutex> completed_lock(completed_mutex);

        if (completed_condition.wait_for(completed_lock, 10ms, []{return completed;})) {
            std::cout << "Progress bar thread has ended" << std::endl;
            break;
        }
    }
}

auto process_data() {
    std::this_thread::sleep_for(200ms);
    std::cout << "Processing thread waiting for data..." << std::endl;

    std::unique_lock<std::mutex> lock(data_mutex);

    completed_condition.wait(lock, [] {return completed;});
    lock.unlock();

    std::lock_guard<std::mutex> data_lock(data_mutex);
    std::cout << "Processed sdata: " << sdata << std::endl;
}

auto main()-> int {
    std::thread fetcher {fetch_data};
    std::thread progress {progress_bar};
    std::thread processor {process_data};

    fetcher.join();
    progress.join();
    processor.join();

    return 0;
}