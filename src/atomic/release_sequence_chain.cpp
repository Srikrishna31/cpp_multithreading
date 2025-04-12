//
// Created by coolk on 12-04-2025.
//

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::vector<int> queue_data;
std::atomic<int> count;

void populate_queue() {
    constexpr unsigned NUMBER_OF_ITEMS = 20;
    queue_data.clear();
    for (auto i = 0; i < NUMBER_OF_ITEMS; ++i) {
        queue_data.push_back(i);
    }
    count.store(NUMBER_OF_ITEMS, std::memory_order_release);    // <--- (1) The initial store
}

void process(const int item) {
    std::cout << "From thread: " << std::this_thread::get_id() << " value read: " << item << std::endl;
}

[[noreturn]] void consume_queue_items() {
    while (true) {
        int item_index;
        if ((item_index=count.fetch_sub(1, std::memory_order_acquire)) <= 0) {  // <--- (2) An RMW operation
            std::this_thread::yield();      // <--- (3) wait for more items.
            continue;
        }
        process(queue_data[item_index]);    // <--- (4) Reading queue data is safe.
    }
}

/**
 * Release sequences and synchronizes_with
 *
 * If the store is tagged with memory_order_release, memory_order_acq_rel, or memory_order_seq_cst, and the load is
 * tagged with memory_order_consume, memory_order_acquire, or memory_order_seq_cst, and each operation in the chain loads
 * the value written by the previous operation, then the chain of operations in the chain loads the value written by the
 * previous operation, then the chain of operations constitutes a release sequence and the initial store synchronizes with
 * (for memory_order_acquire or memory_order_seq_cst) or is dependency-ordered-before (for memory_order_consume) the final
 * load. Any atomic read-modify-write operations in the chain can have any memory ordering (even memory_order_relaxed).
 *
 * One way to handle things would be to have the thread that's producing the data store the items in a shared buffer and
 * then do count.store(number_of_items, memory_order_release) (1) to let the other threads know that data is available. The
 * threads consuming the queue items might then do count.fetch_sub(1, memory_order_acquire) (2) to claim an item from the
 * queue, prior to actually reading the shared buffer (4). Once the count becomes zero, there are no more items, and the
 * thread must wait (3).
 *
 * If there's one consumer thread, this is fine; the fetch_sub() is a read, with memory_order_acquire semantics, and the
 * store had memory_order_release semantics, so the store synchronizes-with the load and the thread can read the item from
 * the buffer. If there are two threads reading, the second fetch_sub() will see the value written by the first and not
 * the value written by the store. Without the rule about the release sequence, this second thread wouldn't have a
 * happens-before relationship with the first thread, and it wouldn't be safe to read the shared buffer unless the first
 * fetch_sub() also had memory_order_release semantics, which would introduce unnecessary synchronization between the
 * two consumer threads. Without the release sequence rule or memory_order_release semantics on the fetch_sub operations,
 * there would be nothing to require that the stores to the queue_data were visible to the second consumer, and you would
 * have a data race.
 * There can be any number of links in the chain, but provided they're all read-modify-write operations such as fetch_sub(),
 * the store() will still synchronize-with each one that's tagged memory_order_acquire.
 */
int main() {
    std::thread a (populate_queue);
    std::thread b (consume_queue_items);
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();
}

