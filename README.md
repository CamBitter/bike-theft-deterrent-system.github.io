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

## Periphreals

### Fingerprint Scanner

---

### Accellerometer

---

### Horn

---

### GPS

---

### OLED Display

The display is used to give the user information, and its three input buttons are used for the user to communicate to the system. It physically acts as a wing to the MCU, sitting directly on it. It communicates via SPI, and uses the adafruit SH110X graphics drivers. The adafruit GFX library provides commands to display text, and images.

Typically, the monochrome screen is blank. When the A button is pressed, it displays the battery charge and lock/unlock status. The B button enrolls a fingerprint, assuming the device is unlocked. The C button displays the GPS status, giving the current connected satelite number and coordinates. When the device goes to sleep, the screen will become blank as it powers down.

We referenced the code provided by the device supplier to write our code for the display.

---

## Bill of Materials

| Item                | Supplier | Cost    |
|---------------------|----------|---------|
| Huzzah32 MCU        | Adafruit | 21.95   |
| Fingerprint sensor  | Adafruit | $39.95  |
| Feather OLED        | Adafruit | $15.95  |
| GPS Module          | Adafruit | $29.95  |
| Car Horn            | Amazon   | $12.99  |
| Battery Pack        | Amazon   | $18.56  |
| Accelerometer       | Adafruit | $4.95   |
| Regulator           | Adafruit | $1.25   |
| MOSFET              | Adafruit | $2.25   |
| **Total**           | —        | **$124.85** |

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

---

# Issues

---

# Future work

---

# references

Adafruit (2020) 128x64 OLED FeatherWing [Example code]. https://learn.adafruit.com/adafruit-128x64-oled-featherwing/arduino-code
