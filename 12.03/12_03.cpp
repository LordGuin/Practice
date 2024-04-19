#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::Mat src = cv::imread("1.jpg", cv::IMREAD_COLOR);

    cv::Mat hsv, lab, yuv, xyz, gray;

    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(src, lab, cv::COLOR_BGR2Lab);
    cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);
    cv::cvtColor(src, xyz, cv::COLOR_BGR2XYZ);
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::imshow("HSV", hsv);
    cv::imshow("Lab", lab);
    cv::imshow("YUV", yuv);
    cv::imshow("XYZ", xyz);
    cv::GaussianBlur(gray, gray, cv::Size(11, 11), 3, 3);

    cv::Mat edges;
    cv::Canny(gray, edges, 50, 120, 3);

    // Применение морфологических операций для улучшения обнаружения кругов
    cv::Mat dilated_edges;
    cv::dilate(edges, dilated_edges, cv::Mat());
    cv::erode(dilated_edges, dilated_edges, cv::Mat());

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 100, 110); // Изменен порог параметра до 100

    for (size_t i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], line_ang = lines[i][1];
        cv::Point pt1, pt2;
        double a = cos(line_ang), b = sin(line_ang);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 800 * (-b));
        pt1.y = cvRound(y0 + 800 * (a));
        pt2.x = cvRound(x0 - 800 * (-b));
        pt2.y = cvRound(y0 - 800 * (a));
        cv::line(src, pt1, pt2, cv::Scalar(0, 0, 255), 3);
    }

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(dilated_edges, circles, cv::HOUGH_GRADIENT, 2, dilated_edges.rows /3, 40, 40, 30, 38); // Изменены параметры для лучшего обнаружения кругов

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(src, center, 3, cv::Scalar(0, 255, 0),2);
        cv::circle(src, center, radius, cv::Scalar(0, 255, 0),2);
    }

    cv::imshow("LineCircle", src);
    cv::waitKey(0);
    return 0;
}
