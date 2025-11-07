# Toggle Capslock LED

Small C program to toggle the Caps Lock LED on compatible keyboards. This project contains a simple implementation (`capslock_led.c`) that demonstrates how to interact with the keyboard LED on Linux systems.

## What it is

- Language: C
- Purpose: Toggle the Caps Lock LED (useful for small utilities or learning how to access input device LEDs)

## Files

- `capslock_led.c` — source code that toggles the Caps Lock LED.

## Build

You'll need a C compiler (gcc/clang) and development headers available on your system.

Example (compile in the project folder):

```bash
gcc -o capslock_led capslock_led.c
```

## Run

Run the resulting binary (may require root privileges depending on how the program accesses input devices):

```bash
sudo ./capslock_led
```

Notes:
- Running as root may be required to access input device files under `/dev/input`.
- Behavior depends on your device and kernel support for LED ioctl operations.

## License

Add a license file if you want to make this project open source. Otherwise keep it private.

## Contact

If you'd like help pushing this repository to GitHub or making it public, tell me and I will push it using the GitHub CLI (if available) or provide step-by-step instructions.
