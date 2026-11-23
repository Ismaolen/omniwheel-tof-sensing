import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import serial
import struct

class RobotController(Node):
    def __init__(self):
        super().__init__('robot_controller')
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            10)

        # Set up serial connection with specific port and baud rate
        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

    def listener_callback(self, msg):
        x = msg.linear.x
        y = msg.linear.y
        rot = msg.angular.z
        self.send_joint_movement(x, y, rot)

    def send_joint_movement(self, x, y, rot):
        # Define packet type and size
        packet_type = 0x02
        packet_size = 0x0C

        # Calculate checksum for data integrity
        checksum = packet_type ^ packet_size ^ 0x55

        # Pack data into a binary format, with checksum included
        data = struct.pack('<BBBBfff', 0x55, packet_type, packet_size, checksum, x, y, rot)

        # Display the values being sent
        self.get_logger().info(f"Sending x: {x:.4f}, y: {y:.4f}, rot: {rot:.4f}")
        self.get_logger().info("Sent data: " + data.hex())  # Display hexadecimal representation of data

        # Send the packed data over serial
        self.ser.write(data)

    def destroy_node(self):
        # Ensure the serial connection is closed on exit
        self.ser.close()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    robot_controller = RobotController()
    rclpy.spin(robot_controller)

    # Destroy the node explicitly
    robot_controller.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
