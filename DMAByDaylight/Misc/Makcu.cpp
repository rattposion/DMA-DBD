#include "pch.h"
#include "Makcu.h"
#include <iostream>
#include <vector>

Makcu* MakcuInstance = new Makcu();

Makcu::Makcu()
{
    hSerial = INVALID_HANDLE_VALUE;
    Connected = false;
    CurrentPort = 0;
    Log("Makcu initialized.");
}

Makcu::~Makcu()
{
    Disconnect();
}

void Makcu::Log(std::string message)
{
    std::string fullMsg = "[Makcu] " + message;
    // Keep internal log
    std::wstring wMsg(fullMsg.begin(), fullMsg.end());
    Logs.push_back(wMsg);
    
    // Also print to console
    std::cout << fullMsg << std::endl;
}

std::vector<std::wstring> Makcu::GetLogs()
{
    return Logs;
}

int Makcu::AutoDetectPort()
{
    Log("Attempting auto-detection...");
    // GUID for Serial Ports
    GUID guid = { 0x4d36e978, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } };
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        Log("AutoDetect: SetupDiGetClassDevs failed.");
        return 0;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        char buffer[256];
        if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)buffer, sizeof(buffer), NULL))
        {
            std::string hwID = buffer;
            // VID_1A86 & PID_55D3
            if (hwID.find("VID_1A86") != std::string::npos && hwID.find("PID_55D3") != std::string::npos)
            {
                // Found the device, now get the COM port name
                HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
                if (hKey != INVALID_HANDLE_VALUE)
                {
                    char portName[32];
                    DWORD size = sizeof(portName);
                    if (RegQueryValueExA(hKey, "PortName", NULL, NULL, (LPBYTE)portName, &size) == ERROR_SUCCESS)
                    {
                        std::string portStr = portName;
                        if (portStr.find("COM") != std::string::npos)
                        {
                            Log("Found Makcu Device at " + portStr);
                            RegCloseKey(hKey);
                            SetupDiDestroyDeviceInfoList(hDevInfo);
                            try {
                                return std::stoi(portStr.substr(3));
                            } catch (...) {
                                Log("Error parsing port number from " + portStr);
                                return 0;
                            }
                        }
                    }
                    RegCloseKey(hKey);
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    Log("Device not found via auto-detection.");
    return 0;
}

bool Makcu::Connect(int port)
{
    if (port == 0)
    {
        port = AutoDetectPort();
        if (port == 0)
        {
             return false;
        }
    }

    if (Connected && CurrentPort == port)
    {
        Log("Already connected to COM" + std::to_string(port));
        return true;
    }

    Disconnect();

    Log("Connecting to COM" + std::to_string(port) + "...");

    std::wstring portName = L"\\\\.\\COM" + std::to_wstring(port);
    hSerial = CreateFileW(portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        Log("Failed to open port COM" + std::to_string(port) + " Error: " + std::to_string(GetLastError()));
        return false;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        Log("Failed to get comm state.");
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return false;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        Log("Failed to set comm state.");
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    Connected = true;
    CurrentPort = port;
    Log("Successfully connected to COM" + std::to_string(port));
    return true;
}

void Makcu::Disconnect()
{
    if (hSerial != INVALID_HANDLE_VALUE)
    {
        Log("Disconnecting...");
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
    Connected = false;
}

bool Makcu::IsConnected()
{
    return Connected;
}

bool Makcu::SendCommand(std::string command)
{
    if (!Connected || hSerial == INVALID_HANDLE_VALUE)
    {
         // Avoid spamming log if disconnected
         // Log("SendCommand failed: Not connected.");
        return false;
    }

    if (command.find("\r\n") == std::string::npos)
        command += "\r\n";

    // Debug log for command (optional, can be spammy)
    // Log("Sending: " + command);

    DWORD bytesWritten;
    if (!WriteFile(hSerial, command.c_str(), (DWORD)command.length(), &bytesWritten, NULL))
    {
        Log("WriteFile failed. Error: " + std::to_string(GetLastError()));
        Disconnect();
        return false;
    }

    return true;
}

void Makcu::Click()
{
    // Default left click
    SendCommand("km.left(1)");
    Sleep(std::rand() % 20 + 30); 
    SendCommand("km.left(0)");
}

void Makcu::PressKey(int key)
{
    UINT scanCode = MapVirtualKey(key, MAPVK_VK_TO_VSC);
    
    // Using generic km.keydown/keyup as per common protocols
    // If specific API differs, this needs adjustment
    std::string cmdDown = "km.keydown(" + std::to_string(scanCode) + ")";
    std::string cmdUp = "km.keyup(" + std::to_string(scanCode) + ")";
    
    SendCommand(cmdDown);
    Sleep(std::rand() % 20 + 30);
    SendCommand(cmdUp);
}