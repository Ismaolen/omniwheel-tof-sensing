import spidev
import time
from gpiozero import OutputDevice                                                                                                           # 🔧 SPI initialisieren
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI0, CS0 (STM32 Slave ist hier angeschlossen)
spi.max_speed_hz = 1000000  # 1 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.mode = 0  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (STM32-Einstellung)
spi.lsbfirst = False  # MSB First (STM32 nutzt ebenfalls MSB First)

# 📌 NSS (Chip Select) über GPIO25 (Pin 22) steuern
NSS = OutputDevice(25)  # GPIO25 (Pin 22) für softwaregesteuertes NSS
NSS.on()  # Standardmäßig deaktiviert (HIGH)

# 🛠 Liste der Befehle, die an den STM32 gesendet werden
COMMANDS = [
    "GET_DATAGET_DATAGET_DATAGET_DATA",
    "STATUSSTATUSSTATUSSTATUS",
    "RESETRESETRESETRESET",
    "TEMPTEMPTEMPTEMP",
    "VOLTAGEVOLTAGEVOLTAGEVOLTAGE"
]

def spi_transfer(command, response_size):
    """
    Führt einen SPI-Transfer durch:
    - command: Befehl als String (max. 64 Bytes)
    - response_size: Erwartete Antwortgröße in Bytes
    """
    NSS.off()  # Chip Select aktivieren (LOW)
    time.sleep(0.002)  # Stabilitätspause für SPI

    # 🛠 Synchronisations-Dummy-Read (wichtig für SPI-DMA!)
    dummy_read = spi.xfer2([0x00] * response_size)

    # 🛠 Befehl auf `response_size` auffüllen (Padding mit \0)
    tx_data = command.ljust(response_size, '\0').encode()

    # 📤 Debugging: Zeige gesendete Daten
    print(f"📤 TX-Daten: {[hex(b) for b in tx_data]}")

    # 🛠 SPI Full-Duplex Übertragung
    response = spi.xfer2(list(tx_data))

    time.sleep(0.002)  # Nach der Übertragung kurz warten
    NSS.on()  # Chip Select deaktivieren (HIGH)

    # 🔍 Debugging: Empfangene Rohdaten als HEX anzeigen
    print(f"🛠️ Rohdaten empfangen: {[hex(b) for b in response]}")

    try:
        decoded_response = bytes(response).decode('utf-8', errors='replace').strip('\x00')  # Nullbytes entfernen
    except UnicodeDecodeError as e:
        print(f"❌ Fehler beim Dekodieren: {e}")
        decoded_response = "⚠️ Datenfehler"

    return decoded_response if decoded_response else "⚠️ Keine Daten empfangen"

# 🔄 Endlosschleife, um Befehle an den STM32 zu senden
command_index = 0  # Zähler für Befehlsrotation

while True:
    # 📌 Aktuellen Befehl aus der Liste auswählen
    current_command = COMMANDS[command_index]
    print(f"\n📡 Sende Kommando an STM32: {current_command}")

    # 🛠 SPI-Übertragung ausführen
    response = spi_transfer(current_command, 64)  # 64 Byte Antwort (angepasst an STM32 TX_BUFFER_SIZE)

    # 📝 Empfangene Antwort ausgeben
    print(f"🔄 Antwort vom STM32: {response}")

    # Zum nächsten Befehl wechseln
    command_index = (command_index + 1) % len(COMMANDS)

    time.sleep(1)  # Wartezeit zwischen den Anfragen
















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

for i in range(LOOP_COUNT):
    spi_transfer_table()
    time.sleep(0.00001)  # 100ms Pause zwischen Anfragen


