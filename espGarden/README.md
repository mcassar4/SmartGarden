# ESP Garden Setup Guide

This guide provides step-by-step instructions on setting up the ESP-IDF environment in Visual Studio Code for the `espGarden` project. Follow these instructions to prepare your development environment.

## Prerequisites

This setup pretains to using the vscode extension, but may be done using the command line tools. See [How to Install Windows](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html) or [How to Install Linux](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html) for more information if you want to install a custom configuration. 

Otherwise, before you begin, ensure you have Visual Studio Code installed on your machine. If not, download and install it from [Visual Studio Code](https://code.visualstudio.com/).

## Step 1: Install the Visual Studio Code Extension

1. Open Visual Studio Code.
2. Go to the Extensions view by clicking on the square icon on the sidebar or pressing `Ctrl+Shift+X`.
3. Search for **ESP-IDF**.
4. Click on the install button next to the extension.

## Step 2: Download Required Drivers

Download and install the drivers needed for the serial port communication with your ESP device. This step is crucial for the proper detection of the device in Windows.

- [Download the Silicon Labs CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
    - Navigate to downloads, then CP210x Universal Windows Driver
    - Follow the installation instructions in the installer

## Step 3: Configure ESP-IDF

After installing the ESP-IDF extension, you need to configure the ESP-IDF environment.
1. Open the command pallate (ctrl + shift + p) and enter **ESP-IDF: Configure ESP-IDF Extension**
2. Click **Express**
3. Set the following options:
   - Select Download Server = `Github`
   - Select ESP-IDF Version = `v5.2.2`
   - Enter ESP-IDF container directory = `C:\Users\YOUR_USERNAME_HERE\esp`
   - Enter ESP-IDF Tools directory = `C:\Users\YOUR_USERNAME_HERE\.espressif`
4. Click **Install**, which should take a while.

## Step 4: Open The Project

1. Open Visual Studio Code.
2. Go to `File > Open Folder`.
3. Navigate to the location where you cloned or downloaded the `smartGarden` project.
4. Select the `espGarden` folder and open it.

## Step 5: Run the Customization Script

1. Open the integrated terminal in Visual Studio Code by selecting `Terminal > New Terminal` from the top menu.
2. Navigate to the root directory of the `espGarden` project if not already there.
3. Run the following command to execute the customization batch file: ./customizeUser.bat

## Step 6: Build the Project

To build the `espGarden` project, follow these steps in Visual Studio Code:

1. **Set the Espressif Target:**
   - Open the Command Palette by pressing `Ctrl+Shift+P`.
   - Type `ESP-IDF: Set Espressif device target` and select it.
   - Choose `ESP32-S3` from the list of available targets.

2. **Select the COM Port:**
   - Again, open the Command Palette with `Ctrl+Shift+P`.
   - Type `ESP-IDF: Select port to use` and select it.
   - Choose the correct COM port that corresponds to the Heltec ESP32-S3 connected over UART.

3. **Build the Project:**
   - In the sidebar, navigate to the ESP-IDF icon and expand the `Project Tasks` menu.
   - Click on `Build your project` under the default build tasks. This will compile the code for the `espGarden` project.
   - Monitor the build process in the terminal window. If the build completes successfully, you will see a `Build Complete` message along with the path to the generated binary file.

By setting the appropriate target and COM port, and using the provided build option, the project should compile without issues, ready for flashing to the Heltec ESP32-S3.

## Step 7: Flash the Project to the Device

To upload the compiled firmware to your Heltec ESP32-S3, use the following steps in Visual Studio Code:

1. **Prepare the Device:**
   - Ensure that your ESP32-S3 is connected via the correct COM port and that no other programs are using the port.

2. **Open the Command Palette:**
   - Press `Ctrl+Shift+P` to open the Command Palette in Visual Studio Code.

3. **Flash the Firmware:**
   - Type `ESP-IDF: Flash (UART) your project` and select it from the list.
   - The flashing process will begin, and progress will be displayed in the terminal window at the bottom of Visual Studio Code. Ensure not to disconnect your device during this process.

4. **Monitor the Output:**
   - After flashing, the console will display messages from the ESP32-S3 as it boots up with the new firmware.
   - If successful, you should see a confirmation that the device is running the new code.

This step uploads the compiled project to the Heltec ESP32-S3, making it ready for operation with the new `espGarden` firmware.

## Step 8: Monitor the Output Using Tera Term

After flashing the `espGarden` project to the ESP32-S3, you can monitor its output and interact with it using Tera Term, a terminal emulator. Follow these steps to set it up:

1. **Download and Install Tera Term:**
   - Tera Term ahould already be installed, but if not, download it from [Tera Term Official Website](https://ttssh2.osdn.jp/index.html.en).
   - Follow the installation instructions to install it on your computer.

2. **Open Tera Term:**
   - Launch Tera Term. It will prompt you to choose a communication method.
   - Select "Serial".

3. **Configure the Serial Port:**
   - In the "Port" field, select the COM port connected to the ESP32-S3.
   - Click "OK" to proceed.

4. **Set the Baud Rate:**
   - Go to the "Setup" menu, then select "Serial port..."
   - Set the baud rate to 115200, data 8 bit, parity none, stop 1 bit, and flow control none.
   - Click "OK" to apply the settings.

Now, Tera Term will display the output from the Heltec ESP32-S3. This is useful for debugging and verifying that the device is functioning correctly after flashing the new firmware.

**Note:** If you need to flash the firmware to the Heltec ESP32-S3 again, you must first disconnect Tera Term to free up the COM port. To do this:
   
1. In Tera Term, go to the "File" menu and select "Disconnect".
2. Perform the flashing process as described in Step 7.
3. After flashing, return to Tera Term, and use the "Setup" menu followed by the "Serial port..." option to reconnect to the correct COM port with the settings mentioned in Step 8.

This ensures that the COM port is available for flashing and re-establishes the connection for monitoring after the update.


## Conclusion

After following these steps, your development environment should be ready for working on the `espGarden` project using ESP-IDF in Visual Studio Code. For more detailed information and API reference, consult the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/).
