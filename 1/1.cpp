#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>

std::mutex locker;

void multi_threaded_factorial(int num) {
    unsigned long long result = 1;
    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;

    int chunk = num / 4;

    for (int i = 0; i < 4; i++) {
        int start = chunk * i + 1;
        int end = (i == 3) ? num : chunk * (i + 1);
        workers.push_back(std::thread([&, start, end]() {
            unsigned long long partial_result = 1;
            for (int j = start; j <= end; j++) {
                partial_result *= j;
            }
            locker.lock();
            result *= partial_result;
            locker.unlock();
            }));
    }

    for (auto& t : workers) {
        t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    locker.lock();
    std::cout << "4-threaded calculation: " << result << " in " << time.count() << " nanoseconds." << std::endl;
    locker.unlock();
}

void single_threaded_factorial(int num) {
    unsigned long long result = 1;
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 1; i <= num; i++) {
        result *= i;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    locker.lock();
    std::cout << "Single-threaded calculation: " << result << " in " << time.count() << " nanoseconds." << std::endl;
    locker.unlock();
}


int main() {
    int number;
    std::cout << "Enter a number for factorial calculation:  ";

    if (!(std::cin >> number) || number < 0) {
        std::cout << "Input error. Enter a non-negative integer." << std::endl;
        return 0;
    }

    single_threaded_factorial(number);
    multi_threaded_factorial(number);

    return 0;
}
