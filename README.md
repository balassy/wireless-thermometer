# Wireless thermometer (IoT device) - Work in Progress!

A network connected thermometer implemented with ESP8266 and DHT22.

## Over-the-air updates (OTA)

ESP8266 [supports](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html) over the air updates out of the box, so you can load the firmware to the ESP using Wi-Fi connection rather than a serial port.

### Security disclaimer

Make sure you read the [Security disclaimer](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#security-disclaimer) and other security related sections in the official documentation to understand the risks!


### Prerequisites
- [Python 2.7](https://www.python.org/) (v3 is not supported yet). Make sure you check the _Add python.exe to Path_ option during installation on Windows.
- [Arduino IDE](https://www.arduino.cc/en/Main/Software) 1.6.7+

### Steps

1. Customize the parameters in `config.h`:
  - The `OTA_UPDATE_HOSTNAME` parameter defines the name for your device as it should appear during the update, for example in the Arduino IDE.
  - The `OTA_UPDATE_PASSWORD` parameter defines a secret that is requested by the device to allow loading a new firmware. Make sure you set a strong password! If it is set to `NULL`, then anyone can modify the code on your device who has access to your wireless network. Note: Visual Studio Code does not support OTA with password yet.
2. Load the initial version of the code to the ESP through USB.
3. **Important:** Reset the device!
4. Figure out the IP address of your device.
5. Start or restart Arduino IDE, and load the new version of your code.
6. In the **Tools** menu change **Port** to **SmarTherm at 192.168.0.111** (it will vary based on the name you set in `config.h` and the IP address of your device).
7. Click **Upload**. Arduino IDE will ask for the password you set in `config.h`, because you did set one, right?

### Over-the-air updates from Visual Studio Code

Visual Studio Code has [limited support](https://github.com/Microsoft/vscode-arduino/issues/359) for OTA updates:

- It does not support passwords for OTA, so you have two options:
  - Be really secure, set a password, and use the Arduino IDE for OTA updates.
  - Set the password to `NULL` in `config.h`, and use VS Code for your pleasure. Make sure that in this way you add just another insecure IoT device into the existing world of insecure IoT devices.

- VS Code is not able to discover devices on your network, so you can't conveniently select the IP address in the Port menu. To resolve this, you have to manually replace the serial port with the IP address of your device in `arduino.json`. For example:

```
{
    ...
    "port": "192.168.0.107",
    ...
}
```

### Troubleshooting

#### Connection failure

You may receive the following error after starting an OTA update:

```
Start updating sketch
Connect Failed
Error[2]: Connect Failed
Error[4]: End Failed
ERROR[0]: No Error
```

During OTA update your ESP device tries to connect to your computer to download the update. To enable this make sure that you have a hole in your firewall, and your antivirus software (e.g. ESET) does not block this kind of requests.

#### Python.exe is not found

You may receive the following error after starting as OTA update, even if you have selected to add python.exe to the Path during installation on Windows:

```
Cannot run program "python.exe": CreateProcess error=2, The system cannot find the file specified
```

This error may come in VS Code even if OTA upload works successfully in Arduino IDE.

The solution is to edit `platform.txt` in the `C:\Users\<YOURNAME>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.2` folder, and set the full path to `python.exe`:

```
tools.esptool.network_cmd.windows=C:/Python27/python.exe
```

#### Monitoring the upload

During upload your ESP device will send debug log messages through the serial port. So if OTA does not work, connect your device to your computer, start a serial monitoring tool (e.g. [Termite](https://www.compuphase.com/software_termite.htm)), and start the OTA update through the wireless network.

You can customize the messages in the `ota-updater.cpp` file.

## About the author

This project is maintained by [György Balássy](https://linkedin.com/in/balassy).