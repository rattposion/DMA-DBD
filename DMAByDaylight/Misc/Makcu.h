#pragma once
#include "pch.h"
#include <string>
#include <Windows.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>

class Makcu
{
private:
    HANDLE hSerial;
    bool Connected;
    int CurrentPort;

public:
    Makcu();
    ~Makcu();

    bool Connect(int port);
    void Disconnect();
    bool IsConnected();
    bool SendCommand(std::string command);
    void Click(); 
    void PressKey(int key);

    // Auto-detection
    int AutoDetectPort();

    // Logging
    std::vector<std::wstring> GetLogs();
    void Log(std::string message);

private:
    std::vector<std::wstring> Logs;
};

extern Makcu* MakcuInstance;