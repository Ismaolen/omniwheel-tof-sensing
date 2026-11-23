import spidev
import time
from gpiozero import OutputDevice

# 🔧 SPI initialisieren
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI0, CS0 (STM32 Slave ist hier angeschlossen)
spi.max_speed_hz = 30_000_000  # 1 MHz SPI-Geschwindigkeit (STM32 muss diese unterstützen!)
spi.mode = 0b01  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (STM32-Einstellung)
spi.lsbfirst = False  # MSB First (STM32 nutzt ebenfalls MSB First)
spi.bits_per_word = 16
spi.no_cs
# 📌 NSS (Chip Select) über GPIO25 (Pin 22) steuern
# NSS = OutputDevice(25)  # GPIO25 (Pin 22) für softwaregesteuertes NSS
# NSS.on()  # Standardmäßig deaktiviert (HIGH)

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
    # NSS.off()  # Chip Select aktivieren (LOW)
    time.sleep(0.00001) # Stabilitätspause für SPI
    # time.sleep(0.01) # Stabilitätspause für SPI

    # 🛠 Synchronisations-Dummy-Read (wichtig für SPI-DMA!)
    # dummy_read = spi.xfer2([0x00] * response_size)

    # 🛠 Befehl auf `response_size` auffüllen (Padding mit \0)
    tx_data = command.ljust(response_size, '\0').encode()

    # 📤 Debugging: Zeige gesendete Daten
    # print(f"📤 TX-Daten: {[hex(b) for b in tx_data]}")

    # 🛠 SPI Full-Duplex Übertragung
    response = spi.xfer2(list(tx_data))

    time.sleep(0.00001)  # Nach der Übertragung kurz warten
    # NSS.on()  # Chip Select deaktivieren (HIGH)

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
    response = spi_transfer(current_command, 128)  # 64 Byte Antwort (angepasst an STM32 TX_BUFFER_SIZE)

    # 📝 Empfangene Antwort ausgeben
    print(f"🔄 Antwort vom STM32: {response}")

    # Zum nächsten Befehl wechseln
    command_index = (command_index + 1) % len(COMMANDS)

    time.sleep(0.00001)  # Wartezeit zwischen den Anfragen
