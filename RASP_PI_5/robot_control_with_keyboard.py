import serial
import time
import struct
import sys
import termios
import tty

# Set up serial connection with specific port and baud rate
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=60)


def send_joint_movement(x, y, rot):
    # Define packet type and size
    packet_type = 0x02
    packet_size = 0x0C

    # Calculate checksum for data integrity
    checksum = packet_type ^ packet_size ^ 0x55

    # Pack data into a binary format, with checksum included
    data = struct.pack('<BBBBfff', 0x55, packet_type, packet_size, checksum, x, y, rot)

    # Display the values being sent
    print(f"Sending x: {x:.4f}, y: {y:.4f}, rot: {rot:.4f}")
    print("Sent data:", data.hex())  # Display hexadecimal representation of data

    # Send the packed data over serial
    ser.write(data)

def get_key():
    # Capture a single key press on Linux (non-blocking)
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

try:
    print("Control the robot with 'w', 'a', 's', 'd' keys. Press 'q' to exit.")
    while True:
        key = get_key()
        if key == 'w':
            send_joint_movement(50.0, 0.0, 0.0)  # Move forward
        elif key == 'a':
            send_joint_movement(0.0, 0.0, 10.0)  # Rotate left
        elif key == 's':
            send_joint_movement(-50.0, 0.0, 0.0)  # Move backward
        elif key == 'd':
            send_joint_movement(0.0, 0.0, -10.0)  # Rotate right
        elif key == 'q':
            print("Exiting control.")
            break
        else:
            print("Invalid key pressed. Use 'w', 'a', 's', 'd' for movement and 'q' to quit.")

        # time.sleep(0.1)  # Adjust delay as necessary

        # Check if there are incoming data and print them
        if ser.in_waiting > 0:
            incoming_data = ser.readline().decode('utf-8').rstrip()
            print("Received:", incoming_data)

except KeyboardInterrupt:
    # Handle script termination gracefully
    print("Program terminated")
finally:
    # Ensure the serial connection is closed on exit
    ser.close()
