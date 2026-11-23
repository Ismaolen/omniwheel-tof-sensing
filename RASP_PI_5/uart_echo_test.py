import serial
import time

# Set up serial connection to the device at ttyS0 with a baudrate of 115200 and a timeout of 60 seconds.
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=60)

try:
    while True:
        ser.write(b'Hello, Microcontroller!\n') # Send a message to the microcontroller.
        time.sleep(1) # Wait for 1 second before the next action.
        if ser.in_waiting > 0: # Check if there is incoming data waiting to be read.
            # Read the incoming data and decode it from bytes to string.
            incoming_data = ser.readline().decode('utf-8').rstrip() 
            print("Received:", incoming_data)
except KeyboardInterrupt: # If a keyboard interrupt is detected (Ctrl+C), exit the loop.
    print("Programm beendet")
finally:
    ser.close() # Always close the serial connection when done.


'''
Here's a simplified and concise guide to configure Raspberry Pi and use Minicom:

1. **Disable Console Use on ttyS0**
- Open configuration: `sudo raspi-config`
- Go to `Interface Options` → `Serial Port`
- Choose `No` for "Would you like a login shell to be accessible over serial?"
    - Choose `Yes` for "Would you like the serial port hardware to be enabled?"
    - Exit the tool and restart the Raspberry Pi.

2. **Identify the Correct Serial Port**
- Use the command: `dmesg | grep tty` or `ls /dev/tty*`
- Typical results: `/dev/ttyUSB0`, `/dev/ttyACM0`

3. **Configure Minicom**
- Start Minicom with setup options: `sudo minicom -s`
- Navigate to:
- `A - Serial Device`: Set to `/dev/ttyACM0` or as per your connection
- `E - Bps/Par/Bits`: Set to `115200 8N1`
- `F - Hardware Flow Control`: Set to `No`
- `G - Software Flow Control`: Set to `No`
- Save the setup as default (`dfl`)
- Start Minicom with the new settings.

This guide should help to set up serial communication on Raspberry Pi using Minicom.
'''