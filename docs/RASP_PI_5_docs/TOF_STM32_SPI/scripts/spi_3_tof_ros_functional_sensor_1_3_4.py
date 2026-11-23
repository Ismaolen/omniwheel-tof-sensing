import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import spidev
import struct
import time

SPI_BUFFER_SIZE = 256
BYTE_BUFFER_SIZE = SPI_BUFFER_SIZE * 2  # 512 Bytes für 256x 16-Bit Werte


class SPISensorNode(Node):
    def __init__(self):
        super().__init__("spi_sensor_node")

        # SPI initialisieren
        self.spi = self.init_spi()

        # Publisher für Sensordaten
        self.sensor_1_pub = self.create_publisher(String, "sensor_1_data", 10)
        self.sensor_3_pub = self.create_publisher(String, "sensor_3_data", 10)
        self.sensor_4_pub = self.create_publisher(String, "sensor_4_data", 10)

        self.previous_crc = 0  # Speichert den letzten CRC-Wert

    def init_spi(self):
        """Initialisiert SPI mit den richtigen Einstellungen."""
        spi = spidev.SpiDev()
        spi.open(0, 0)  # SPI1 auf Raspberry Pi
        spi.max_speed_hz = 10_000_000  # 30 MHz
        spi.mode = 0b00  # CPOL = 0, CPHA = 0
        spi.lsbfirst = False  # MSB First
        spi.bits_per_word = 16  # 16-Bit Übertragung
        return spi

    def crc16(self, data):
        """ Berechnet CRC-16 Prüfsumme. """
        crc = 0xFFFF
        for value in data:
            crc ^= value
            for _ in range(8):
                if crc & 1:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc & 0xFFFF

    def spi_transfer(self):
        """Führt SPI-Transfer durch und veröffentlicht Daten nur bei Änderung."""
        while rclpy.ok():  # Solange ROS 2 aktiv ist
            try:
                # SPI-Übertragung (Leeren TX-Puffer senden, um RX-Daten zu erhalten)
                response = self.spi.xfer2([0] * BYTE_BUFFER_SIZE)

                # Prüfen, ob gültige Daten empfangen wurden
                if len(response) != BYTE_BUFFER_SIZE:
                    continue

                distances = struct.unpack('<' + 'H' * SPI_BUFFER_SIZE, bytes(response))

                # Falls alle Werte 0 sind, überspringen
                if all(v == 0 for v in distances):
                    # time.sleep(0.002)  # 2 ms warten und nochmal versuchen
                    continue

                received_crc = distances[224]
                calculated_crc = self.crc16(distances[:224])

                # Falls CRC falsch, Daten ignorieren
                if received_crc != calculated_crc:
                    # self.get_logger().error(f"CRC-Fehler! Erwartet: {calculated_crc:04X}, Erhalten: {received_crc:04X}")
                    continue

                # Falls die Daten doppelt sind, überspringen
                # if self.previous_crc == calculated_crc:
                #    continue

                # Speichert neuen CRC-Wert
                self.previous_crc = calculated_crc

                # Daten veröffentlichen
                self.publish_sensor_data(distances)

                # time.sleep(0.0005)  # Kurze Pause, um nicht unnötig viele Updates zu machen

            except Exception as e:
                self.get_logger().error(f"SPI-Fehler: {e}")

    def publish_sensor_data(self, distances):
        """Veröffentlicht Sensordaten nur, wenn neue valide Werte vorhanden sind."""
        sensor_1_values = [str(distances[32 + i]) for i in range(64)]
        sensor_3_values = [str(distances[96 + i]) for i in range(64)]
        sensor_4_values = [str(distances[160 + i]) for i in range(64)]

        sensor_1_msg = String()
        sensor_1_msg.data = ",".join(sensor_1_values)
        self.sensor_1_pub.publish(sensor_1_msg)

        sensor_3_msg = String()
        sensor_3_msg.data = ",".join(sensor_3_values)
        self.sensor_3_pub.publish(sensor_3_msg)

        sensor_4_msg = String()
        sensor_4_msg.data = ",".join(sensor_4_values)
        self.sensor_4_pub.publish(sensor_4_msg)

        self.get_logger().info(f"📡 Sensor 3 & 4 Daten veröffentlicht!")


def main(args=None):
    rclpy.init(args=args)
    node = SPISensorNode()

    try:
        node.spi_transfer()  # SPI-Verarbeitung läuft in der ROS 2-Schleife
    except KeyboardInterrupt:
        node.get_logger().info("Knoten beendet.")
    finally:
        node.spi.close()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
