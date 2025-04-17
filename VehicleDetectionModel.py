import cv2
import torch
from ultralytics import YOLO
import requests
import time

# Constants
ESP32_CAM_URL = "http://192.168.110.201/?res=8"
NODEMCU_URL = "http://192.168.110.41/vehiclecount"
MODEL_WEIGHTS = "yolov8n.pt"  # Path to YOLOv8 model weights
CONFIDENCE_THRESHOLD = 0.5
FRAME_INTERVAL = 5  # Seconds

# Load YOLOv8 model
model = YOLO(MODEL_WEIGHTS)

def detect_vehicles(frame):
    """
    Detect vehicles in the frame using YOLOv8.
    Returns the count of vehicles detected and the annotated frame.
    """
    results = model(frame)[0]  # Get the first (and only) result

    vehicle_count = 0

    if results.boxes is not None:
        for box in results.boxes:
            cls_id = int(box.cls[0])
            conf = float(box.conf[0])
            x1, y1, x2, y2 = map(int, box.xyxy[0])

            # Detect only car(2), motorcycle(3), bus(5), truck(7)
            if conf > CONFIDENCE_THRESHOLD and cls_id in [2, 3, 5, 7]:
                vehicle_count += 1
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                label = f"{model.names[cls_id]} {conf:.2f}"
                cv2.putText(frame, label, (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    return vehicle_count, frame


def stream_and_analyze():
    cap = cv2.VideoCapture(ESP32_CAM_URL)

    if not cap.isOpened():
        print("Failed to open ESP32-CAM stream.")
        return
    frame_counter = 0
    while True:
        ret, frame = cap.read()
        frame = cv2.resize(frame, (320, 240))  # Before detect_vehicles

        if not ret:
            print("Failed to read frame.")
            break

        vehicle_count, annotated_frame = detect_vehicles(frame)
        cv2.imshow("ESP32-CAM Feed", annotated_frame)

        frame_counter += 1
        if frame_counter % 10 == 0 and vehicle_count > 0:  # Process every 10th frame
            send_vehicle_count(vehicle_count)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break


    cap.release()
    cv2.destroyAllWindows()

def send_vehicle_count(count):
    payload = f"{count},{count}"  # Sending the same count for both directions as a placeholder
    try:
        response = requests.post(NODEMCU_URL, data=payload, timeout=5)
        print("POST sent:", payload, "->", response.text)
    except Exception as e:
        print("Failed to send POST:", e)

if __name__ == "__main__":
    print("Press 'q' to quit")
    stream_and_analyze()
