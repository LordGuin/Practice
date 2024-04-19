#include <opencv2/opencv.hpp>

int main() {
    cv::Mat image = cv::imread("kotik.jpg", cv::IMREAD_COLOR);

    // Drawing shapes
    cv::line(image, cv::Point(10, 10), cv::Point(1000, 1000), cv::Scalar(255, 255, 0), 2);
    cv::ellipse(image, cv::Point(700, 700), cv::Size(100, 50), 45, 0, 360, cv::Scalar(0, 0, 255), 2);
    cv::rectangle(image, cv::Point(200, 200), cv::Point(300, 300), cv::Scalar(0, 255, 0), 2);
    cv::circle(image, cv::Point(500, 500), 75, cv::Scalar(255, 0, 0), 2);

    std::vector<cv::Point> triangle_points;
    triangle_points.push_back(cv::Point(300, 100));
    triangle_points.push_back(cv::Point(400, 100));
    triangle_points.push_back(cv::Point(350, 200));
    cv::polylines(image, std::vector<std::vector<cv::Point>>{triangle_points}, true, cv::Scalar(0, 0, 255), 2);

    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", image);

    // Splitting image into parts
    int rows = image.rows;
    int cols = image.cols;

    cv::Mat photo1 = image(cv::Rect(0, 0, cols / 2, rows / 2));
    cv::Mat photo2 = image(cv::Rect(cols / 2, 0, cols / 2, rows / 2));
    cv::Mat photo3 = image(cv::Rect(0, rows / 2, cols / 2, rows / 2));
    cv::Mat photo4 = image(cv::Rect(cols / 2, rows / 2, cols / 2, rows / 2));

    cv::namedWindow("Photo 1", cv::WINDOW_NORMAL);
    cv::imshow("Photo 1", photo1);
    cv::namedWindow("Photo 2", cv::WINDOW_NORMAL);
    cv::imshow("Photo 2", photo2);
    cv::namedWindow("Photo 3", cv::WINDOW_NORMAL);
    cv::imshow("Photo 3", photo3);
    cv::namedWindow("Photo 4", cv::WINDOW_NORMAL);
    cv::imshow("Photo 4", photo4);

    // Creating a square mask
    cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::rectangle(mask, cv::Point(image.cols / 2 - 100, image.rows / 2 - 100), cv::Point(image.cols / 2 + 100, image.rows / 2 + 100), cv::Scalar(255), -1);

    cv::Mat finalRes;
    image.copyTo(finalRes, mask);

    cv::namedWindow("Mask", cv::WINDOW_NORMAL);
    cv::imshow("Mask", finalRes);

    cv::waitKey(0);

    return 0;
}
