#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <vector>

bool isThreeSided(const std::vector<cv::Point>& vertexList) {
    std::vector<cv::Point> simplified;
    cv::approxPolyDP(vertexList, simplified, cv::arcLength(vertexList, true) * 0.02, true);
    return simplified.size() == 3;
}

void process_image(const std::string& image_location, const std::string& window_name, int blur_radius, bool show_labels) {
    cv::Mat image = cv::imread(image_location, cv::IMREAD_COLOR);

    cv::Mat grayscale;
    cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);

    cv::Mat smoothed;
    cv::GaussianBlur(grayscale, smoothed, cv::Size(blur_radius, blur_radius), 0);

    cv::Mat edge_map;
    cv::Canny(smoothed, edge_map, 30, 90);

    cv::Mat colored_edge_map = cv::Mat::zeros(image.size(), CV_8UC3);

    std::vector<std::vector<cv::Point>> vertex_lists;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edge_map, vertex_lists, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(colored_edge_map, vertex_lists, -1, cv::Scalar(0, 255, 0), 2);

    if (show_labels) {
        for (size_t i = 0; i < vertex_lists.size(); i++) {
            double area = cv::contourArea(vertex_lists[i]);
            cv::Rect bounding_box = cv::boundingRect(vertex_lists[i]);
            float width_height_ratio = static_cast<float>(bounding_box.width) / bounding_box.height;

            std::string shape_name;
            if (isThreeSided(vertex_lists[i])) {
                shape_name = "triangle";
            }
            else if (std::abs(1.0 - width_height_ratio) <= 0.05 && std::abs(1.0 - area / (bounding_box.width * bounding_box.height)) <= 0.05) {
                shape_name = "square";
            }
            else if (width_height_ratio >= 0.95 && width_height_ratio <= 1.05) {
                shape_name = "circle";
            }
            else {
                shape_name = "unknown";
            }

            cv::Point text_position(bounding_box.x + bounding_box.width / 2 - shape_name.size() * 4 / 2, bounding_box.y + bounding_box.height / 2);
            cv::putText(colored_edge_map, shape_name, text_position, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
        }
    }

    cv::namedWindow("Processed Image", cv::WINDOW_NORMAL);
    cv::imshow("Processed Image", colored_edge_map);
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", image);

    cv::waitKey(0);
    cv::destroyAllWindows();
}

int main() {
    process_image("geometry.jpg", "geometry", 3, true);
    process_image("text.jpg", "text", 11, false);

    return 0;
}
