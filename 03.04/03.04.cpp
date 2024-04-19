#include <complex>
#include <mpi.h>
#include <opencv2/opencv.hpp>

const int MAX_ITERATIONS = 100;
const int MAX_COLOR_INDEX = 50;
const int MID_COLOR_INDEX = MAX_COLOR_INDEX / 2;
const double X_MIN = -2.0;
const double X_MAX = 1.0;
const double Y_MIN = -1.5;
const double Y_MAX = 1.5;

cv::Vec3b colorMapping(int iterations) {
    cv::Vec3b color;

    if (iterations == MAX_ITERATIONS) {
        return color;
    }

    int index = iterations % MAX_COLOR_INDEX;

    if (index > MID_COLOR_INDEX) {
        index = MAX_COLOR_INDEX - index;
    }

    int value = (index * 255) / MAX_COLOR_INDEX;

    color[0] = value;
    color[1] = value * 2; 
    color[2] = value * 3; 

    if (color[0] > 255) {
        color[0] = 255;
    }
    if (color[1] > 255) {
        color[1] = 255;
    }
    if (color[2] > 255) {
        color[2] = 255;
    }

    return color;
}


int fractalCalculation(double realPart, double imaginaryPart) {
    int iterations = 0;
    std::complex<double> z;
    std::complex<double> c(realPart, imaginaryPart);

    while (iterations < MAX_ITERATIONS && std::abs(z) <= 2.0) {
        z = z * z + c;
        iterations++;
    }

    return iterations;
}

int main(int argCount, char** argValues) {
    int processRank, processCount;
    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    int imageWidth = 1000;
    int imageHeight = 1000;
    int chunkSize = imageHeight / processCount;
    int startRow = processRank * chunkSize;
    int endRow = (processRank == processCount - 1) ? imageHeight : startRow + chunkSize;

    cv::Mat globalImage(imageHeight, imageWidth, CV_8UC3);
    cv::Mat localImage(endRow - startRow, imageWidth, CV_8UC3);

    for (int y = startRow; y < endRow; y++) {
        for (int x = 0; x < imageWidth; x++) {
            double realPart = X_MIN + (X_MAX - X_MIN) * x / imageWidth;
            double imaginaryPart = Y_MIN + (Y_MAX - Y_MIN) * y / imageHeight;

            int iterations = fractalCalculation(realPart, imaginaryPart);
            cv::Vec3b color = colorMapping(iterations);

            localImage.at<cv::Vec3b>(y - startRow, x)[0] = color[0];
            localImage.at<cv::Vec3b>(y - startRow, x)[1] = color[1];
            localImage.at<cv::Vec3b>(y - startRow, x)[2] = color[2];
        }
    }

    MPI_Gather(localImage.data, localImage.total() * localImage.channels(), MPI_UNSIGNED_CHAR,
        globalImage.data, localImage.total() * localImage.channels(), MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD);

    if (processRank == 0) {
        cv::namedWindow("Fractal Image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Fractal Image", globalImage);
        cv::waitKey(0);
        cv::destroyAllWindows();
        cv::imwrite("fractal_image.png", globalImage);
    }

    MPI_Finalize();
    return 0;
}
