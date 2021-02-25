#pragma once

#include "Definitions.h" // Maxon Motor Header file
#include <vector>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <cmath>
#include <stdio.h>

#define TRUE 1
#define FALSE 0


class CMaxonMotor
{
private:
    const char* PortName_0;
    const char* PortName_1;
    DWORD errorCode;
    unsigned short nodeId_0;
    unsigned short nodeId_1;
    void* keyHandle_0 = NULL;
    void* keyHandle_1 = NULL;

    void EnableDevice(void* keyHandle_, unsigned short nodeId);
    void DisableDevice(void* keyHandle_, unsigned short nodeId);
    void* ActivateDevice(const char* PortName, unsigned short nodeId);
    long HomingProcedure(void* keyHandle_, unsigned short nodeId);
    void CloseDevice(void* keyHandle_);
    void Move(void* keyHandle_, long TargetPosition, unsigned short nodeId);
    void GetCurrentPosition(void* keyHandle_, long& CurrentPosition, unsigned short nodeId);
    void SetPositionProfile(void* keyHandle_, unsigned short nodeId);
    void Halt(void* keyHandle_, unsigned short nodeId);

public:
    CMaxonMotor();

    void InitializeAllDevices(); // Initialize devices at the start 
    void CloseAllDevice();
    void DisableAllDevice();
    void GetCurrentPositionAllDevice(long* CurrentPosition);
    void MoveAllDevice(const long* TargetPosition);
    std::vector<long> HomingProcedureAllDevices();
    void HomeAllDevices(std::vector<long> home_position);
    void ActivateProfileVelocityModeAll();
    void MoveWithVelocityOne(long motor_velocity);
    void MoveWithVelocityTwo(long motor_velocity);
    //    CMaxonMotor(char[], unsigned int );


    //    void GetCurrentPosition(int& CurrentPosition); // need change

    //    void GetPositionProfile();
    //    void SetPositionProfile(unsigned int ProfileVelocity, unsigned int ProfileAcc, unsigned int ProfileDec);
};


