Threadpool C++
==============

A simple, header only threadpool implementation for C++11.

# Dependencies
C++ compiler with C++11 support. Tested with G++ 8.3 and 10.2 on Linux and G++ 7.3 on Windows (MinGW-64).

# Example
Please take a look at `example\threadpool-main.cpp`.

# Notes
1. On Windows, if the MinGW used does not support C++11 threading classes (`std::thread`, `std::mutex` etc), [`mingw-std-threads`](https://github.com/meganz/mingw-std-threads) header only library can be used to add support for it.

