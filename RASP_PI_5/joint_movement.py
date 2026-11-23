import serial
import time
import struct
import random

# Set up serial connection with specific port and baud rate
# ser = serial.Serial('/dev/ttyACM0', 115200, timeout=60)
# ser = serial.Serial('/dev/serial/by-id/usb-STMicroelectronics_STLINK-V3_003800233137510539383538-if02', 115200, timeout=60)
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

try:
    while True:
        # Generate random values for x, y, and rotation within a realistic range
        x = round(random.uniform(10.0, 50.0), 2)  # Speed in mm/min along X-axis
        y = round(random.uniform(0.0, 0.0), 2)  # Speed in mm/min along Y-axis
        rot = round(random.uniform(0.0, 0.0), 2)    # Rotational speed in RPM

        # Send joint movement command
        send_joint_movement(x, y, rot)

        time.sleep(0.1)  # Pause for 4 seconds

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
