#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture video("vid.mp4");

    int width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::namedWindow("Source", cv::WINDOW_NORMAL);
    cv::resizeWindow("Source", width, height);

    cv::namedWindow("Filtered", cv::WINDOW_NORMAL);
    cv::resizeWindow("Filtered", width, height);

    cv::Mat currentFrame, frame_HSV, filteredFrame, contourFrame;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::Scalar lowerColor(70, 10, 130);
    cv::Scalar upperColor(255, 50, 255);

    int fourcc = cv::VideoWriter::fourcc('H', '2', '6', '4');
    cv::VideoWriter output("result.mp4", fourcc, video.get(cv::CAP_PROP_FPS), cv::Size(width, height), false);

    while (true) {

        video >> currentFrame;
        if (currentFrame.empty()) {
            break;
        }

        cv::cvtColor(currentFrame, frame_HSV, cv::COLOR_BGR2HSV);
        cv::inRange(frame_HSV, lowerColor, upperColor, filteredFrame);
        cv::erode(filteredFrame, filteredFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
        cv::dilate(filteredFrame, filteredFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
        cv::findContours(filteredFrame, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) {

            std::vector<cv::Point> simplifiedContour;
            cv::approxPolyDP(contours[i], simplifiedContour, cv::arcLength(contours[i], true) * 0.02, true);

            cv::Moments contourMoments = cv::moments(contours[i]);
            int cx = contourMoments.m10 / contourMoments.m00;
            int cy = contourMoments.m01 / contourMoments.m00;

            std::string shapeType;
            if (simplifiedContour.size() == 3) {
                shapeType = "triangle";
            }
            else if (simplifiedContour.size() == 4) {
                float aspectRatio = (float)cv::boundingRect(simplifiedContour).width / cv::boundingRect(simplifiedContour).height;
                if (aspectRatio >= 0.95 && aspectRatio <= 1.05) {
                    shapeType = "square";
                }
                else {
                    shapeType = "rectangle";
                }
            }
            else {
                shapeType = "unknown";
            }

            cv::putText(currentFrame, shapeType, cv::Point(cx, cy), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
            cv::drawContours(currentFrame, contours, i, cv::Scalar(0, 0, 255), 2);
        }

        output.write(currentFrame);

        cv::imshow("Source", currentFrame);
        cv::imshow("Mask", filteredFrame);

        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    video.release();
    output.release();

    cv::destroyWindow("Source");
    cv::destroyWindow("Mask");

    return 0;
}
