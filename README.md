# T-Display-S3-Touch-Bluetooth-MIDI-Pad v0.1 README

This project is an implementation of a Bluetooth (BLE) MIDI Drum Pad for the LilyGo T-Display S3 Touch.

Having experimented with the LilyGo T-Display-S3 ESP32 S3 based microcontroller before, getting ahold of a touch-screen version
made it obvious this would be perfect for a project like this with all the communication capabilities.

The code divides the 320x170 pixel touch screen into a 16 rectangle grid.  Once connected to a host via BLE, pressing in the 
grid will send a MIDI note - it is set up to send the 16 most common drum pad MIDI commands.  

It connects well to iOS devices and I've tested it with an M2 Macbook Pro.  On a Macbook, you need to connect via MIDI studio (Applications, Utilities, Audio MIDI setup).
On iOS devices, I've tested it with Garageband - (connect under Settings, Advanced, Bluetooth devices) and Korg Gadget II (Settings, Bluetooth devices).  If you have
gadget open it will autodetect the BLE device when you power it up, pretty sure Garageband does too.


----
You need the ESP32-BLE-MIDI and TFT-touch libraries for this.

If you need basic help getting your Arduino environment up and running to program this particular board please see this excellent resource:
https://github.com/teastainGit/LilyGO-T-display-S3-setup-and-examples

You can make a basic enclosure to keep the device safe out of the same plastic box the T-display ships in.  Just cut a hole the size of the PCB but slightly shorter
in the long dimension.  Be careful as the touch screen will easily lift off of the backlight panel, it can just stick back down.

---
Default MIDI notes sent:
1. Kick Drum (Bass Drum): MIDI Note 36 (C1)
2. Side Stick: MIDI Note 37 (C#1)
3. Snare Drum (Acoustic Snare): MIDI Note 38 (D1)
4. Hand Clap: MIDI Note 39 (D#1)
5. Snare Drum (Electric Snare): MIDI Note 40 (E1)
6. Low Tom: MIDI Note 41 (F1)
7. Closed Hi-Hat: MIDI Note 42 (F#1)
8. High Floor Tom: MIDI Note 43 (G1)
9. Pedal Hi-Hat: MIDI Note 44 (G#1)
10. Open Hi-Hat: MIDI Note 46 (A#1)
11. Low-Mid Tom: MIDI Note 47 (B1)
12. High Tom: MIDI Note 48 (C2)
13. Crash Cymbal: MIDI Note 49 (C#2)
14. Ride Cymbal: MIDI Note 51 (D#2)
15. Chinese Cymbal: MIDI Note 52 (E2)
16. Ride Bell: MIDI Note 53 (F2)
    


Road Map 
----------
v0.1 is basic and works pretty well.  Sometimes if you rapidly flail about on the pads a note will "stick".  In order to solve this I need to add a routine to limit
the length of time that a MIDI note can play once touched.

Only one touch is registered at a time in the current version, nor can we address the home touch button on the t-display S3 touch or use swipe gestures for functions.
These capabilities could be opened up by a change from touchlib.h based functions to use the CST816 library that was tested for T-display S3 touch specifically by mjdonders. 
This will enable using the home button and swipe gestures among other things potentially
(CST816_TouchLib)

The buttons aren't currently used but I could see things like:
- selecting different sets of MIDI note presets.
- changing colors of grid and highlights (currently hard-coded blue and red)
- switching to an onscreen piano keyboard (probably 2 rows of keys)


Turning this into a beat sequencer by enabling a beat sequencing mode and repurposing the grid would be another cool way to expand the capabilities here.

Hope you have fun! Enjoy and all the best!
Kris Ellestad


---

MISC information for programming your board

settings for Arduino IDE.  I'm using v2.2.1 currently:

“ESP32S3 Dev Module"

Upload Speed: "921600"

USB Mode: "Hardware CDC and TAG"

USB CDC On Boot: "Enabled"

USB Firmware MSC On Boot: "Disabled"

USB DFU On Boot: "Disabled"

Upload Mode: "UARTO / Hardware CDC'

CPU Frequency: "240MHz (WiFi)"

Flash Mode: "QIO 80MHz"

Flash Size: "16MB (128Mb)"

Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"

Core Debug Level: "="

PSRAM: "OPI PSRAM"

Arduino Runs On: "Core 1"

Events Run On: "Core 1"



