/*
The MIT License (MIT)

Copyright (c) 2016 Jan Schulte

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <thread>
#include <iostream>
#include <algorithm>
#include <atomic>

#include "concurrent_cout.h"
#include "concurrent_queue.h"


int main() {
    thread::concurrent_queue<unsigned int> cqueue;
    cqueue.set_max_items(10);

    std::atomic_uint current_value(0);
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (unsigned int i = 0; i < 1; ++i) {
        producers.push_back(std::thread([i, &cqueue, &current_value](){
            while(true) {
                unsigned int const item = ++current_value;
                cqueue.push(item);
                std::size_t const size = cqueue.size();
                thread::println("Producer [", i, "] pushed item [", item, "] - Queue Size [", size ,"]");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }));
    }

    for (unsigned int i = 0; i < 4; ++i) {
        consumers.push_back(std::thread([i, &cqueue]() {
            while(true) {
                unsigned int item = 0;
                cqueue.pop(item);
                std::size_t const size = cqueue.size();
                thread::println("Consumer [", i, "] poped item [", item, "] - Queue Size [", size,"]");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }));
    }

    thread::println("--- Concurrent Queue Test ---");

    thread::println("|- Starting Producer Threads...");
    std::for_each(producers.begin(), producers.end(), [](std::thread &t) {
        t.join();
    });

    thread::println("|- Starting Consumer Threads...");
    std::for_each(consumers.begin(), consumers.end(), [](std::thread &t) {
        t.join();
    });

    return 0;
}