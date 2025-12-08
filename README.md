# Bike Theft Prevention System

A proof-of-concept embedded system designed to deter bike theft, authenticate owners with biometrics, and provide location tracking after a theft event.

---

# Introduction

Our goal was to create a system for securing a bike that took less time to use than a traditional U- or cable lock. Bike theft is a serious problem on Middlebury campus, with it happening to people we know or ourselves often. At the same time, traditional locks are a hassle to use when moving around campus as much as the average student, and many students tend to not lock their bikes, making the issue worse. Out goal was to make a bike theft deterent that would require little work to operate, and strongly discourage stealing a bike. We determined the best solution to this problem would utilize a microcontroller to manage an array of theft deterent and retreival methods.

In order to lock and unlock the bike, we determined that a fingerprint scanner would be ideal for a quick and easy lock and unlock without having to manage a physical aspect. An accellerometer would detect when the bike was stolen, by measuring when the bike was moving while locked. Once the system determined a theft attempt was underway, it would activate a loud alarm, similar in volume to a car horn, and track the bike's location via an onboard GPS. An MQTT server would receive information from the device and display it to the owner. An OLED display would inform the user as to the lock status, battery, and provide information on the fingerprint registration system.

(put references here)

---

# Methods

## MCU

We used the Huzzah32 esp32 feather board by Adafruit due to its wifi capabilities and because it was the hardware that we already had on hand. We used the Platformio coding environment on Visual Studio Code to flash to the device, which controlled all of the periphreals. We use its I2C and SPI functionalities, allowing it to manage all of the devices, and it also manipulates the horn via an output pin that controls a MOSFET. 

---

## Periphreals

### Fingerprint Scanner



---

### Accellerometer

---

### Horn

The horn is intended to by supplied with a 12v power supply, as opposed to the 5v that the MCU expects. For this reason, we purchased a regulator and MOSFET, and purchased a 12v battery. The voltage is regulated down to 5v for the MCU, and the MCU controls the horn indirectly, via the MOSFET. The MOSFET allows the MCU to allow or disallow current to the horn without directly supplying the current at the level the horn expects.

---

### GPS

The GPS is connected via its RX pin to the MCU, which makes up half of an SPI connection. In the issues section this will be explained. As it stands, the GPS provides data in the form of coordinates.

---

### OLED Display

We decided on the Adafruit 128x64 OLED FeatherWing due to its ability to connect to the MCU easily, its simple input system, and modest graphical capabilities, being limited to black and white.

The display is used to give the user information, and its three input buttons are used for the user to communicate to the system. It physically acts as a wing to the MCU, sitting directly on it. It communicates via SPI, and uses the adafruit SH110X graphics drivers. The adafruit GFX library provides commands to display text, and images.

Typically, the monochrome screen is blank. When the A button is pressed, it displays the battery charge and lock/unlock status. The B button enrolls a fingerprint, assuming the device is unlocked. The C button displays the GPS status, giving the current connected satelite number and coordinates. When the device goes to sleep, the screen will become blank as it powers down.

We referenced the code provided by the device supplier to write our code for the display.¹

---

## Bill of Materials

| Item                | Supplier | Cost    | Store link |
|---------------------|----------|---------|------------|
| Huzzah32 MCU        | Adafruit | 21.95   | https://www.adafruit.com/product/3619 |
| Fingerprint sensor  | Adafruit | $39.95  | https://www.adafruit.com/product/4651 |
| Feather OLED        | Adafruit | $15.95  | https://www.adafruit.com/product/6313 |
| GPS Module          | Adafruit | $29.95  | https://www.adafruit.com/product/4279 |
| Car Horn            | Amazon   | $12.99  | https://www.amazon.com/dp/B08B4LTRJX  |
| Battery Pack        | Amazon   | $18.56  | https://www.amazon.com/dp/B09DP9D8Q4  |
| Accelerometer       | Adafruit | $4.95   | https://www.adafruit.com/product/2809 |
| Regulator           | Adafruit | $1.25   | https://www.adafruit.com/product/2165 |
| MOSFET              | Adafruit | $2.25   | https://www.adafruit.com/product/355  |
| **Total**           | —        | **$124.85** | |

---

# Results



---

# Accessibility

Although this product was designed with a bike in mind, there's no reason this product couldn't be used to secure any device that is intended to be secured. There are still some limitations in terms of accessibility, with the most prominent being the input device. The buttons are placed quite close together and labeled in very small text, so people with visual or motor disabilities may have trouble using it. Additionally, people with hearing disabilities would struggle to realize if they unintentionally set off the alarm, which may cause trouble for them or others in the area, given the volume of the horn.

---

# Ethical Implications

The volume of the horn is the most identifiable impact that the device could have on a local population, Although unlikely, there is a possibility that the device gets triggered when someone unlocks their bike and jostles the user's bike for an extended period of time. This would unnessecerily trigger a loud alarm. Whether unintentional or not, the horn going off may startle a thief and cause them harm if they crash the bike, and the local community may get aggrivated at the alarm sound.

---

# Schedule

Our original schedule was as follows:

Finger Print Authentication (~1 week)
  We will begin by ensuring our FPA functions. This will consist of connecting the Fingerprint Sensor to our MCU and configuring it to verify certain finger profiles.
Accelerometer (~1 week)
  We will need to configure the SPI communication to be able to read data from the accelerometer and determine when motion is detected
Horn & Battery configuration & wiring (~1 week)
  Involves wiring the BTEDD through the MOSFET to the battery pack as well as using the Huzzah board to control power to the BTEDD
Wire the regulator from the Huzzah to the battery (~1 week)
  We will next connect our Car Horn to the MCU. Since the car requires 12 volts, it will be connected through a MOSFET directly to the 12v battery
GPS detection (~1 week)
  We will connect a GPS to the MCU, which will be usually sleeping. In the case of a BTE, it shall awaken, and will begin transmitting location data to the user through Adafruit IO. We are assuming the nature of the bike thief to be another student, so we will rely on campus Wi-Fi.
Adafruit IO Dashboard (~1 week)
  We will use the IO dashboard example code to push GPS and other data to a dashboard we can view
OLED Display (~1 week)
    We will use a small OLED display compatible with our MCU to indicate locking status as well as facilitate adding and deleting finger profiles. An LED will also be used as a more visible armed/disarmed indicator (Danny)

Due to the limitation of time we had to complete the project, the actual schedule went as follows:

Accelerometer (2 days)
GPS detection (2 days)
Adafruit IO Dashboard (2 days)
Finger Print Authentication (3 days)
OLED Display (1 day)
Horn & Battery configuration & wiring (~1 week)
Wire the regulator from the Huzzah to the battery (1 day)

We worked mostly asynchroniously on different tasks, but this was the general order in which tasks were completed. We had multiple problems with the horn and battery configuration, detailed below.

---

# Issues

The first major issue we encountered was that the GPS module didn't directly have a TX and RX line, which make up an SPI connection interface. The model GPS we bought was intended to connect to a computer or rasberry pi via a USB-C port, and formatted its pin outputs in terms of the USB-C transmission protocol. We realized we had purchased the wrong model GPS, but we figured out that we could directly solder wires to the physical  GPS module, bypassing the conversion unit. Although the RX pin was soldered just fine, the delicate nature of the task resulted in the TX pin being ripped off of the module. This means that we can receive data from the GPS, which outputs constantly, but we can't send input to it. Ultimately, this doesn't affect the function of the product greatly.

When we integrated the code for the periphreals into the main code, it would output "invalid header" and get stuck in a boot loop. The same code functioned in Arduino IDE, so we had to do a lot of work to figure out why PlatformIO was struggling. The problem was with library versions, as in PlatformIO, we had the wrong library version. Arduino had the newest versions, so we had to manually update the libraries in PlatformIO.

Due to the large number of sensors that need to work at the same time, we encountered performance issues. We solved this by creating "task" modules, which are asynchronously ran, which doesn't block the main code from running.

Something about the horn.

---

# Future work

The form factor of our finished product is not ideal for actual applications. It works as a proof of concept, but ideally the device would fit inside the hollow internal tubes of the bike frame. As it stands, a neon box will realistically be a target for thieves.

As it stands, the user will have to access the Adafruit IO dashboard via the website, but ideally for ease of use we could port the data over to an app.

---

# References

1. Adafruit (2020) 128x64 OLED FeatherWing [Example code]. https://learn.adafruit.com/adafruit-128x64-oled-featherwing/arduino-code
