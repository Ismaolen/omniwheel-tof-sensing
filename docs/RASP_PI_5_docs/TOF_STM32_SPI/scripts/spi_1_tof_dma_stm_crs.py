import spidev
import struct
import sys
import time
import os

# 🔧 SPI initialisieren
spi = spidev.SpiDev()
spi.open(1, 0)  # SPI2
# spi.max_speed_hz = 30_000_000  # 10 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.max_speed_hz = 30_000_000  # 30 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.mode = 0b00  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (STM32-Einstellung)
spi.lsbfirst = False  # MSB First (STM32 nutzt ebenfalls MSB First)
spi.bits_per_word = 16  # ✅ SPI ist jetzt 16-Bit
spi.no_cs = True
# 📌 SPI-Puffergröße (256 Werte = 512 Bytes)
SPI_BUFFER_SIZE = 256
BYTE_BUFFER_SIZE = SPI_BUFFER_SIZE * 2  # 512 Bytes für 256x 16-Bit Werte

# 🔄 Anzahl der Messdurchläufe
LOOP_COUNT = 10000

previous_crc = 0


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
    """
    Führt einen SPI-Transfer durch:
    Empfängt eine 8x8 Matrix pro Sensor (2 Sensoren = 128 Werte, je 2 Byte),
    mit zusätzlichen Nullzeilen am Anfang und einer CRC-Prüfsumme am Ende.
    """
    global previous_crc

    # 📤 Leeren TX-Puffer senden, um RX-Daten vom STM32 zu erhalten

    # time.sleep(0.00001)  # 1 ms warten

    print("was")
    response = spi.xfer2([0] * BYTE_BUFFER_SIZE)
    print(response)
    # time.sleep(0.00001)  # 1 ms warten
    # Prüfen, ob die empfangene Datenmenge korrekt ist
    if len(response) != BYTE_BUFFER_SIZE:
        print(f"❌ Fehler: Empfangene Datenmenge ungleich {BYTE_BUFFER_SIZE} Bytes, erhalten: {len(response)}")
        return False

    # **Jetzt die Bytes als 16-Bit Werte dekodieren**
    distances = struct.unpack('<' + 'H' * SPI_BUFFER_SIZE, bytes(response))  # Little-Endian '<H'

    if all(v == 0 for v in distances):
        print(f"❌ Fehler: Empfangene Datenmenge NULL")
        time.sleep(0.0002)  # 2 ms warten und nochmal versuchen
        # return False

    # ✅ CRC-16 Prüfsumme überprüfen
    received_crc = distances[160]  # CRC befindet sich an Index 160
    calculated_crc = crc16(distances[:160])  # CRC wird über die ersten 160 Werte berechnet

    # if received_crc != calculated_crc:
    # return False  # Falls die Prüfsumme nicht übereinstimmt, ignorieren

    if index == 0:
        previous_crc = calculated_crc

    elif previous_crc == calculated_crc:
        return False

    previous_crc = calculated_crc

    if received_crc == calculated_crc:
        print("✅ CRC-Check erfolgreich!")
    else:
        print(f"❌ CRC-Fehler! Erwartet: {calculated_crc:04X}, Erhalten: {received_crc:04X}")

    # **Terminal-Ausgabe ohne Clear-Screen**
    print("\n✅ [SPI Callback] SPI TX/RX abgeschlossen.")

    # 📥 Empfangene RX-Daten anzeigen (Hex-Werte wie STM32)
    print("📥 Empfangene RX-Daten:")

    # Zeige die ersten zwei Zeilen mit Nullen an
    print(" ".join(f"{distances[i]:04X}" for i in range(16)))
    print(" ".join(f"{distances[i]:04X}" for i in range(16, 32)))

    print("\n📥 Matrix 1 (Sensor 3):")
    for i in range(32, 96, 16):  # Matrix 1 (64 Werte)
        print(" ".join(f"{distances[i + j]:04X}" for j in range(16)))

    print("\n📥 Matrix 2 (Sensor 4):")
    for i in range(96, 160, 16):  # Matrix 2 (64 Werte)
        print(" ".join(f"{distances[i + j]:04X}" for j in range(16)))

    print(f"\n📥 CRC-16 Checksumme: {distances[160]:04X}\n")

    print("\n🔄 Starte nächste SPI DMA Übertragung...\n")

    sys.stdout.flush()  # Sofortige Aktualisierung


# ANSI-Escape-Sequenzen für die Terminalsteuerung
CLEAR_SCREEN = "\033[2J\033[H"  # Bildschirm löschen und Cursor an den Anfang setzen
CURSOR_TOP = "\033[H"  # Cursor an den Anfang setzen


def spi_transfer_table():
    """
    Führt einen SPI-Transfer durch:
    Empfängt zwei 8x8 Matrizen (eine pro Sensor) und zeigt sie in einer
    formatierten Tabelle nebeneinander an.
    Aktualisiert das Terminal an Ort und Stelle.
    """
    index = 0
    global previous_crc

    while True:
        # 📤 Leeren TX-Puffer senden, um RX-Daten vom STM32 zu erhalten
        # time.sleep(0.001)  # 1 ms warten
        # time.sleep(0.0001)  # 1 ms warten
        response = spi.xfer2([0] * BYTE_BUFFER_SIZE)
        # time.sleep(0.0001)  # 1 ms warten
        # time.sleep(0.001)  # 1 ms warten

        # Prüfen, ob die empfangene Datenmenge korrekt ist
        if len(response) != BYTE_BUFFER_SIZE:
            continue  # Falls das Paket unvollständig ist, überspringen

        # **Jetzt die Bytes als 16-Bit Werte dekodieren**
        distances = struct.unpack('<' + 'H' * SPI_BUFFER_SIZE, bytes(response))  # Little-Endian '<H'

        # Falls das gesamte Paket nur aus Nullen besteht, ignorieren
        if all(v == 0 for v in distances):
            # time.sleep(0.0002)  # 2 ms warten und nochmal versuchen
            time.sleep(0.002)  # 2 ms warten und nochmal versuchen
            continue

        # ✅ CRC-16 Prüfsumme überprüfen
        received_crc = distances[160]  # CRC befindet sich an Index 160
        calculated_crc = crc16(distances[:160])  # CRC wird über die ersten 160 Werte berechnet
        # print(f"\n📥 CRC-16 Checksumme: {distances[160]:04X}\n")
        # print(f"\n📥 CRC-16 erwartete Checksumme: {calculated_crc:04X}\n\n")
        if received_crc != calculated_crc:
            continue  # Falls die Prüfsumme nicht übereinstimmt, ignorieren

        if index == 0:
            previous_crc = calculated_crc

        elif previous_crc == calculated_crc:
            continue

        previous_crc = calculated_crc

        # **Bildschirm aktualisieren**

        sys.stdout.write(CLEAR_SCREEN)  # Bildschirm löschen
        sys.stdout.write(CURSOR_TOP)  # Cursor an den Anfang setzen
        sys.stdout.write("✅ [SPI Callback] SPI TX/RX abgeschlossen.\n\n")

        # 📥 Sensorwerte als 8x8 Matrix ausgeben
        sys.stdout.write("\t\t📥 Sensor 4       \t\t|      \t\t📥 Sensor 3\n")
        sys.stdout.write("=" * 98 + "\n")

        for row in range(8):  # 8 Zeilen für die 8x8 Matrix
            sensor_2_values = [f"{distances[32 + row * 8 + col]:4}" for col in range(8)]
            sensor_1_values = [f"{distances[96 + row * 8 + col]:4}" for col in range(8)]

            sys.stdout.write("  ".join(sensor_1_values) + "  |  " + "  ".join(sensor_2_values) + "\n")

        # 📥 CRC-16 Prüfsumme anzeigen
        sys.stdout.write(f"\n📥 CRC-16 Checksumme: {distances[160]:04X}\n")

        # 🔄 Nächste SPI DMA Übertragung
        sys.stdout.write("\n🔄 Starte nächste SPI DMA Übertragung...\n")

        sys.stdout.flush()  # Sofortige Aktualisierung

        # time.sleep(0.00005)  # Kurze Pause, um nicht unnötig viele Updates zu machen
        time.sleep(0.0005)  # Kurze Pause, um nicht unnötig viele Updates zu machen


# 📌 Zeitmessung starten
start_time = time.time()

for i in range(LOOP_COUNT):
    # if spi_transfer(i) == False:
    #   LOOP_COUNT = LOOP_COUNT + 1
    print("ww")
    spi_transfer_table()
    time.sleep(0.00001)  # 100ms Pause zwischen Anfragen
    # time.sleep(0.001)  # 100ms Pause zwischen Anfragen

print(LOOP_COUNT)
# 📌 Zeitmessung beenden
end_time = time.time()

# 🔍 Ergebnis ausgeben
total_time = end_time - start_time
average_time = total_time / LOOP_COUNT

print(f"\n⏱️  Gesamtzeit für {LOOP_COUNT} SPI-Transfers: {total_time:.4f} Sekunden")
print(f"⏳ Durchschnittliche Übertragungszeit pro Transfer: {average_time:.6f} Sekunden")
