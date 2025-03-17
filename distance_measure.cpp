#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <filesystem>

// Constants
static constexpr float INCH = 0.39;
std::map<std::string, std::pair<float, float>> RefSizeDict = {
    {"person", {160 * INCH, 50 * INCH}},
    {"bicycle", {100 * INCH, 65 * INCH}},
    {"motorbike", {100 * INCH, 100 * INCH}},
    {"car", {150 * INCH, 180 * INCH}},
    {"bus", {320 * INCH, 250 * INCH}},
    {"truck", {345 * INCH, 250 * INCH}}
};

// Function to draw detected points and add shadowed text
void drawDetectionAndText(cv::Mat& frame, const std::vector<std::vector<float>>& distance_points) {
    if (!distance_points.empty()) {
        for (const auto& point : distance_points) {
            cv::circle(frame, cv::Point(point[0], point[1]), 4, cv::Scalar(255, 255, 255), -1);

            std::string unit = "m";
            std::string text = (point[2] < 0) ? "unknown " + unit : std::to_string(point[2]) + " " + unit;

            double fontScale = std::max(0.4, std::min(1.0, 1.0 / point[2]));
            cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_TRIPLEX, fontScale, 3, nullptr);
            int textX = static_cast<int>(point[0] - textSize.width / 2);
            int textY = static_cast<int>(point[1] + textSize.height);

            // put Text on the frame
            cv::putText(frame, text, cv::Point(textX, textY), cv::FONT_HERSHEY_TRIPLEX, fontScale, cv::Scalar(255, 255, 255), 1);
        }
    }
}

// Function to update the distance based on object bounding boxes
void updateDistance(const std::vector<std::tuple<float, float, float, float, std::string, float>>& boxes, 
                    const std::vector<std::string>& object_list, int focal_length, 
                    std::vector<std::vector<float>>& distance_points) {
    distance_points.clear();

    if (!boxes.empty()) {
        for (const auto& box : boxes) {
            int xmin = std::get<0>(box), ymin = std::get<1>(box);
            int xmax = std::get<2>(box), ymax = std::get<3>(box);
            std::string label = std::get<4>(box);  // category
            float confidence = std::get<5>(box); // confidence score

            if (std::find(object_list.begin(), object_list.end(), label) != object_list.end() && ymax <= 650) {
                int point_x = (xmax + xmin) / 2;
                int point_y = ymax;

                try {
                    float distance = (RefSizeDict[label].first * focal_length) / (ymax - ymin);
                    distance = distance / 12 * 0.3048; // 1ft = 0.3048 m

                    // Store the distance point
                    distance_points.push_back({static_cast<float>(point_x), static_cast<float>(point_y), distance, confidence});
                }
                catch (const std::exception& e) {
                    std::cerr << "Error calculating distance: " << e.what() << std::endl;
                }
            }
        }
    }
    else {
        std::cout << "Box is empty!!!" << std::endl;
    }
}

namespace fs = std::filesystem;

int main() {
    // Load the input image
    std::string imagePath = "bdd100k/images/test/cb97debb-12f48570.jpg";  // Your original image path
    cv::Mat frame = cv::imread(imagePath);

    if (frame.empty()) {
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    // Initialize parameters
    std::vector<std::string> object_list = {"person", "bicycle", "car", "motorbike", "bus", "truck"};
    int focal_length = 400;
    std::vector<std::vector<float>> distance_points;

    // Simulate the bounding boxes (custom bounding box ground truth data)
    std::vector<std::tuple<float, float, float, float, std::string, float>> boxes = {
        {547, 142, 629, 227, "traffic sign", 1.0},
        {740, 287, 779, 299, "traffic sign", 1.0},
        {780, 286, 811, 297, "traffic sign", 1.0},
        {419, 232, 507, 267, "traffic sign", 1.0},
        {337, 235, 412, 270, "traffic sign", 1.0},
        {217, 242, 297, 275, "traffic sign", 1.0},
        {0, 265, 237, 398, "car", 1.0},
        {287, 284, 402, 352, "car", 1.0},
        {750, 301, 825, 362, "car", 1.0},
        {830, 301, 862, 327, "car", 1.0},
        {658, 298, 696, 326, "car", 1.0},
        {574, 308, 611, 329, "car", 1.0},
        {545, 307, 574, 327, "car", 1.0},
        {630, 307, 656, 324, "car", 1.0},
        {611, 303, 631, 322, "car", 1.0},
        {711, 306, 735, 322, "car", 1.0},
        {694, 307, 711, 317, "car", 1.0},
    };

    // Update the distances
    updateDistance(boxes, object_list, focal_length, distance_points);

    // Draw detected points and shadowed text on the frame
    drawDetectionAndText(frame, distance_points);

    // Show the result
    cv::imshow("Detected Points", frame);
    cv::waitKey(0); // Wait for a key press

    // Get the directory and filename of the original image
    fs::path originalPath(imagePath);
    std::string originalDir = originalPath.parent_path().string();
    std::string originalFilename = originalPath.stem().string();
    std::string originalExtension = originalPath.extension().string();

    // Create a new filename for the processed image
    std::string processedImagePath = originalDir + "/" + originalFilename + "_processed" + originalExtension;

    // Save the processed image
    bool success = cv::imwrite(processedImagePath, frame);

    if (success) {
        std::cout << "Processed image saved successfully as '" << processedImagePath << "'" << std::endl;
    } else {
        std::cerr << "Failed to save the processed image!" << std::endl;
    }

    return 0;
}
