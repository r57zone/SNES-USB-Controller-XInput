[![EN](https://user-images.githubusercontent.com/9499881/33184537-7be87e86-d096-11e7-89bb-f3286f752bc6.png)](https://github.com/r57zone/SNES-USB-Controller-XInput/) 
[![RU](https://user-images.githubusercontent.com/9499881/27683795-5b0fbac6-5cd8-11e7-929c-057833e01fb1.png)](https://github.com/r57zone/SNES-USB-Controller-XInput/blob/master/README.RU.md)
&#8211; Other languages / Другие языки

# SNES USB Controller XInput
Xbox emulator for [SNES USB controller](http://alii.pub/6hw4yd). Works based on the driver [ViGEm](https://github.com/ViGEm).

![](https://user-images.githubusercontent.com/9499881/197362614-fee74a6d-0f46-4421-95ff-56c5b3ab57e4.png)

# Working modes
Switching emulation from stick to DPAD and back is done by pressing `Select + DPAD ←` or `ALT + A`.


Switching emulation from triggers to bumpers and back is done by pressing `Select + DPAD ↑` or `ALT + Q`.


The default modes can be configured in the `Config.ini` configuration file.

## Setup
1. Install [ViGEmBus](https://github.com/ViGEm/ViGEmBus/releases).
2. Install Microsoft Visual C++ Redistributable 2017 or newer.
3. Connect the controller.
4. Run the program.



Instead of an application, you can also use libraries for XInputInjector:

1. [Download XInputInjector](https://github.com/r57zone/X360Advance/releases/) and extract.
2. Unpack the contents of the `SNES.USB.Controller.XInput.Libraries.zip` archive into the `Libraries` folder.
3. Launch XInputInjector and select the required application.

## Download
>Version for Windows 10.

**[Download](https://github.com/r57zone/SNES-USB-Controller-XInput/releases)**

## Credits
* [ViGEm](https://github.com/ViGEm) for the ability to emulate different gamepads.
* [MinHook library](https://github.com/TsudaKageyu/minhook) for a ready solution for function injection.

## Feedback
`r57zone[at]gmail.com`