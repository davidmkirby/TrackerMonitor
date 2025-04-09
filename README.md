# EO Imaging 7007 Tracker Monitor

## Overview

This Qt6-based application provides a real-time interface for the EO Imaging 7007 tracker card. It allows direct monitoring of track errors, status information, and system configuration through memory-mapped I/O. The application is designed for high-performance environments including real-time kernels.

## Features

- **Direct Hardware Access**: Memory-mapped I/O for high-speed communication
- **Track Error Display**: Real-time monitoring of both raw and filtered X/Y track errors
- **Status Information**: Display of track state, mode, target polarity, and error status
- **Target Information**: Size, position, and pixel count visualization
- **Mount Position**: Display of azimuth and elevation angles
- **High-Speed Polling**: 250Hz polling rate (4ms) to match tracker capabilities
- **Ping Functionality**: Card communication testing
- **CSV Data Logging**: Record all tracker parameters for analysis
- **PCI Configuration**: Automatic PCI device configuration

## System Requirements

- Linux system with root access
- Qt6 development libraries
- `setpci` utility installed (part of pciutils package)
- Compatible EO Imaging 7007 tracker card (Camera Link base digital video interface)

## Installation

### Dependencies

Install the required dependencies:

```bash
# For Fedora/RHEL systems
sudo dnf install qt6-qtbase-devel qt6-tools-devel pciutils
```

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/tracker-monitor.git
   cd tracker-monitor
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure and build the project:
   ```bash
   cmake ..
   make -j$(nproc)
   ```

4. Install (optional):
   ```bash
   sudo make install
   ```

## Usage

### Running the Application

The application requires root privileges to access hardware:

```bash
sudo ./TrackerMonitor
```

### Main Interface

The application consists of several panels:

1. **Track Errors Panel**: Displays raw and filtered X/Y track errors
2. **Status Information Panel**: Shows track state, mode, and polarity
3. **Target Information Panel**: Displays target size, position, and pixel count
4. **Control Panel**: Provides init, ping, and logging functionality

### Basic Operations

1. **Initialize**: Click the "Initialize" button to establish connection with the tracker card
2. **Ping**: Test communication with the tracker card
3. **Automatic Poll**: Enable continuous polling at 250Hz to update displays
4. **Start/Stop Logging**: Record track data to CSV files for later analysis

## Configuration

### PCI Device Settings

The application automatically configures the PCI device, but you can modify the PCI device location in the initialization call if needed:

```cpp
// Default values
m_trackerMemory->initialize(0xdba00000, 0x0800, 0x98, 0x00, 0x00);

// Parameters:
// 1. Base memory address (0xdba00000)
// 2. Memory size (0x0800)
// 3. PCI bus number (0x98)
// 4. PCI slot number (0x00)
// 5. PCI function number (0x00)
```

### Tracker Parameters

The tracker card uses memory-mapped registers with predefined offsets:

```
Command Message: 0x0000
Command Mailbox: 0x03FE
Status Message: 0x0400
Query Response: 0x07FC
Status Mailbox: 0x07FE
```

## Troubleshooting

### Common Issues

1. **PCI Configuration Failed**
   - Ensure the application is run with root privileges
   - Verify correct PCI device location (bus:slot.function)
   - Check that the `setpci` utility is installed

2. **Device Not Responding**
   - Verify physical installation and power
   - Check that the card is properly seated in the PCI slot
   - Ensure Camera Link cable is properly connected

3. **Memory Mapping Failed**
   - Verify correct base memory address
   - Ensure real-time kernel permissions allow memory access

### Debug Logging

The application provides detailed console logging. To capture this information:

```bash
sudo ./TrackerMonitor 2>&1 | tee debug.log
```

## Technical Details

### Memory Map Structure

The tracker card uses memory-mapped I/O with a specific communication protocol:

1. **Command Sequence**:
   - Write command data to Command Message area
   - Set Command Mailbox to non-zero value
   - Wait for Command Mailbox to return to zero

2. **Status Reading**:
   - Check Status Mailbox for non-zero value
   - Read data from Status Message area
   - Set Status Mailbox to zero to acknowledge

### Status Message Format

- Bytes 0-1: Sync word (0xA5A5)
- Bytes 2-3: Message Type (0xFF for status)
- Bytes 4-7: Raw X/Y track errors (16-bit values divided by 32)
- Bytes 10-11: Status information (polarity, track state, mode)
- Bytes 12-13: Status flags
- Bytes 14-23: Target information
- Bytes 24-31: Mount position (azimuth, elevation)
- Bytes 32-35: Filtered X/Y track errors

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Based on EO Imaging 7007 tracker card documentation
- Uses Qt6 framework for GUI development
- Memory mapping approach inspired by established direct hardware access patterns

---

**Note**: This application requires direct hardware access and should be used only on systems where you have appropriate authorization to access hardware resources.