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

## Features

- Simple command-line utility to set the Caps Lock LED on or off.
- Uses low-level device I/O (open/ioctl/close) to control the kernel LED interface.
- Minimal, dependency-free C program suitable for learning about device ioctl calls and keyboard LED control.

## Public behavior / usage API

- Invocation: `./capslock_led 1` to turn Caps Lock LED ON.
- Invocation: `./capslock_led 0` to turn Caps Lock LED OFF.
- Exit codes: `0` on success, non-zero on error (invalid args, device open/ioctl failures).

## Methods / system calls used (what the program calls)

- main(argc, argv)
	- Parses command-line argument and maps it to an LED state.

- open(const char *pathname, int flags)
	- Opens the console device (`/dev/console`) with `O_NOCTTY` to get a file descriptor used for ioctl.

- ioctl(int fd, unsigned long request, ...)
	- Uses the KDSETLED request from `<linux/kd.h>` to set the LED state. This is the kernel-facing control point.

- close(int fd)
	- Closes the file descriptor when finished.

These are the core I/O calls: open -> ioctl(KDSETLED) -> close.

## Device/driver details

- Device node used: `/dev/console` (the program opens this console device and sends the KDSETLED ioctl).
- Kernel interface: The `KDSETLED` ioctl is handled by the kernel's virtual console / keyboard driver subsystem. The program does not talk directly to hardware — it uses the kernel's driver to apply the LED state.

## Libraries / headers used

- `<stdio.h>` — command-line I/O and error messages (fprintf, perror).
- `<stdlib.h>` — exit codes and general utilities.
- `<fcntl.h>` — open flags (`O_NOCTTY`).
- `<unistd.h>` — close and other POSIX helpers.
- `<sys/ioctl.h>` — ioctl function declaration.
- `<linux/kd.h>` — kernel definitions for KDSETLED and LED_CAP.

## Permissions and security

- Writing to `/dev/console` or issuing KDSETLED may require root privileges depending on system udev rules and device file permissions. If you get `Error opening /dev/console` or `Error setting LED state`, try running the binary with `sudo`.
- The program uses ioctl — it's a privileged kernel interface; only trusted code should perform such operations.

## Graphical hierarchy of the communication

Below is a simple ASCII diagram showing the flow of control and data from the user command through the kernel to the keyboard driver and hardware LED.

User space           Program (capslock_led)            Kernel / Drivers                 Hardware
-----------          ------------------------          ----------------                 --------
 Terminal  --->  argv parsing & main()  --->  open("/dev/console")  --->  KDSETLED ioctl  --->  keyboard driver  --->  keyboard LED

Sequence (expanded):
1. User runs `./capslock_led 1` in a shell.
2. `main()` parses the argument and selects LED_CAP or 0.
3. Program calls `open("/dev/console", O_NOCTTY)` to get a file descriptor.
4. Program calls `ioctl(fd, KDSETLED, led_state)`. This syscall transitions control into kernel space.
5. Kernel's console/keyboard driver receives KDSETLED and updates its internal LED state; the driver communicates with the physical keyboard controller to update the LED.
6. Program calls `close(fd)` and returns to user space.

## Notes and extensions

- The current program only toggles the Caps Lock LED (uses `LED_CAP`). It can be extended to control Num Lock / Scroll Lock or to cycle multiple LEDs.
- For devices that don't expose `/dev/console`, you may need to target a specific input device (e.g., `/dev/input/eventX`) and use a different ioctl interface; that typically requires reading kernel driver docs and possibly using the input subsystem.
- Consider adding error handling for cases where KDSETLED isn't supported on the opened device — the program currently reports the ioctl error.

## Contact

If you'd like, I can:
- Add command-line parsing to support named flags (e.g., `--caps on|off`),
- Add support for other LEDs (Num/Scroll), or
- Produce a simple visualization (PNG/SVG) of the communication hierarchy and include it in the repo.


## License

Add a license file if you want to make this project open source. Otherwise keep it private.

## Contact

If you'd like help pushing this repository to GitHub or making it public, tell me and I will push it using the GitHub CLI (if available) or provide step-by-step instructions.
