import cv2
import mediapipe as mp
import math
import numpy as np
import socket

N_LEFT_SHOULDER = 11
N_RIGHT_SHOULDER = 12
N_LEFT_ELBOW = 13
N_RIGHT_ELBOW = 14
N_LEFT_WRIST = 15
N_RIGHT_WRIST = 16

SERVER_IP = '172.20.10.10'
SERVER_PORT = 8888
BUFFER_SIZE = 1024


def calculate_angle(point1, point2, point3):
    """

    :param point1:
    :param point2: intersection of two straight lines
    :param point3:
    :return: angle
    """
    angle_line1 = math.atan2(-(point1.y - point2.y), (point1.x - point2.x)) * 180 / math.pi
    angle_line2 = math.atan2(-(point3.y - point2.y), (point3.x - point2.x)) * 180 / math.pi

    if angle_line2 < angle_line1:
        angle = angle_line1 - angle_line2
    else:
        angle = 360 - (angle_line2 - angle_line1)

    return angle


def draw_angle(img, point1, point2, point3, angle):
    """

    :param img:
    :param point1:
    :param point2: intersection of two straight lines
    :param point3:
    :return: angle
    """
    angle_line1 = math.atan2(-(point1.y - point2.y), (point1.x - point2.x)) * 180 / math.pi

    if angle_line1 <= 0:
        rotation = -angle_line1
    else:
        rotation = 360 - angle_line1

    cv2.ellipse(img, (int(point2.x * 640), int(point2.y * 480)),
                (32, 24), rotation, 0, angle, (0, 0, 255), 5)


mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_pose = mp.solutions.pose

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((SERVER_IP, SERVER_PORT))

# Abortion strategy.
# Step 1: Input a default image for configuration to find the original proportions.
# Step 2: Find the real-time proportion.
# Step 3: Estimate the 3D coordinates of points of arms.

cap = cv2.VideoCapture(0)
with mp_pose.Pose(
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5) as pose:
    while cap.isOpened():
        success, image = cap.read()
        if not success:
            print("Ignoring empty camera frame.")
            # If loading a video, use 'break' instead of 'continue'.
            continue

        # To improve performance, optionally mark the image as not writeable to
        # pass by reference.
        image.flags.writeable = False
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        results = pose.process(image)

        # Draw the pose annotation on the image.
        image.flags.writeable = True
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        mp_drawing.draw_landmarks(
            image,
            results.pose_landmarks,
            mp_pose.POSE_CONNECTIONS,
            landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style())

        # cv2.imshow('MediaPipe Pose', cv2.flip(image, 1))
        # if cv2.waitKey(5) & 0xFF == 27:
        #     break

        # Determine if both arm and nose points are visible.
        if results.pose_landmarks is not None:
            lm = results.pose_landmarks.landmark
        else:
            cv2.imshow('Marking Pose', image)
            if cv2.waitKey(5) & 0xFF == 27:
                send_data = "end"
                client_socket.send(send_data.encode("UTF-8"))
                break
            continue

        if lm[N_LEFT_SHOULDER].visibility < 0.7 or lm[N_RIGHT_SHOULDER].visibility < 0.7 \
                or lm[N_LEFT_ELBOW].visibility < 0.7 or lm[N_LEFT_WRIST].visibility < 0.7:
            cv2.imshow('Marking Pose', image)
            if cv2.waitKey(5) & 0xFF == 27:
                send_data = "end"
                client_socket.send(send_data.encode("UTF-8"))
                break
            continue

        # Calculate the angle between the lines.
        angle_l1 = calculate_angle(lm[N_LEFT_SHOULDER], lm[N_LEFT_ELBOW], lm[N_LEFT_WRIST])
        angle_l2 = calculate_angle(lm[N_RIGHT_SHOULDER], lm[N_LEFT_SHOULDER], lm[N_LEFT_ELBOW])
        print("angle1 = {0}, angle2 = {1}".format(angle_l1, angle_l2))

        # Judge if it's an arm movement paradigm.
        if 165 < angle_l1 < 195 and 165 < angle_l2 < 195:
            left_paradigm = 0
        elif 110 < angle_l1 < 150 and 180 < angle_l2 < 270 \
                and lm[N_LEFT_WRIST].z < lm[0].z:
            left_paradigm = 2
        elif 110 < angle_l1 < 150 and 165 < angle_l2 < 270 \
                and lm[N_LEFT_WRIST].z > lm[0].z:
            left_paradigm = 3
        elif 270 < angle_l1 < 330 and 180 < angle_l2 < 270 \
                and lm[N_LEFT_WRIST].z < lm[N_LEFT_ELBOW].z < lm[N_LEFT_SHOULDER].z:
            left_paradigm = 4
        elif 80 < angle_l1 < 100 and 165 < angle_l2 < 195:
            left_paradigm = 1
        else:
            cv2.imshow('Marking Pose', image)
            if cv2.waitKey(5) & 0xFF == 27:
                send_data = "end"
                client_socket.send(send_data.encode("UTF-8"))
                break
            continue

        # Draw reference lines and show the paradigm number
        image = cv2.line(image, (int(lm[N_LEFT_SHOULDER].x * 640), int(lm[N_LEFT_SHOULDER].y * 480)),
                       (int(lm[N_LEFT_ELBOW].x * 640), int(lm[N_LEFT_ELBOW].y * 480)), (0, 0, 255), 5)
        image = cv2.line(image, (int(lm[N_LEFT_WRIST].x * 640), int(lm[N_LEFT_WRIST].y * 480)),
                       (int(lm[N_LEFT_ELBOW].x * 640), int(lm[N_LEFT_ELBOW].y * 480)), (0, 0, 255), 5)
        draw_angle(image, lm[N_LEFT_SHOULDER], lm[N_LEFT_ELBOW], lm[N_LEFT_WRIST], angle_l1)
        draw_angle(image, lm[N_RIGHT_SHOULDER], lm[N_LEFT_SHOULDER], lm[N_LEFT_ELBOW], angle_l2)
        cv2.putText(image, 'action paradigm {0}'.format(left_paradigm), (50, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)

        # Transmit the paradigm.
        send_data = "*{0}{1}{2}.".format(left_paradigm, left_paradigm, left_paradigm)
        client_socket.send(send_data.encode("UTF-8"))

        cv2.imshow('Marking Pose', image)
        if cv2.waitKey(5) & 0xFF == 27:
            send_data = "end"
            client_socket.send(send_data.encode("UTF-8"))
            break
cap.release()
