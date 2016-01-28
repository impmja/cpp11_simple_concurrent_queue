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
 * Credits: Howard Hinnant @ https://github.com/HowardHinnant
 * Source: https://stackoverflow.com/questions/18277304/using-stdcout-in-multiple-threads
 */

#ifndef CONCURRENT_COUT_H
#define CONCURRENT_COUT_H

#include <iostream>
#include <mutex>

namespace thread {

std::ostream& print_one(std::ostream & os) {
    return os;
}

template <class A0, class ...Args>
std::ostream & print_one(std::ostream & os, const A0 & a0, const Args & ...args) {
    os << a0;
    return print_one(os, args...);
}

template <class ...Args>
std::ostream & print(std::ostream& os, const Args & ...args) {
    return print_one(os, args...);
}

template <class ...Args>
std::ostream & print(const Args& ...args) {
    static std::mutex lock_;
    std::lock_guard<std::mutex> lock(lock_);
    return print(std::cout, args...);
}

template <class ...Args>
std::ostream & println(const Args& ...args) {
    return print(args...) << std::endl;
}

}

#endif // CONCURRENT_COUT_H
