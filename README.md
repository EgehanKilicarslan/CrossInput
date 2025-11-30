# CrossInput

A cross-platform C++ library for simulating keyboard and mouse input.

## Features

- **Keyboard Simulation**: `KeyDown()`, `KeyUp()`, `KeyPress()`
- **Mouse Simulation**: `MouseButtonDown()`, `MouseButtonUp()`, `MouseClick()`
- **Cursor Control**: `SetCursorPosition()`, `MoveCursor()`, `GetCursorPosition()`
- **Key State Detection**: `IsKeyPressed()`

## Platform Support

| Platform        | Status  | Backend                          |
| --------------- | ------- | -------------------------------- |
| Windows         | ✅ Full | Win32 API                        |
| Linux (X11)     | ✅ Full | Xlib + XTest                     |
| Linux (Wayland) | ✅ Full | libei + XDG Portal (Hybrid mode) |

### Wayland Hybrid Mode

On Wayland, CrossInput uses a hybrid approach for best compatibility:

- **Input simulation** (keyboard, mouse, cursor movement): Native Wayland via libei
- **State reading** (cursor position, key state): XWayland

This provides full functionality on modern Wayland desktops like KDE Plasma and GNOME.

## Dependencies

### Linux

```bash
# Debian/Ubuntu
sudo apt install libx11-dev libxtst-dev libei-dev libglib2.0-dev

# Arch Linux
sudo pacman -S libx11 libxtst libei glib2

# Fedora
sudo dnf install libX11-devel libXtst-devel libei-devel glib2-devel
```

### Windows

No additional dependencies required (uses Win32 API).

## Building

```bash
# Build everything
make

# Build release version
make release

# Build library only
make lib

# Run tests
make run_interactive
```

## Installation

```bash
# System-wide install (requires sudo)
sudo make install

# Local install (no sudo)
make install PREFIX=$HOME/.local

# Uninstall
sudo make uninstall
```

## Usage

### Basic Example

```cpp
#include <CrossInput/CrossInput.h>

int main() {
    // Get cursor position
    auto pos = CrossInput::GetCursorPosition();

    // Move cursor
    CrossInput::SetCursorPosition({100, 200});
    CrossInput::MoveCursor(50, 0);  // Move right by 50 pixels

    // Simulate keyboard
    CrossInput::KeyPress(CrossInput::KeyCode::KEY_A);

    // Simulate mouse click
    CrossInput::MouseClick(CrossInput::MouseButton::Left);

    return 0;
}
```

### Compiling Your Program

```bash
# If installed system-wide
g++ -std=c++17 myapp.cpp -lCrossInput -lX11 -lXtst -lei -lgio-2.0 -lgobject-2.0 -lglib-2.0

# If using direct path
g++ -std=c++17 myapp.cpp \
    -I/path/to/CrossInput/include \
    -L/path/to/CrossInput/build \
    -lCrossInput -lX11 -lXtst -lei -lgio-2.0 -lgobject-2.0 -lglib-2.0
```

## API Reference

### Keyboard Functions

| Function                         | Description                         |
| -------------------------------- | ----------------------------------- |
| `bool IsKeyPressed(KeyCode key)` | Check if a key is currently pressed |
| `void KeyDown(KeyCode key)`      | Simulate key press down             |
| `void KeyUp(KeyCode key)`        | Simulate key release                |
| `void KeyPress(KeyCode key)`     | Simulate full key press (down + up) |

### Mouse Functions

| Function                                | Description                     |
| --------------------------------------- | ------------------------------- |
| `void MouseButtonDown(MouseButton btn)` | Simulate mouse button press     |
| `void MouseButtonUp(MouseButton btn)`   | Simulate mouse button release   |
| `void MouseClick(MouseButton btn)`      | Simulate full click (down + up) |

### Cursor Functions

| Function                            | Description                      |
| ----------------------------------- | -------------------------------- |
| `Point GetCursorPosition()`         | Get current cursor position      |
| `void SetCursorPosition(Point pos)` | Move cursor to absolute position |
| `void MoveCursor(int dx, int dy)`   | Move cursor by relative amount   |

### Supported Key Codes

- **Letters**: `KEY_A` through `KEY_Z`
- **Numbers**: `KEY_0` through `KEY_9`
- **Function Keys**: `KEY_F1` through `KEY_F12`
- **Modifiers**: `KEY_SHIFT`, `KEY_CONTROL`, `KEY_ALT`
- **Navigation**: `KEY_UP`, `KEY_DOWN`, `KEY_LEFT`, `KEY_RIGHT`
- **Special**: `KEY_SPACE`, `KEY_ENTER`, `KEY_TAB`, `KEY_ESCAPE`, `KEY_BACKSPACE`

### Mouse Buttons

- `MouseButton::Left`
- `MouseButton::Right`
- `MouseButton::Middle`

## Makefile Targets

| Target                 | Description                           |
| ---------------------- | ------------------------------------- |
| `make`                 | Build everything                      |
| `make lib`             | Build library only                    |
| `make release`         | Build with optimizations              |
| `make debug`           | Build with debug symbols              |
| `make install`         | Install to system (PREFIX=/usr/local) |
| `make uninstall`       | Remove installed files                |
| `make run_interactive` | Run interactive test                  |
| `make clean`           | Remove build artifacts                |
| `make help`            | Show all targets                      |

## License

See [LICENSE](LICENSE) file.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
