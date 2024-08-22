# 🩺 Medibox

## Overview

Medibox is a smart medication reminder system built using an ESP32 microcontroller. It helps users manage their medication schedule by setting alarms, monitoring environmental conditions, and displaying relevant information on an OLED screen.

## Features

- **Alarm System**: Set up to three customizable alarms to remind users to take their medication.
- **NTP Time Synchronization**: Automatically synchronizes time using an NTP server to ensure accuracy.
- **OLED Display**: Provides a simple interface for setting alarms, checking the time, and viewing notifications.
- **Temperature and Humidity Monitoring**: Continuously monitors the environment and alerts users if the conditions are outside the desired range.
- **Buzzer and LED Alerts**: Audible and visual notifications to ensure alarms and warnings are noticed.
- **User-Friendly Menu Navigation**: Easy-to-use buttons allow for navigating the menu, setting alarms, and configuring time zones.

![image](https://github.com/user-attachments/assets/0beca7b4-e5d2-4ddf-bfd8-0927920ab400)

## Getting Started

1. **Hardware Setup**: 
   - Assemble the Medibox by connecting the OLED display, DHT22 sensor, buzzer, LEDs, and push buttons to the ESP32. Ensure all components are correctly connected and powered.

2. **Software Setup**:
   - Install the necessary libraries such as Adafruit GFX, Adafruit SSD1306, and DHT Sensor.
   - Clone the repository and upload the code to your ESP32 using your preferred IDE.

3. **Wi-Fi Configuration**:
   - Ensure the device is connected to a Wi-Fi network for time synchronization.

## Usage

1. **Set Time Zone**:
   - Navigate through the menu to set your local time zone.

2. **Set Alarms**:
   - Set up to three alarms to remind you when it's time to take your medication.

3. **Environmental Monitoring**:
   - The Medibox will automatically monitor the temperature and humidity, providing alerts if conditions are not ideal.
  
---

# 🩺 Advanced Medibox

## Overview

The Advanced Medibox is designed to enhance the functionality of a medication management device. This version of Medibox integrates light intensity monitoring, dynamic light adjustment, and user-configurable settings to ensure optimal storage conditions for sensitive medicines. 

## Features

- **Light Intensity Monitoring**:
  - Utilizes two Light Dependent Resistors (LDRs) placed on either side of the Medibox.
  - Measures and displays the highest light intensity from the two LDRs on the Node-RED dashboard.
  - Includes a gauge for real-time highest intensity and a plot to visualize past variations.
  - Indicates which LDR (left or right) has the highest intensity.

- **Dynamic Light Adjustment**:
  - A shaded sliding window controlled by a servo motor adjusts the light intensity entering the Medibox.
  - Motor angle adjustment based on light intensity using the following formula:
    \[
    \theta = \min\{ \theta_{\text{offset}} \times D + (180 - \theta_{\text{offset}}) \times I \times \gamma, 180 \}
    \]
    Where:
    - \(\theta\) is the motor angle.
    - \(\theta_{\text{offset}}\) is the minimum angle (default: 30 degrees).
    - \(I\) is the light intensity (0 to 1).
    - \(\gamma\) is the controlling factor (default: 0.75).
    - \(D\) is 0.5 if the right LDR has max intensity, 1.5 if the left LDR has max intensity.

- **User-Configurable Settings**:
  - New group in the Node-RED dashboard to adjust the minimum angle and controlling factor.
  - Two slider controls:
    - **Minimum Angle Slider**: Ranges from 0 to 120 degrees.
    - **Controlling Factor Slider**: Ranges from 0 to 1.
  - Dropdown menu with options for commonly used medicines (e.g., Tablet A, B, C) and a custom option.
  - Custom option allows manual adjustment of minimum angle and controlling factor.

 ![image](https://github.com/user-attachments/assets/d4078d7e-d11d-4a90-ba86-74124dc784ba)

## Tech Stack

- **Hardware**: Arduino, LDRs, Servo Motor
- **Software**: Node-RED, Arduino IDE
- **Languages**: JavaScript (Node-RED), C++ (Arduino)

## Installation and Setup

1. **Clone the repository**:

   ```bash
   git clone https://github.com/yourusername/advanced-medibox.git
   cd advanced-medibox
   
2. **Setup Node-RED**:

   - Install Node-RED if not already installed. Follow [Node-RED installation instructions](https://nodered.org/docs/getting-started/).
   - Import the provided Node-RED flow file into your Node-RED instance.
     - Open Node-RED in your browser (default: [http://localhost:1880](http://localhost:1880)).
     - Click on the menu (top right) and select "Import".
     - Choose the flow file from this repository and click "Import" to add it to your workspace.
   - Configure the Node-RED flow to communicate with the Arduino and handle the light intensity monitoring and adjustment.

3. **Arduino Setup**:

   - Open the Arduino IDE and load the provided sketch file.
   - Connect the LDRs and servo motor to the Arduino according to the schematic provided in the repository.
   - Upload the sketch to the Arduino.

4. **Configure Environment**:

   - Edit the Node-RED flow to configure the communication between Node-RED and the Arduino.
   - Set up the light intensity monitoring, gauge, plot, and sliders as described in the features section.

## Usage

1. Access the Node-RED dashboard at [http://localhost:1880/ui](http://localhost:1880/ui).
2. Monitor real-time light intensity and view historical data.
3. Adjust the shaded sliding window settings using the sliders and dropdown menu.

## Future Enhancements

- Integration of additional sensors for temperature and humidity.
- Advanced data analytics for better medication management.
- Enhanced user interface with more customization options.

## Contributing

Feel free to open issues or submit pull requests if you'd like to contribute to this project.

## License

This project is licensed under the MIT License.

