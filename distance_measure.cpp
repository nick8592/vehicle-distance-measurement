#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <filesystem>

class DistanceMeasure {
public:
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

    // Constructor
    DistanceMeasure(std::vector<std::string> object_list = {"person", "bicycle", "car", "motorbike", "bus", "truck"}, int focal_length = 400): object_list(object_list), f(focal_length) {}


    // Method to check if point is inside polygon
    bool isInsidePolygon(cv::Point pt, const std::vector<cv::Point>& poly) {
        bool c = false;
        int l = poly.size();
        int j = l - 1;

        for (int i = 0; i < l; i++) {
            if ((poly[i].x <= pt.x && pt.x < poly[j].x) || (poly[j].x <= pt.x && pt.x < poly[i].x)) {
                if (pt.y < (poly[j].y - poly[i].y) * (pt.x - poly[i].x) / (poly[j].x - poly[i].x) + poly[i].y) {
                    c = !c;
                }
            }
            j = i;
        }
        return c;
    }

    // Update the distance of the target object through the size of pixels
    void updateDistance(const std::vector<std::tuple<float, float, float, float, std::string, float>>& boxes) {
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
                        float distance = (RefSizeDict[label].first * f) / (ymax - ymin);
                        distance = distance / 12 * 0.3048; // 1ft = 0.3048 m
                        
                        // Print the distance to the terminal
                        std::cout << "Object: " << label << ", Distance: " << distance << " meters, Confidence: " << confidence << std::endl;
                        
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

    // Calculate collision point
    std::vector<float> calcCollisionPoint(const std::vector<cv::Point>& poly) {
        if (!distance_points.empty() && !poly.empty()) {
            std::sort(distance_points.begin(), distance_points.end(), [](const std::vector<float>& a, const std::vector<float>& b) {
                return a[2] < b[2];
            });

            for (const auto& point : distance_points) {
                cv::Point pt(point[0], point[1]);
                bool status = cv::pointPolygonTest(poly, pt, false) >= 0;

                if (status) {
                    return {point[0], point[1], point[2]};
                }
            }
        }
        return {};
    }

    // Draw detected objects on the frame
    void drawDetectedOnFrame(cv::Mat& frame_show) {
        if (!distance_points.empty()) {
            for (const auto& point : distance_points) {
                cv::circle(frame_show, cv::Point(point[0], point[1]), 4, cv::Scalar(255, 255, 255), -1);

                std::string unit = "m";
                std::string text = (point[2] < 0) ? "unknown " + unit : std::to_string(point[2]) + " " + unit;

                double fontScale = std::max(0.4, std::min(1.0, 1.0 / point[2]));
                cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_TRIPLEX, fontScale, 3, nullptr);
                int textX = static_cast<int>(point[0] - textSize.width / 2);
                int textY = static_cast<int>(point[1] + textSize.height);

                // Use shadow text with putText_shadow function
                putText_shadow(frame_show, text, cv::Point(textX + 1, textY + 5), cv::FONT_HERSHEY_TRIPLEX, fontScale, 
                               cv::Scalar(255, 255, 255), 1, cv::Scalar(150, 150, 150));
            }
        }
    }

private:
    std::vector<std::string> object_list;
    int f;  // focal length
    std::vector<std::vector<float>> distance_points;

    // Function to add shadowed text (just like in your original code)
    void putText_shadow(cv::Mat& frame, const std::string& text, cv::Point org, int fontFace, double fontScale,
                        const cv::Scalar& color, int thickness, const cv::Scalar& shadowColor) {
        cv::putText(frame, text, org + cv::Point(1, 1), fontFace, fontScale, shadowColor, thickness);
        cv::putText(frame, text, org, fontFace, fontScale, color, thickness);
    }
};

namespace fs = std::filesystem;

int main() {
    // Load the input image
    std::string imagePath = "bdd100k/images/test/cbf2d780-06947287.jpg";  // Your original image path
    cv::Mat frame = cv::imread(imagePath);

    if (frame.empty()) {
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    // Initialize the SingleCamDistanceMeasure object
    DistanceMeasure distanceMeasure;

    // Simulate the bounding boxes (custom bounding box ground truth data)
    // {"x, y, x1, y1, category, confidence score"}
    // x: top-left x cooridinate,       y: top-left y cooridinate
    // x1: bottom-right x cooridnate,   y1: bottom-right y cooridnate
    std::vector<std::tuple<float, float, float, float, std::string, float>> boxes = {
        {505, 344, 519, 380, "person", 0.88},
        {522, 339, 535, 374, "person", 0.88},
        {535, 341, 550, 375, "person", 0.88},
        {1225, 237, 1279, 483, "person", 0.88},
        {1055, 267, 1179, 486, "person", 0.88},
        {824, 269, 912, 487, "person", 0.88},
        {436, 340, 447, 350, "car", 0.88},
        {431, 346, 440, 356, "car", 0.88},
        {400, 343, 418, 365, "car", 0.88},
        {384, 342, 395, 354, "car", 0.88},
        {362, 341, 377, 346, "car", 0.88},
        {341, 348, 359, 363, "car", 0.88},
        {213, 344, 273, 406, "car", 0.88},
        {44, 359, 116, 424, "car", 0.88},
        {652, 316, 956, 434, "car", 0.88},
        {445, 341, 459, 352, "car", 0.88},
        {464, 342, 489, 359, "car", 0.88},
        {249, 344, 355, 425, "car", 0.88},
        {79, 354, 217, 435, "car", 0.88},
        {960, 328, 978, 387, "person", 0.88},
        {362, 343, 389, 365, "car", 0.88},
        {1005, 301, 1278, 452, "car", 0.88},
    };

    // Update the distances and print them to the terminal
    distanceMeasure.updateDistance(boxes);

    // Draw detected points on the frame
    distanceMeasure.drawDetectedOnFrame(frame);

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
