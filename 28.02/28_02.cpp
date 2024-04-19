#include <iostream>
#include <clocale>
#include <omp.h>
#include <limits>
using namespace std;

void greet(int times) {
    for (int i = 0; i < times; i++) {
        cout << "Hello, World!" << endl;
    }
}

void display_thread_sequence(int thread_count) {
#pragma omp parallel for
    {

#pragma omp barrier
        for (int i = 0; i < thread_count; i++) {
            std::cout << "Thread number " << i << std::endl;
        }
    }
}

void show_thread_numbers(int thread_count) {
    omp_set_num_threads(thread_count);
#pragma omp parallel
    {
        cout << "Thread " << omp_get_thread_num() << " is running." << endl;
    }
}

void display_partial_and_total_sum(int number, int thread_count) {
    omp_set_num_threads(thread_count);
    long long partial_sum = 0;
    long long grand_total = 0;

#pragma omp parallel shared(grand_total) private(partial_sum)
    {
#pragma omp for reduction(+:partial_sum)
        for (int i = 0; i <= number; i++) {
            partial_sum += i;
        }

#pragma omp critical
        {
            cout << "Partial sum: " << partial_sum << endl;
            grand_total += partial_sum;
        }
    }

    cout << "Grand total: " << grand_total << endl;
}

int main() {
    int value, thread_count;

    cout << "Enter the number of greetings: ";
    cin >> value;
    greet(value);

    cout << "Enter the number of threads for sequence display: ";
    cin >> thread_count;
    display_thread_sequence(thread_count);

    cout << "Enter the number of threads for showing thread numbers: ";
    cin >> thread_count;
    show_thread_numbers(thread_count);

    cout << "Enter a number for sum calculation: ";
    cin >> value;
    cout << "Enter the number of threads for sum calculation: ";
    cin >> thread_count;
    display_partial_and_total_sum(value, thread_count);

    return 0;
}
