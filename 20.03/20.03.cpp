#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>
#include <locale>
#include <windows.h>

int main() {

    setlocale(LC_ALL, "Ru");

    cv::Mat originalImage = cv::imread("car.jpg", cv::IMREAD_COLOR);

    cv::Mat grayscaleImage, sepiaTone, inverted, contours;

#pragma omp sections
    {
#pragma omp section
        {
            cv::cvtColor(originalImage, grayscaleImage, cv::COLOR_BGR2GRAY);
        }

#pragma omp section
        {
            cv::Mat sepiaKernel = (cv::Mat_<double>(3, 3) << 0.272, 0.534, 0.131,
                0.349, 0.686, 0.168,
                0.393, 0.769, 0.189);
            cv::transform(originalImage, sepiaTone, sepiaKernel);
        }

#pragma omp section
        {
            cv::bitwise_not(originalImage, inverted);
        }

#pragma omp section
        {
            cv::Canny(originalImage, contours, 100, 200);
        }
    }

    std::vector<std::pair<std::string, cv::Mat>> imageList = {
        {"Original", originalImage},
        {"Grayscale", grayscaleImage},
        {"Sepia", sepiaTone},
        {"Inverted", inverted},
        {"Contours", contours}
    };

    int currentImageIndex = 0;

    cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Image", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::imshow("Image", imageList[currentImageIndex].second);

    while (true) {
        int key = cv::waitKey(0);
        if (key == 32) { 
            currentImageIndex = (currentImageIndex + 1) % imageList.size();
            cv::imshow("Image", imageList[currentImageIndex].second);
            std::cout << imageList[currentImageIndex].first << std::endl;
        }
        else if (key == 27 || key == 13) { 
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}
