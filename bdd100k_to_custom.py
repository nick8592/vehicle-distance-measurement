import json
import os

def convert_bdd100k_to_custom(json_file, output_dir):
    # Check if output directory exists, if not, create it
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created output directory: {output_dir}")
    
    # Load BDD100K annotations from JSON file
    with open(json_file) as f:
        data = json.load(f)

    # Process each image annotation
    for item in data['frames']:
        frame_objects = item['objects']
        image_name = data['name'] + '.jpg'  # Assuming image name is same as "name" in the JSON
        
        # Prepare custom annotation content
        custom_annotations = []

        for annotation in frame_objects:
            label = annotation['category']
            
            # Check if the object has the box2d key
            if 'box2d' not in annotation:
                continue  # Skip this annotation if it doesn't have a box2d

            # Get bounding box coordinates
            x_min = round(annotation['box2d']['x1'])
            y_min = round(annotation['box2d']['y1'])
            x_max = round(annotation['box2d']['x2'])
            y_max = round(annotation['box2d']['y2'])
            width = (x_max - x_min)
            height = (y_max - y_min)

            # Get the class ID from the label using the custom class mapping
            if label in class_mapping:
                class_id = class_mapping[label]
            else:
                continue  # Skip if the label is not mapped

            # Append the formatted annotation to the list
            custom_annotations.append("{"f'{x_min}, {y_min}, {x_max}, {y_max}, "{label}"'"},")

        # Write annotations to a .txt file
        if custom_annotations:
            file_path = os.path.join(output_dir, os.path.splitext(image_name)[0] + '.txt')
            with open(file_path, 'w') as f:
                f.write("\n".join(custom_annotations))

        print(f"Processed {image_name}")


if __name__ == '__main__':

    # Updated class mapping based on your provided class names
    class_mapping = {
        "person": 0,
        "bicycle": 1,
        "car": 2,
        "motorcycle": 3,
        "bus": 4,
        "train": 5,
        "truck": 6,
        "traffic sign": 7,
        "traffic_light": 8,
        "stop_sign": 9,
        "curve_warning": 10,
        "bumpy": 11,
        "slippery_road": 12,
        "pedestrian_crossing": 13,
        "double_curve": 14,
        "traffic_signal_ahead": 15,
        "crossroad_junction_ahead": 16,
        "roundabout": 17,
        "road_narrow": 18,
        "no_motorcycle_and_car_allowed": 19,
        "no_turn": 20,
        "no_overtaking": 21,
        "speed_limit": 22,
        "no_stopping": 23,
        "turn": 24,
        "proceed_or_turn": 25,
        "keep_left_or_right": 26,
        "schoolzone": 27
    }

    # Example usage
    json_file = "bdd100k/labels/test/cb4614bc-47577a9a.json"  # Path to BDD100K JSON file
    output_dir = "bdd100k/custom_labels/test"  # Path to save custom annotations

    convert_bdd100k_to_custom(json_file, output_dir)
