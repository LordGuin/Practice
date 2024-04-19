#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>

int main() {
    cv::CascadeClassifier eyeDetector;
    cv::CascadeClassifier smileDetector;
    cv::CascadeClassifier objDetector;

    if (!eyeDetector.load("haarcascade_eye.xml")) {
        std::cout << "Error loading eye detector" << std::endl;
        return -1;
    }
    if (!smileDetector.load("haarcascade_smile.xml")) {
        std::cout << "Error loading smile detector" << std::endl;
        return -1;
    }
    if (!objDetector.load("haarcascade_frontalface_default.xml")) {
        std::cout << "Error loading object detector" << std::endl;
        return -1;
    }

    cv::VideoCapture vid("input.mp4");
    if (!vid.isOpened()) {
        std::cout << "Error opening video file" << std::endl;
        return -1;
    }

    int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');
    int framesPerSec = vid.get(cv::CAP_PROP_FPS);
    cv::Size vidFrameSize(static_cast<int>(vid.get(cv::CAP_PROP_FRAME_WIDTH)),
        static_cast<int>(vid.get(cv::CAP_PROP_FRAME_HEIGHT)));
    cv::VideoWriter outputVid("result.mp4", codec, framesPerSec, vidFrameSize);

    cv::Mat currentFrame, grayFrame, blurredFrame;

    auto startTime = std::chrono::high_resolution_clock::now();

    while (true) {
        int frameCount = 0;
        vid >> currentFrame;
        if (currentFrame.empty()) {
            std::cout << "Error reading frame from video file" << std::endl;
            break;
        }

        cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(grayFrame, blurredFrame, cv::Size(7, 7), 0);

        std::vector<cv::Rect> objects;
        objDetector.detectMultiScale(blurredFrame, objects, 1.1, 9);

        for (int i = 0; i < objects.size(); i++) {
            const auto& obj = objects[i];

            rectangle(currentFrame, obj, cv::Scalar(0, 255, 255), 2);

            std::vector<cv::Rect> eyes;
            eyeDetector.detectMultiScale(blurredFrame(obj), eyes, 1.1, 35, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

            for (const auto& eye : eyes) {
                if (eye.area() > 0.01 * obj.area()) {
                    rectangle(currentFrame, eye + obj.tl(), cv::Scalar(255, 255, 0), 2);
                }
            }

            std::vector<cv::Rect> smiles;
            smileDetector.detectMultiScale(blurredFrame(obj), smiles, 2, 20, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

            for (const auto& smile : smiles) {
                if (smile.area() > 0.05 * obj.area()) {
                    rectangle(currentFrame, smile + obj.tl(), cv::Scalar(255, 0, 255), 2);
                }
            }
        }

        outputVid << currentFrame;

        imshow("Frames", currentFrame);

        if (cv::waitKey(1) == 27) {
            break;
        }
        frameCount++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "Processing time: " << duration << " ms" << std::endl;

    vid.release();
    outputVid.release();
    cv::destroyAllWindows();

    return 0;
}
