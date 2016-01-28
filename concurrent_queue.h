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

/*
 * Credits: Alfredo Pons
 * Source: http://gnodebian.blogspot.com.es/2013/07/a-thread-safe-asynchronous-queue-in-c11.html
 */

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>


namespace thread {

template <typename T, class Container = std::list<T>>
class concurrent_queue {
private:
    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef Container container_type;

public:
    concurrent_queue() = default;
    concurrent_queue(const concurrent_queue & _other) {
        std::lock_guard<std::mutex> lock(_other.lock_);
        queue_  = _other.queue_;
    }
    concurrent_queue(concurrent_queue && _other) {
        queue_ = std::move(_other.queue_);
    }
    ~concurrent_queue() {
        std::lock_guard<std::mutex> lock(lock_);
    }

    void set_max_items(unsigned int _value) {
        max_items_ = _value;
    }

    unsigned int get_max_items() const {
        return max_items_;
    }

    void push(const value_type & _item) {
        std::lock_guard<std::mutex> lock(lock_);

        if (max_items_ > 0 && queue_.size() >= max_items_) {
            queue_.pop();
        }

        queue_.push(_item);
        condition_.notify_one();
    }

    void push(const value_type && _item) {
        std::lock_guard<std::mutex> lock(lock_);

        if (max_items_ > 0 && queue_.size() >= max_items_) {
            queue_.pop();
        }

        queue_.push(_item);
        condition_.notify_one();
    }

    void pop(value_type & _item) {
        std::unique_lock<std::mutex> lock(lock_);

        condition_.wait(lock, [this]() {
            return !queue_.empty();
        });

        _item = queue_.front();
        queue_.pop();
    }

    void move_pop(value_type & _item) {
        std::unique_lock<std::mutex> lock(lock_);

        condition_.wait(lock, [this]() {
            return !queue_.empty();
        });

        _item = std::move(queue_.front());
        queue_.pop();
    }

    void pull(value_type & _item) {
        std::unique_lock<std::mutex> lock(lock_);

        condition_.wait(lock, [this]() {
            return !queue_.empty();
        });

        _item = queue_.front();
    }

    void move_pull(value_type & _item) {
        std::unique_lock<std::mutex> lock(lock_);

        condition_.wait(lock, [this]() {
            return !queue_.empty();
        });

        _item = std::move(queue_.front());
        queue_.pop();
    }

    size_type size() const {
        std::lock_guard<std::mutex> lock(lock_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(lock_);
        return queue_.empty();
    }

    concurrent_queue & operator = (const concurrent_queue & _other) {
        if (this != &_other) {
            std::lock(lock_, _other.lock_);
            std::lock_guard<std::mutex> lock_a(lock_, std::adopt_lock);
            std::lock_guard<std::mutex> lock_b(_other.lock_, std::adopt_lock);
            std::queue<T, Container> temp { _other.queue_ };
            queue_.swap(temp);

            if (!queue_.empty()) {
                condition_.notify_all();
            }
        }
        return *this;
    }

    concurrent_queue & operator = (concurrent_queue && _other) {
        std::lock_guard<std::mutex> lock (lock_);
        queue_ = std::move(_other.queue_);

        if (!queue_.empty()) {
            condition_.notify_all();
        }

        return *this;
    }

private:
    std::queue<T, Container> queue_;
    mutable std::mutex lock_;
    std::condition_variable condition_;
    unsigned int max_items_ = 0;
};

}

#endif // CONCURRENT_QUEUE_H
