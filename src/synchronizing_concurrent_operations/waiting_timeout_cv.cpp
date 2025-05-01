//
// Created by coolk on 28-04-2025.
//
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <future>
#include <iostream>

std::condition_variable cv;
bool done = false;
std::mutex m;

/**
 *
 * Waiting with a time limit
 *
 * There are two sorts of timeouts you can specify: a duration-based timeout, where you wait for a specific amount of time,
 * or an absolute timeout, where you wait until a specific point in time. Most of the waiting functions provide variants
 * that handle both forms of timeouts. The variants that handle the duration-based timeouts have a _for suffix, and those
 * that handle the absolute timeouts have a _until suffix.
 *
 * Clocks
 * As far as the C++ standard library is concerned, a clock is a source of time information. In particular, a clock is a
 * class that provides four distinct pieces of information:
 *
 *  * The time now.
 *  * The type of the value used to represent the times obtained from the clock.
 *  * The tick period of the clock
 *  * Whether the clock ticks at a uniform rate and is thus considered to be a steady clock.
 *
 * The current time of a clock can be obtained by calling the static member function now() for that clock class; for example,
 * std::chrono::system_clock::now() will return the current time of the system clock. The type of the time points for a
 * particular clock is specified by the time_point member typedef, so the return type of some_clock::now() is some_clock::time_point.
 *
 * The tick period of the clock is specified as a fractional number of seconds, which is given by the period member typedef
 * of the clock. If the period of the clock can't be known until runtime, or it may vary during a given run of the
 * application, the period may be specified as the average tick period, smallest possible tick period, or some other value
 * that the library writer deems appropriate. There's no guarantee that the observed tick period in a given run of the
 * program matches the specified period for that clock.
 *
 * If a clock ticks at a uniform rate (whether that rate matches the period) and can't be adjusted, the clock is
 * said to be a steady clock. The is_steady static data member of the clock class is true if the clock is steady and false
 * otherwise. Typically, std::chrono::system_clock will not be steady, because the clock can be adjusted, even if such
 * adjustment is done automatically to take account of local clock drift. Such an adjustment may cause a call to now() to
 * return a value earlier than that returned by a prior call to now(), which is in violation of the requirement for a
 * uniform tick rate.
 *
 * Durations
 *
 * Durations are handled by the std::chrono::duration<> class template. The first template parameter is the type of
 * the representation (such as int, long or double), and the second is a fraction specifying how many seconds each unit
 * of the duration represents.
 * The Standard Library provides a set of predefined typedefs in the std::chrono namespace for various durations: nanoseconds,
 * microseconds, milliseconds, seconds, minutes, and hours. They all use a significantly larger integral type for the
 * representation chosen such that you can represent a duration of over 500 years in the appropriate units.
 * Conversion between durations is implicit where it does not require truncation of the value (so converting hours to seconds
 * is OK, but converting seconds to hours is not). Explicit conversions can be done with std::chrono::duration_cast<>;
 *
 * The wait functions all return a status to indicate whether the wait timed out or the waited-for event occurred. When
 * waiting for a future, the function returns std::future_status::timeout if the wait times out, std::future_status::ready
 * if the future is ready, or std::future_status::deferred if the future's task is deferred. The time for a duration-based
 * wait is measured using a steady clock internal to the library, so a specified wait time remains exact, even if the system
 * clock was adjusted (forward or backward) during the wait. Of course, the vagaries of system scheduling and the varying
 * precisions of OS clocks means that the actual time between the thread issuing the call and returning from it may be
 * much longer than the specified wait time.
 *
 * Time points
 *
 * The time point for a clock is represented by an instance of the std::chrono::time_point<> class template, which specifies
 * which clock it refers to as the first template parameter and the units of measurement (a specialization of
 * std::chrono::duration<>) as the second template parameter. The value of a time point is the length of time(in multiples
 * of the specified duration) since a specific point in time called the epoch of the clock. The epoch of a clock is a basic
 * property but not something that's directly available to query or specified by the C++ standard. Clocks may share an
 * epoch or have independent epochs. If two clocks share an epoch, the time_point typedef in one class may specify the
 * other as the clock type associated with the time_point. Although you can't find out when the epoch is, you can get the
 * time_since_epoch() for a given time_point. This member function returns a duration value specifying the length of time
 * since the clock epoch to that particular time point.
 * You can subtract one timepoint from another that shares the same clock. The result is a duration specifying the length
 * of time between the two time points. This is useful for timing blocks of code.
 * When you pass the time point to a wait function that takes an absolute timeout, the clock parameter of the time point
 * is used to measure the time. If the clock is adjusted forward, this may reduce the total length of the wait (as measured
 * by a steady clock), and if it's adjusted backwards, this may increase the total length of the wait. Time points are
 * used with the _until variants of the wait functions.
 */
bool wait_loop() {
    auto const timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lock(m);
    while (!done) {
        if (cv.wait_until(lock, timeout) == std::cv_status::timeout) {
            break;
        }
    }
    return done;
}

int main() {
    constexpr std::chrono::milliseconds ms{54802};
    constexpr std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);

    std::cout << ms.count() << " " << s.count() << std::endl;

    std::future<int> f = std::async([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        return 5;
    });
    if (f.wait_for(std::chrono::seconds(35)) == std::future_status::timeout) {
        std::cout << f.get() << std::endl;
    }
}