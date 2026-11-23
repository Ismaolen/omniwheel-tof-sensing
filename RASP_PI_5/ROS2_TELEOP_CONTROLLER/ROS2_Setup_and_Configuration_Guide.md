# ROS 2 Setup and Node Creation for Raspberry Pi and Command Sending System

## Overview
This guide details the steps to set up ROS 2 on a Raspberry Pi and another system to send commands to control a robot. 
It covers installing ROS 2 Humble or Jazzy Jalisco, configuring the environment, and running ROS 2 nodes.



## 1. Installing ROS 2

### 1.1 ROS 2 Humble Installation
Follow the instructions to install ROS 2 Humble from the official documentation:  
[ROS 2 Humble Installation](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debs.html)

**Note:** ROS 2 Humble is not supported on Ubuntu 24.04. Instead, you can use Jazzy Jalisco, which is supported.

### 1.2 Container Solution for ROS 2
If you prefer a container-based solution, especially when using an unsupported Ubuntu version, consider using Docker:

- **ROS 2 Installation via Docker:**  
  [ROS 2 Docker Installation Guide](https://docs.ros.org/en/humble/Installation.html#:~:text=use%20a%20container%20solution%20to%20run%20ROS%202%20on%20your%20platform.)
  
- **Running ROS 2 Nodes in Docker:**  
  [Run 2 Nodes in Single or Separate Docker Containers](https://docs.ros.org/en/humble/How-To-Guides/Run-2-nodes-in-single-or-separate-docker-containers.html)

- **Available Docker Images:**  
  [Docker Hub ROS Images](https://hub.docker.com/_/ros/tags?page=&page_size=&ordering=&name=humble)

  Recommended Image:
  ```bash
  sudo docker pull ros:humble-perception-jammy
  ```

For more details, visit the [Docker Image](https://hub.docker.com/layers/library/ros/humble-perception-jammy/images/sha256-30cf486cd3f11948045fd18cdb6fa778338c142d5f7abe9ec32e9257530c377a?context=explore) page.

### 1.3 Installing Jazzy Jalisco Natively
If you need to install ROS 2 natively on Ubuntu 24.04, use Jazzy Jalisco:

- **Jazzy Jalisco Release Information:**  
  [Release Jazzy Jalisco](https://docs.ros.org/en/humble/Releases/Release-Jazzy-Jalisco.html)

- **Jazzy Jalisco Installation:**  
  [Jazzy Jalisco Installation Guide](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html)

- **Jazzy Jalisco Tutorials:**  
  [Jazzy Tutorials](https://docs.ros.org/en/jazzy/Tutorials.html)

- **Jazzy Jalisco Configuring environment:**

  [Jazzy Configuring environment](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools/Configuring-ROS2-Environment.html)

## 2. Configuring the ROS Environment

### 2.1 Setting Up the Environment
1. Configure the environment by adding the setup script to your shell configuration:
   ```bash
   echo "source /opt/ros/jazzy/setup.zsh" >> ~/.zshrc 
   ```
2. Verify the environment variables:
   ```bash
   printenv | grep -i ROS
   ```

### 2.2 Setting the `ROS_DOMAIN_ID`
1. Determine a unique `ROS_DOMAIN_ID` for your ROS 2 nodes. This should be an integer between 0 and 232-1.

   **Note:** By default, domain IDs 0-101 and 215-232 are safe to use without colliding with ephemeral ports.

   [About Domain ID](https://docs.ros.org/en/jazzy/Concepts/Intermediate/About-Domain-ID.html)

2. Set the `ROS_DOMAIN_ID` in your current shell session:
   ```bash
   export ROS_DOMAIN_ID=<your_domain_id>
   ```

3. To persist the `ROS_DOMAIN_ID` across sessions, add it to your `.bashrc`:
   ```bash
   echo "export ROS_DOMAIN_ID=<your_domain_id>" >> ~/.bashrc
   ```
4. Apply the changes:
   ```bash
   source ~/.bashrc
   ```
   

5. Verify the `ROS_DOMAIN_ID`:
   ```bash
   echo $ROS_DOMAIN_ID
   ```

   **Note:** The current `ROS_DOMAIN_ID` is 93.

### 2.3 Optional: The `ROS_AUTOMATIC_DISCOVERY_RANGE` Variable
ROS 2 communication is not limited to localhost by default. You can limit the ROS 2 discovery 
range by setting the `ROS_AUTOMATIC_DISCOVERY_RANGE` environment variable. This is useful in 
environments where multiple robots may publish to the same topic, causing conflicts.

## 3. Installing ROS Packages
Install the necessary ROS packages like **turtlesim** and **rqt** by following 
this [tutorial](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools/Introducing-Turtlesim/Introducing-Turtlesim.html).

## 4. SSH and Remote Configuration
To connect remotely to WSL via SSH, follow this guide:  
[WSL SSH Remote Access](https://medium.com/@wuzhenquan/windows-and-wsl-2-setup-for-ssh-remote-access-013955b2f421)

**Important:** Configure WSL properly for ROS 2 communication, as detailed in these resources:

- [ROS 2 WSL2 External Communication](https://answers.ros.org/question/413680/ros2-wsl2-external-communication/)
- [ROS 2 WSL YouTube Tutorial](https://www.youtube.com/watch?v=NOOUfsExYCE)

Make sure to **disable** the option "Allow management operating system to share this network adapter."

## 5. Adding Network Configuration Commands on System Startup

If you need to set a specific IP address and network configuration on system startup, you can create a 
custom systemd service:

### Creating a `restart-network.service`
1. Create the service file:
   ```bash
   sudo nano /etc/systemd/system/restart-network.service
   ```

2. Add the following content:
   ```ini
   [Unit]
   Description=Restart network with specific IP settings
   After=network.target

   [Service]
   Type=oneshot
   ExecStart=/usr/local/bin/set_static_ip.sh
   RemainAfterExit=yes

   [Install]
   WantedBy=multi-user.target
   ```

3. Create the network configuration script:
   ```bash
   sudo nano /usr/local/bin/set_static_ip.sh
   ```

4. Add the commands:
   ```bash
   #!/bin/bash
   ip addr flush dev eth0
   ip addr add <ip-adress>/<port> dev eth0
   ip link set dev eth0 up
   ip route add default via <route-adress> dev eth0
   ```

5. Make the script executable:
   ```bash
   sudo chmod +x /usr/local/bin/set_static_ip.sh
   ```

6. Enable and start the service:
   ```bash
   sudo systemctl daemon-reload
   sudo systemctl enable restart-network.service
   sudo systemctl start restart-network.service
   ```



## 6. Running ROS 2 Scripts on Raspberry Pi and the Command-Sending System

### 6.1 ROS 2 Script for Raspberry Pi
The script `serial_robot_controller.py` should run on the Raspberry Pi to control the robot:

#### Running the Script:
```bash
python3 serial_robot_controller.py
```

### 6.2 ROS 2 Script for Command-Sending System
The script `keyboard_teleop.py` should be executed on the system sending movement commands to the robot:

#### Running the Script:
```bash
python3 keyboard_teleop.py
```

### 6.3 Description of the Scripts:

#### `serial_robot_controller.py` (On Raspberry Pi)
This script subscribes to the `/cmd_vel` topic to receive movement commands (linear x, y, and angular z values) 
and sends corresponding joint movement commands to the robot over a serial connection.

#### `keyboard_teleop.py` (On Command-Sending System)
This script publishes commands to the `/cmd_vel` topic using the `geometry_msgs/Twist` message type based on 
keyboard input (`w`, `a`, `s`, `d` keys) to control the robot's movements.

Both scripts were tested and verified to work properly, with communication established between the Raspberry Pi and the command-sending system.



