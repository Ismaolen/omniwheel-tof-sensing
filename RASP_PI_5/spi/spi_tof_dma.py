import spidev
import time
import struct
import sys

# 🔧 SPI initialisieren
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI0, CS0
spi.max_speed_hz = 10_000_000  # 1 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.mode = 0b00  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (STM32-Einstellung)
spi.lsbfirst = False  # MSB First (STM32 nutzt ebenfalls MSB First)
spi.bits_per_word = 16  # ✅ SPI ist jetzt 16-Bit

# 📌 SPI-Puffergröße (128 Werte = 256 Bytes)
SPI_BUFFER_SIZE = 128
BYTE_BUFFER_SIZE = SPI_BUFFER_SIZE * 2  # 256 Bytes für 128x 16-Bit Werte

# 🔄 Anzahl der Messdurchläufe
LOOP_COUNT = 100000


def spi_transfer():
    """
    Führt einen SPI-Transfer durch:
    Empfängt eine 8x8 Matrix pro Sensor (2 Sensoren = 128 Werte, je 2 Byte)
    """
    response = spi.xfer2([0x0000] * BYTE_BUFFER_SIZE)  # 256 Bytes senden & empfangen
    if len(response) != BYTE_BUFFER_SIZE:
        print(f"❌ Fehler: Empfangene Datenmenge ungleich {BYTE_BUFFER_SIZE} Bytes, erhalten: {len(response)}")
        return

    # **Jetzt die Bytes als 16-Bit Werte dekodieren**
    distances = struct.unpack('<' + 'H' * SPI_BUFFER_SIZE, bytes(response))  # Little-Endian '<H'

    # **Terminal-Ausgabe aktualisieren ohne Scrollen**
    sys.stdout.write("\033[H\033[J")  # ANSI Escape-Sequenz: Clear Screen + Cursor nach oben

    print("📡 Empfangene Distanzwerte (Sensor 1 & Sensor 2):\n")

    for i in range(8):  # 8x8 Matrix pro Sensor
        sensor_1 = "  ".join(f"{distances[i*8 + j]:4d}" for j in range(8))  # Links: Sensor 1
        sensor_2 = "  ".join(f"{distances[64 + i*8 + j]:4d}" for j in range(8))  # Rechts: Sensor 2
        print(f"{sensor_1}    ||    {sensor_2}")  # Sensoren nebeneinander ausgeben

    print("\n")
    sys.stdout.flush()  # Sofortige Aktualisierung





# 📌 Zeitmessung starten
start_time = time.time()

for i in range(LOOP_COUNT):
    spi_transfer()
    time.sleep(0.0001)  # 1ms Pause zwischen Anfragen

# 📌 Zeitmessung beenden
end_time = time.time()

# 🔍 Ergebnis ausgeben
total_time = end_time - start_time
average_time = total_time / LOOP_COUNT

print(f"\n⏱️  Gesamtzeit für {LOOP_COUNT} SPI-Transfers: {total_time:.4f} Sekunden")
print(f"⏳ Durchschnittliche Übertragungszeit pro Transfer: {average_time:.6f} Sekunden")

