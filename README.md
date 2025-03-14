# Vehicle Distance Measurement

This repository contains a set of tools for object detection and distance measurement using the BDD100K dataset annotations. The tools are designed to extract bounding boxes, calculate the distance of objects, and detect potential collisions. The entire workflow involves converting the BDD100K annotations from JSON to a custom format, then using that data to process images, calculate distances, and visualize results.

## Table of Contents
1. [Overview](#overview)
2. [Dependencies](#dependencies)
3. [Features](#features)
4. [How to Use](#how-to-use)
5. [Project Structure](#project-structure)
6. [Class and Function Details](#class-and-function-details)
7. [Custom Annotation Format](#custom-annotation-format)
8. [License](#license)

## Overview

This repository includes two main components:
1. **BDD100K to Custom Annotation Converter**: This script converts the BDD100K JSON annotations into a custom format containing bounding box coordinates and class IDs.
2. **Object Distance Measurement**: This script calculates the distance of detected objects in an image, displays the distances, and detects if an object is within a specified polygon for collision detection.

The two tools can work together to process the BDD100K dataset, create custom annotations, and measure the distance of detected objects in images. The custom annotation files can be used for training object detection models, while the distance measurement tool provides valuable insights into the real-world distances of detected objects.

## Dependencies

- **Python 3.x**: For the custom annotation converter script.
- **OpenCV**: For image processing and object detection in C++.
- **C++**: For compiling and running the distance measure code.
- **JSON and OS Libraries**: Standard Python libraries for handling JSON files and directories.
- **Filesystem Library**: For managing file paths and directories in Python.

## Features

### BDD100K to Custom Annotation Converter
- **Bounding Box Extraction**: Extracts bounding box coordinates from BDD100K annotations.
- **Class ID Mapping**: Converts object category names (e.g., "car", "person") to custom class IDs using a predefined mapping dictionary.
- **Custom Annotation Format**: Saves bounding box and class ID information in a custom `.txt` format suitable for object detection tasks.
- **Output Directory Management**: Automatically creates an output directory if it doesn't exist.

### Object Distance Measurement
- **Object Detection**: Detects specific objects (e.g., cars, trucks) and calculates their distance from the camera based on the size of their bounding box.
- **Collision Detection**: Identifies whether detected objects are within a specific polygon, useful for applications like autonomous driving or object tracking.
- **Visual Feedback**: Annotates the image with detected object distances, confidence scores, and custom labels.
- **Customizable Parameters**: You can define the objects to track, set the focal length, and adjust the size reference for various objects.

## How to Use

### Step 1: Convert BDD100K Annotations to Custom Format
1. **Download the BDD100K Dataset**: Ensure you have the BDD100K dataset's annotation JSON files.
2. **Update the Class Mapping**: Modify the `class_mapping` dictionary to ensure correct class IDs are assigned to the object categories.
3. **Set File Paths**: Set the paths for the BDD100K JSON files and the output directory for custom annotations.
4. **Run the Script**:
   - Save the script as `convert_bdd100k_to_custom.py`.
   - Run the script in your terminal:
     ```bash
     python convert_bdd100k_to_custom.py
     ```

The script will process the annotations and output `.txt` files in the specified output directory.

### Step 2: Measure Object Distance and Detect Collisions
1. **Prepare the Image and Annotation Data**: Ensure the custom annotations (from Step 1) are available for processing.
2. **Set Up the Distance Measure Code**: 
   - Ensure OpenCV and necessary libraries are installed.
   - Update the image path and bounding box data with the custom annotations in the C++ code.
3. **Compile and Run the C++ Code**:
   - Compile the C++ code:
     ```bash
     g++ -o distance_measure distance_measure.cpp `pkg-config --cflags --libs opencv4`
     ```
   - Run the compiled program:
     ```bash
     ./distance_measure
     ```

The program will:
- Calculate the distance of detected objects.
- Draw the detected points and distances on the image.
- Save the processed image.

## Project Structure

The directory structure of the project is as follows:
```
vehicle-distance-detection
├── bdd100k
│   ├── custom_labels
│   │   └── test
│   │       └── cabc30fc-eb673c5a.txt
│   ├── images
│   │   └── test
│   │       ├── cabc30fc-eb673c5a.jpg
│   │       └── cabc30fc-eb673c5a_processed.jpg
│   └── labels
│       └── test
│           └── cabc30fc-eb673c5a.json
├── bdd100k_to_custom.py
├── distance_measure
├── distance_measure.cpp
└── README.md
```


## Class and Function Details

### **BDD100K to Custom Annotation Converter**

#### `convert_bdd100k_to_custom(json_file, output_dir)`
Converts BDD100K annotations from JSON format to a custom format.

- **json_file**: Path to the BDD100K JSON file.
- **output_dir**: Directory to save the custom annotation `.txt` files.
- **class_mapping**: A dictionary that maps BDD100K class labels to custom class IDs.

#### Custom Annotation Format
The converted annotations are saved in `.txt` files with the following format:
```
x_min, y_min, x_max, y_max, "category"
```
For example:
```
100, 150, 300, 400, "car"
200, 250, 500, 600, "truck"
```

### **Object Distance Measurement**

#### `DistanceMeasure`
The class that handles distance measurement and collision detection.

- **Constructor**: 
  Initializes the object with a list of objects to track and the focal length.
  
- **Methods**:
  - `updateDistance(boxes)`: Updates the distance of detected objects based on their bounding box size.
  - `calcCollisionPoint(poly)`: Calculates the collision point for objects within a specified polygon.
  - `drawDetectedOnFrame(frame_show)`: Draws detected points and their distances on the image.

#### Key Variables:
- **f**: The focal length of the camera.
- **object_list**: A list of object categories to detect.
- **RefSizeDict**: A dictionary containing the reference sizes of various objects (e.g., cars, buses).

## Custom Annotation Format

The custom annotation format used for object detection tasks is:
```
x_min, y_min, x_max, y_max, "category"
```
Where:
- `x_min`, `y_min`: Top-left corner of the bounding box.
- `x_max`, `y_max`: Bottom-right corner of the bounding box.
- `"category"`: The object class name (e.g., "car", "person").

These annotations are saved in `.txt` files, one for each image.

## References
- [Vehicle-CV-ADAS](https://github.com/jason-li-831202/Vehicle-CV-ADAS.git)

## License

This project is licensed under the MIT License.