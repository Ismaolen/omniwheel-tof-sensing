import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import sys
import tty
import termios

class TeleopNode(Node):
    def __init__(self):
        super().__init__('teleop_node')
        self.publisher_ = self.create_publisher(Twist, 'cmd_vel', 10)
        self.get_logger().info('Teleop Node has been started.')

    def send_twist(self, linear_x, linear_y, angular_z):
        twist = Twist()
        twist.linear.x = linear_x
        twist.linear.y = linear_y
        twist.angular.z = angular_z
        self.publisher_.publish(twist)
        self.get_logger().info(f'Published Twist: linear_x={linear_x}, linear_y={linear_y}, angular_z={angular_z}')

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

def main(args=None):
    rclpy.init(args=args)
    teleop_node = TeleopNode()

    try:
        print("Control the robot with 'w', 'a', 's', 'd' keys. Press 'q' to exit.")
        while rclpy.ok():
            key = get_key()
            if key == 'w':
                teleop_node.send_twist(50.0, 0.0, 0.0)  # Move forward
            elif key == 'a':
                teleop_node.send_twist(0.0, 0.0, 10.0)  # Rotate left
            elif key == 's':
                teleop_node.send_twist(-50.0, 0.0, 0.0)  # Move backward
            elif key == 'd':
                teleop_node.send_twist(0.0, 0.0, -10.0)  # Rotate right
            elif key == 'q':
                print("Exiting control.")
                break
            else:
                print("Invalid key pressed. Use 'w', 'a', 's', 'd' for movement and 'q' to quit.")

            # Allow the ROS node to process other callbacks
            rclpy.spin_once(teleop_node, timeout_sec=0.1)

    except KeyboardInterrupt:
        print("Program terminated")

    finally:
        teleop_node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()