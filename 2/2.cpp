#include <iostream>
#include <mutex>
#include <fstream>
#include <thread>

std::mutex locker;
int shared_counter = 0;

void unsynchronized() {
    for (int i = 0; i < 5000; ++i) {
        shared_counter++;
    }
}

void synchronized() {
    for (int i = 0; i < 5000; ++i) {

        locker.lock();
        shared_counter++;

        locker.unlock();
    }
}

int main() {
    int unsynch_counter = 0;
    shared_counter = 0;
    std::thread unsynch_thread1(unsynchronized);
    std::thread unsynch_thread2(unsynchronized);

    unsynch_thread1.join();
    unsynch_thread2.join();

    unsynch_counter = shared_counter;

    std::cout << "Counter without using mutex: " << unsynch_counter << std::endl;

    int synch_counter = 0;
    shared_counter = 0;
    std::thread synch_thread1(synchronized);
    std::thread synch_thread2(synchronized);

    synch_thread1.join();
    synch_thread2.join();

    synch_counter = shared_counter;

    std::cout << "Counter with using mutex: " << synch_counter << std::endl;

    std::ofstream outputFile("output.txt", std::ios_base::app);

    outputFile << "Counter without using mutex: " << unsynch_counter << std::endl;
    outputFile << "Counter with using mutex: " << synch_counter << std::endl << std::endl;
    outputFile.close();

    return 0;
}
