import socket
import cv2
import numpy as np
from openpyxl import Workbook
from datetime import datetime

# Set up UDP socket
udp_ip = "192.168.181.216"  # ESP8266 IP
udp_port = 4210
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Generate a unique filename based on the current date and time
timestamp_for_file = datetime.now().strftime("%Y%m%d_%H%M%S")
filename = f"robotic_ARM_LOG_{timestamp_for_file}.xlsx"

# Initialize workbook and sheet
wb = Workbook()
ws = wb.active
ws.append(["Time", "Right Count", "Left Count"])  # Headers

# Initialize variables
last_message = ""
right_count = 0
left_count = 0
wait_for_done = False

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()

# Color ranges in HSV
lower_red, upper_red = np.array([0, 120, 70]), np.array([10, 255, 255])
lower_black, upper_black = np.array([0, 0, 0]), np.array([180, 255, 30])

while True:
    ret, frame = cap.read()
    if not ret:
        print("Error: Failed to capture image.")
        break

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    red_mask, black_mask = cv2.inRange(hsv, lower_red, upper_red), cv2.inRange(hsv, lower_black, upper_black)
    red_count, black_count = cv2.countNonZero(red_mask), cv2.countNonZero(black_mask)

    # Listen for DONE response if waiting
    if wait_for_done:
        try:
            data, addr = sock.recvfrom(1024)
            if data.decode('utf-8') == "DONE":
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                if last_message == "right":
                    right_count += 1
                    ws.append([timestamp, right_count, left_count])
                elif last_message == "left":
                    left_count += 1
                    ws.append([timestamp, right_count, left_count])
                wb.save(filename)  # Save the workbook with the unique filename
                wait_for_done = False  # Reset waiting state
        except socket.timeout:
            pass

    # Send message only if not waiting for DONE
    elif red_count > 500:
        sock.sendto("right".encode('utf-8'), (udp_ip, udp_port))
        last_message, wait_for_done = "right", True
        print("Sent message: right")
    elif black_count > 500:
        sock.sendto("left".encode('utf-8'), (udp_ip, udp_port))
        last_message, wait_for_done = "left", True
        print("Sent message: left")

    cv2.imshow('Frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
sock.close()
