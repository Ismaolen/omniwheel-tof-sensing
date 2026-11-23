import spidev
import time
from gpiozero import OutputDevice

# SPI initialisieren
spi = spidev.SpiDev()
spi.open(0, 0)  # SPI0, CS0
spi.max_speed_hz = 1000000  # SPI-Geschwindigkeit (1 MHz)

# WICHTIGE KONFIGURATION für CPOL & CPHA, um STM32 zu matchen:
spi.mode = 0  # CPOL = 0, CPHA = 0 → SPI_MODE_0 (entspricht CPOL_LOW und CPHA_1EDGE)

spi.lsbfirst = False  # MSB First setzen

# NSS (Chip Select) über GPIO25 (Pin 22) steuern
NSS = OutputDevice(25)  # GPIO25 ist Pin 22
NSS.on()  # Standardmäßig deaktiviert (HIGH)

def spi_transfer(send_data):
    """ Sendet Daten an den STM32 und empfängt die Antwort """
    NSS.off()  # NSS aktivieren (LOW)
    time.sleep(0.002)  # Stabilitätspause
    response = spi.xfer2(send_data + [0x00] * len(send_data))  # Full-Duplex
    time.sleep(0.002)  # Stabilitätspause
    NSS.on()  # NSS deaktivieren (HIGH)
    return response

while True:
    print("\n📡 Sende Daten an STM32...")
    send_data = [0x50, 0xD9, 0x61, 0xEA]  # Beispiel-Daten
    response = spi_transfer(send_data)

    print(f"🔄 Antwort vom STM32: {[hex(b) for b in response]}")

    time.sleep(0.5)  # Wartezeit zwischen den Übertragungen
