import spidev
import struct
import sys
import time
import os

# 🔧 SPI initialisieren
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI1
spi.max_speed_hz = 30_000_000  # 10 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.mode = 0b00  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (STM32-Einstellung)
spi.lsbfirst = False  # MSB First (STM32 nutzt ebenfalls MSB First)
spi.bits_per_word = 16  # ✅ SPI ist jetzt 16-Bit

# 📌 SPI-Puffergröße (512 Werte = 1024 Bytes)
SPI_BUFFER_SIZE = 256
BYTE_BUFFER_SIZE = SPI_BUFFER_SIZE * 2  # 512 Bytes für 256x 16-Bit Werte

# 🔄 Anzahl der Testdurchläufe
TARGET_VALID_PACKETS = 1000
valid_packets = 0
invalid_packets = 0
incorrect_crc = 0
invalid_lenght = 0
invalid_null_packet = 0
duplicate_packets = 0
total_transfers = 0
previous_crc = 0

# ANSI Escape-Sequenzen für die Terminalsteuerung
CLEAR_SCREEN = "\033[2J\033[H"  # Bildschirm löschen
CURSOR_TOP = "\033[H"  # Cursor an den Anfang setzen

def crc16(data):
    """
    Berechnet die CRC-16 Prüfsumme mit dem Standardpolynom 0xA001.
    """
    crc = 0xFFFF
    for value in data:
        crc ^= value
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF

def spi_transfer(index):
    global previous_crc, valid_packets, invalid_packets, duplicate_packets, total_transfers, incorrect_crc, invalid_null_packet, invalid_lenght

    total_transfers += 1  # Jeder Versuch zählt

    time.sleep(0.0001)
    response = spi.xfer2([0] * BYTE_BUFFER_SIZE)
    # time.sleep(0.0001)

    if len(response) != BYTE_BUFFER_SIZE:
        invalid_lenght += 1
        return False

    distances = struct.unpack('<' + 'H' * SPI_BUFFER_SIZE, bytes(response))  # Little-Endian '<H'

    if all(v == 0 for v in distances):
        # time.sleep(0.0002)
        invalid_null_packet += 1
        return False

    received_crc = distances[160]
    calculated_crc = crc16(distances[:160])

    if received_crc != calculated_crc:
        incorrect_crc += 1
        return False

    if index == 0:
        previous_crc = calculated_crc
    elif previous_crc == calculated_crc:
        duplicate_packets += 1
        return False

    previous_crc = calculated_crc
    valid_packets += 1

    # **Bildschirm aktualisieren**
    sys.stdout.write(CLEAR_SCREEN)
    sys.stdout.write(CURSOR_TOP)
    sys.stdout.write("✅ [SPI Callback] SPI TX/RX abgeschlossen.\n\n")

    # 📥 Sensorwerte für Sensor 3 & 4 ausgeben
    sys.stdout.write("\t📥 Sensor 3\t\t|  📥 Sensor 4\n")
    sys.stdout.write("=" * 70 + "\n")

    for row in range(8):
        sensor_3_values = [f"{distances[32 + row * 8 + col]:4}" for col in range(8)]
        sensor_4_values = [f"{distances[96 + row * 8 + col]:4}" for col in range(8)]

        sys.stdout.write(
            "  ".join(sensor_3_values) + "  |  " + "  ".join(sensor_4_values) + "\n"
        )

    sys.stdout.write(f"\n📥 CRC-16 Checksumme: {distances[160]:04X}\n")
    sys.stdout.write("\n🔄 Starte nächste SPI DMA Übertragung...\n")
    sys.stdout.flush()
    # time.sleep(0.0005)
    return True









import time

# 📊 SPI Communication Test Overview
print("\n📊 SPI COMMUNICATION TEST OVERVIEW")
print("=" * 80)
print("🔍 This test evaluates the reliability and performance of SPI communication between a")
print("    Raspberry Pi and an STM32 microcontroller, using two VL53L5CX sensors (Sensor 3 & 4).")
print("\n📡 The test continuously transmits and receives data, verifying integrity using a CRC-16 checksum.")
print("=" * 80)

# 🏆 Key Metrics
print("\n📊 KEY METRICS DISPLAYED IN THE TEST:")
print("✅ Valid Packets: Successfully received and verified transmissions")
print("❌ Invalid Packets: Errors due to incorrect length, CRC mismatch, or null data")
print("⚠️ Duplicate Packets: Cases where the same packet was received multiple times")

# ⏱ Performance Metrics
print("\n⏱ PERFORMANCE METRICS:")
print("- Total number of SPI transfers")
print("- Average transmission time per valid packet")
print("- Total runtime for all transfers")

# 🏁 Test Conclusion
print("\n🛠 The test concludes with a pass/fail status, indicating whether all target packets")
print("    were successfully transmitted and received.")
print("=" * 80)

# 🕒 Wait 5 seconds before proceeding
time.sleep(7)














# 📌 Zeitmessung starten
start_time = time.time()

while valid_packets < TARGET_VALID_PACKETS:
    spi_transfer(valid_packets)

# 📌 Zeitmessung beenden
end_time = time.time()

# 🔍 Ergebnis ausgeben
total_time = end_time - start_time
average_time = total_time / valid_packets

sys.stdout.write(CLEAR_SCREEN)
sys.stdout.write(CURSOR_TOP)




# 📊 TESTERGEBNISSE: Test Protokoll
print("\n📊 TEST RESULTS - SPI Communication Test")
print("=" * 70)

# ⚙️ Test Configuration
print("\n🔧 TEST CONFIGURATION:")
print(f"   🔹 SPI Frequency: {spi.max_speed_hz / 1_000_000:.1f} MHz")
print(f"   🔹 SPI Mode: {spi.mode} (CPOL = {spi.mode & 0b10 >> 1}, CPHA = {spi.mode & 0b01})")
print(f"   🔹 Data Transmission: {spi.bits_per_word}-Bit, {'LSB First' if spi.lsbfirst else 'MSB First'}")
print(f"   🔹 Buffer Size: {SPI_BUFFER_SIZE} values ({BYTE_BUFFER_SIZE} bytes)")
print(f"   🔹 Target: {TARGET_VALID_PACKETS} valid packets\n")

invalid_packets = invalid_lenght + incorrect_crc + invalid_null_packet + duplicate_packets
# 📊 Summary of Results
print("=" * 70)
print("📊 RESULTS:")
print(f"   ✅ Successfully received packets: {valid_packets}/{TARGET_VALID_PACKETS}")
print(f"   ❌ Total faulty packets: {invalid_packets}")
print(f"      ⚠️ Invalid Length: {invalid_lenght}")
print(f"      ⚠️ CRC Errors: {incorrect_crc}")
print(f"      ⚠️ Null Packets: {invalid_null_packet}")
print(f"      ⚠️ Duplicate Packets: {duplicate_packets}")

# ⏱ Performance Analysis
print("\n⏱ PERFORMANCE:")
print(f"   🔄 Total Transfers: {total_transfers}")
print(f"   ⏳ Average Transmission Time per Valid Packet: {average_time * 1_000_000:.2f} µs")
print(f"   ⏱ Total Time for {valid_packets} Valid Packets: {total_time:.4f} seconds")

# 📊 Error Statistics

# 🏁 Final Report
print("=" * 70)
if valid_packets == TARGET_VALID_PACKETS:
    print("🏆 TEST PASSED: All target packets were successfully received!")
else:
    print("⚠️ TEST FAILED: Some packets were lost or contained errors!")
print("=" * 70)
