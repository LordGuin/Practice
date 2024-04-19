#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <chrono>
#include <locale>

void mergeVec(std::vector<int>& vec, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            vec[k] = L[i];
            i++;
        }
        else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}

void parallelMergeSort(std::vector<int>& vec, int l, int r) {
    if (l >= r)
        return;

    int m = l + (r - l) / 2;

#pragma omp task
    parallelMergeSort(vec, l, m);
#pragma omp task
    parallelMergeSort(vec, m + 1, r);

#pragma omp taskwait
    mergeVec(vec, l, m, r);
}

void mergeSort(std::vector<int>& vec, int l, int r) {
    if (l >= r)
        return;

    int m = l + (r - l) / 2;

    mergeSort(vec, l, m);
    mergeSort(vec, m + 1, r);

    mergeVec(vec, l, m, r);
}

int main() {


  
    int sizeVec = 10000;
    std::vector<int> vec(sizeVec);

    for (int i = 0; i < sizeVec; i++)
        vec[i] = rand() % sizeVec;

    std::vector<int> vecCopy(vec);

    auto startTime = std::chrono::high_resolution_clock::now();

    mergeSort(vecCopy, 0, sizeVec - 1);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "Sequential sort time: " << duration << " ms";
    std::cout << std::endl;

    auto startTimeParallel = std::chrono::high_resolution_clock::now();

#pragma omp parallel
    {
#pragma omp single
        parallelMergeSort(vec, 0, sizeVec - 1);
    }

    auto endTimeParallel = std::chrono::high_resolution_clock::now();
    auto durationParallel = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeParallel - startTimeParallel).count();

    std::cout << "Parallel sorting time: " << durationParallel << " ms";
    std::cout << std::endl << std::endl;

    int n, sum = 0;
    std::cout << "Enter array size: ";
    std::cin >> n;

    int* arr = new int[n];

    srand(time(0));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }

#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    std::cout << "Sum of array elements: " << sum;
    std::cout << std::endl;
 
    int numPoints = 1000000;
    int numInside = 0;
    double x, y;

#pragma omp parallel for reduction(+:numInside)
    for (int i = 0; i < numPoints; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            numInside++;
        }
    }

    double pi = 4.0 * numInside / numPoints;
    std::cout << "PI calculation using Monte Carlo method: " << pi << std::endl;

    delete[] arr;
    return 0;
}
