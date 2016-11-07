# StereoCapture

This project is licensed under the terms of the MIT license.

## Description

This software snaps a sequence of images for a system of 2 cameras with minimal latency. 
Frame capture can be triggered by a timer by setting a framerate, or by selecting fast mode, 
in which case all frames coming from the devices are saved (in this case, the framerate matches the camera settings).
The capture gets unstable when the bandwidth of the USB ports is saturated, and can lead to the app freezing.
This software relies on the IC Imaging Control toolikt and only works for The Imaging Source cameras on Windows.

## Requirements

### To run the software

- [Device driver for USB cameras](https://www.theimagingsource.com/support/downloads-for-windows/device-drivers/icwdmuvccamtis/)
- Visual C++ redistributable for Visual Studio 2013
- The IC imaging control dll library, which can be found in the [IC imaging control C++ SDK](https://www.theimagingsource.com/support/downloads-for-windows/software-development-kits-sdks/icimagingcontrol/)
- If your processor goes to idle mode when capturing images and frames are dropped, [this program](https://www.theimagingsource.com/support/downloads-for-windows/software-tools/procidlestateman/) can help

### To build the software (Windows)

- [IC imaging control C++ SDK](https://www.theimagingsource.com/support/downloads-for-windows/software-development-kits-sdks/icimagingcontrol/)
- Visual Studio 2013 (probably works with other versions as well)

## Build instructions

1. Open the solution file (.sln) with Visual Studio
2. Define the path to the IC imaging control C++ toolkit as `IC33PATH` in Visual Studio as a user macro:
  * go to View -> Property Manager or View -> Other Windows -> Property Manager
  * double click on *.user
  * go to User Macros
  * add or modify a macro: name it `IC33PATH` and assign the path to it (ex: `D:\username\Documents\IC Imaging Control 3.4`)
  * enable "Set this macro as an environment variable in the build environment check box."
  * See [Microsoft online help](https://msdn.microsoft.com/en-us/library/669zx6zc.aspx) for further assistance
3. Select the wanted platform and build the executable.


## References

https://www.theimagingsource.com/support/documentation/ic-imaging-control-cpp/