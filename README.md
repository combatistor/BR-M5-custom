[![BR-M5 | Open-Source Bluetooth Remote for Canon Cameras](./.github/cover.png)](https://youtu.be/Gh5uEc2dNJM "BR-M5 | Open-Source Bluetooth Remote for Canon Cameras")

# BR-M5: An open-source Bluetooth Remote for Canon cameras

This Bluetooth remote aims to replicate Canon's BR-E1 behavior while adding key features like fully programable time-lapses that most Canon cameras lack!

Besides I added a custom remote where you can change the timer, the number of shots taken and the interval between each shot. This feature is implemented in lower end cameras (as custom remote) but absent from the higher ends cameras (like R5, R6).

<img src="https://github.com/combatistor/BR-M5-custom/blob/master/img/canon_remote.jpg" alt="canon custom remote" height="200"/>

Fortunately, *M5Stack* proposes a nice little inexpensive development board that perfectly fits our needs, the [M5Stick-Cplus](https://m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit?variant=35275856609444).

The BR-M5 has been primarily developed for the Canon M50 Mark I, which misses a descent timelapse mode that can do long exposures. The shutter speed option in M50's timelapse mode is indeed limited to 1/30s. Furthermore, the M50 doesn't have any shutter release port to plug classic intervalometers. No official Bluetooth, WiFi, or tethering tools currently offer time-lapse capabilities.

I followed up the new custom remote on my canon EOS R6 and it works perfectly!  
I added beeping sound and LED blinking to have an indication of the remaining time as it will not blink on the camera.

<img src="https://github.com/combatistor/BR-M5-custom/blob/master/img/remote_shooting.jpg" alt="custom remote shooting" width="100"/>

## Installation

* Install [Visual Studio Code](https://code.visualstudio.com/download) and the [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension.
* Clone this repository - ```git clone https://github.com/combatistor/BR-M5-custom``` -, open it as project in PlatformIO and upload - ```Ctrl+Alt+u``` - to a M5Stick-Cplus connected to your PC.

## How-To-Use

* Power on by pressing the left-side button and power off with a 6 seconds press on the same button.
* For the first connection with your camera you have to power on the stick and press the center button to pair your Canon camera with the remote, the display should show *Pairing in progress*.
* Once the camera and remote are paired, you don't need to follow the first connection step when turning the remote on, it should try to connect directly to the camera.
* In the code, you can change which mode is first displayed to the user when the connection is done: Timelapse or Remote
you can comment/uncomment the lines  
```current_shooting_mode = Shooting_remote;```  
```current_shooting_mode = Shooting_timelapse;```

You can always change the mode by pressing 2 times rapidly on the right-side button, then pressing on the right-side button to select the mode and validate using the center button

<img src="https://github.com/combatistor/BR-M5-custom/blob/master/img/remote_mode.jpg" alt="custom remote mode" width="100"/>

### Timelapse mode
* To change the interval, press the right-side button for half a second and use the same button to increase and the center button to decrease the timelapse interval. Once set, press the center button for half a second to validate.
* Start and stop time-lapses using the center button.

### Custom remote mode
* To change the settings, press the right-side button for half a second and use the same button to increase and the center button to decrease the timelapse interval.  
To change the other settings, press the right-side button for half a second again and it should change which setting you are modifying. Once set, press the center button for half a second to validate.  
<img src="https://github.com/combatistor/BR-M5-custom/blob/master/img/remote_settings.jpg" alt="custom remote settings" width="100"/>
* Start and/or stop the shooting using the center button.  
<img src="https://github.com/combatistor/BR-M5-custom/blob/master/img/remote_ready.jpg" alt="custom remote ready" width="100"/>

## To-do

- [x] Fix connection reliability issues.
- [x] Auto connection to camera when remote start-up.
- [ ] Add battery level indication.
- [ ] Enable/Disable beep sound.
- [ ] Enable/Disable LED.

Feel free to contribute!
