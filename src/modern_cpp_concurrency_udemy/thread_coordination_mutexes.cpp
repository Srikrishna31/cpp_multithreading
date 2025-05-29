//
// Created by coolk on 29-05-2025.
//
// Simulation of a program which performs a download
//
// One thread fetches the data
// Another thread displays a progress bar
// A third thread processes the data when the download is complete.
//
// Implemented using bools to communicate between the threads
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>

using namespace std::literals;

// shared variable for the data being fetched.
std::string sdata;

// Flags for thread communication
bool update_progress = false;
bool completed = false;

std::mutex data_mutex;
std::mutex completed_mutex;

// Data fetching thread
auto fetch_data() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Fetcher thread waiting for the data..." << std::endl;
        std::this_thread::sleep_for(2s);

        // Update sdata, then notify the progress bar thread
        std::lock_guard<std::mutex> data_lk(data_mutex);
        sdata += "Block" + std::to_string(i+1);
        std::cout << "sdata: " << sdata << std::endl;
        update_progress = true;
    }

    std::cout << "Fetch sdata has ended" << std::endl;

    // Tell the progress bar thread to exit and wake up the processing thread
    std::lock_guard<std::mutex> completed_lk(completed_mutex);
    completed = true;
}

// Progress bar thread
auto progress_bar() {
    size_t len = 0;

    while (true) {
        std::cout << "Progress bar thread waiting for data..." << std::endl;

        // Wait until there is some new data to display
        std::unique_lock<std::mutex> data_lk(data_mutex);
        while (!update_progress) {
            data_lk.unlock();
            std::this_thread::yield();
            data_lk.lock();
        }

        // Wake up and use the new value
        len = sdata.size();

        // Set the flag back to false;
        update_progress = false;
        data_lk.unlock();

        std::cout << "Received data: " << len << " bytes so far" << std::endl;

        // Terminate when the download has finished
        std::lock_guard<std::mutex> completed_lk(completed_mutex);
        if (completed) {
            std::cout << "Progress bar thread has ended" << std::endl;
            break;
        }
    }
}

auto process_data() {
    std::cout << "Processing thread waiting for data..." << std::endl;

    // Wait until download is complete
    std::unique_lock<std::mutex> completed_lk(completed_mutex);

    while (!completed) {
        completed_lk.unlock();
        std::this_thread::yield();
        completed_lk.lock();
    }

    completed_lk.unlock();

    std::lock_guard<std::mutex> data_lk(data_mutex);
    std::cout << "Processing data: " << sdata << std::endl;

    // Process the data
}

/**
 *
 * Implementation with mutex
 *  * This is not ideal
 *      . Too many loops
 *      . Too much explicit locking and unlocking
 *      . How do we choose the sleep duration? (Ofcourse we used std::this_thread::yield here.
 *  * Better solution
 *      . Thread A indicates that it is waiting for something
 *      . Thread B does the "something"
 *      . Thread A is woken up and resumes.
 */
int main() {
    // Start the threads
    std::thread fetcher(fetch_data);
    std::thread progress(progress_bar);
    std::thread processor(process_data);

    fetcher.join();
    progress.join();
    processor.join();
}