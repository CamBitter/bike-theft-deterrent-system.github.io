# Bike Theft Prevention System

A proof-of-concept embedded system designed to deter bike theft, authenticate owners with biometrics, and provide location tracking after a theft event.

---

# Introduction

Our goal was to create a system for securing a bike that took less time to use than a traditional U- or cable lock. Bike theft is a serious problem on Middlebury campus, with it happening to people we know or ourselves often. At the same time, traditional locks are a hassle to use when moving around campus as much as the average student, and many students tend to not lock their bikes, making the issue worse. Out goal was to make a bike theft deterent that would require little work to operate, and strongly discourage stealing a bike. We determined the best solution to this problem would utilize a microcontroller to manage an array of theft deterent and retreival methods.

In order to lock and unlock the bike, we determined that a fingerprint scanner would be ideal for a quick and easy lock and unlock without having to manage a physical aspect. An accellerometer would detect when the bike was stolen, by measuring when the bike was moving while locked. Once the system determined a theft attempt was underway, it would activate a loud alarm, similar in volume to a car horn, and track the bike's location via an onboard GPS. An MQTT server would receive information from the device and display it to the owner. An OLED display would inform the user as to the lock status, battery, and provide information on the fingerprint registration system.

(put references here)

---

# Methods

## Components

### Fingerprint Scanner

---

### Accellerometer

---

### Horn

---

### GPS

---

### OLED Display

---

## Bill of Materials
