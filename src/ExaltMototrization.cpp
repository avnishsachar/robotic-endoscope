// ExaltMototrization.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <stdio.h>
#include <iostream>
#include "CMaxonMotor.h"
#include "Definitions.h"
#include "serial.h"
#include <chrono>
#include <Windows.h>
#include <cmath>
#include <vector>


using namespace std;

int main(int argc, char* argv[])
{
    long CurrentPosition[2];
    long motor_velocity = 0;
    const std::string port = "COM3";
    uint32_t  baud = 115200;
    const int size = 7;
    std::string exalt_state = {0, size};
    unsigned int joystick_ud = 0;
    unsigned int joystick_rl = 0;
    unsigned int button_joystick = 1;
    unsigned int button_joystick_previous = 1;
    unsigned int button_home = 1;
    unsigned int button_home_previous = 1;
    int p_gain = 20;


    CMaxonMotor motor;

    motor.InitializeAllDevices();
    std::vector<long> home_position = motor.HomingProcedureAllDevices();
    
    serial::Serial serial_port (port, baud, serial::Timeout::simpleTimeout(300));
    if (!serial_port.isOpen()) {
        std::cout << "Serial Connection Failed" << endl;
    }
    else {
        serial_port.flush(); // flush the port 
        std::cout << "serial Connection successful" << endl;
    }
    bool exit_flag = TRUE;

    while (exit_flag) {
        auto reset = chrono::steady_clock::duration::zero();
        auto zero = std::chrono::steady_clock::now();
        if (GetAsyncKeyState(VK_ESCAPE)) {
            exit_flag = FALSE;
            std::cout << "ESC PRESSED - EXIT" << endl;
        }
        auto start = std::chrono::steady_clock::now();
        serial_port.write("r");
        auto end = std::chrono::steady_clock::now();
        while (serial_port.getBytesize() < 7 && chrono::duration_cast<chrono::milliseconds>(end - start).count() < 100) {
        }
        if (serial_port.getBytesize() >= 7 ) {
            exalt_state = serial_port.read(size);
            if (exalt_state.length() == 0)
                continue;
            joystick_ud = (uint8_t)exalt_state[2] * 256 + (uint8_t)exalt_state[3];
            joystick_rl = (uint8_t)exalt_state[4] * 256 + (uint8_t)exalt_state[5];

            //Update the velocity mode. 
            button_joystick = (uint8_t)exalt_state[6] >> 2;

            button_home = ((uint8_t)exalt_state[6] >> 1) & 0x01;

            if (button_joystick == 0 && button_joystick_previous == 1)
            {
                if (p_gain == 20) {
                    p_gain = 5;
                    std::cout << "Speed Setting: LOW" << endl;
                }
                else if (p_gain == 12) {
                    p_gain = 20;
                    std::cout << "Speed Setting: HIGH" << endl;
                }
                else if (p_gain == 5) {
                    p_gain = 12;
                    std::cout << "Speed Setting: NORMAL" << endl;
                }
                else {
                    p_gain = 20;
                    std::cout << "Speed Setting: HIGH" << endl;
                }
            }
            if (button_home == 0 && button_home_previous == 1)
            {
                std::cout <<"Home"<< endl;
                motor.HomeAllDevices(home_position);
                motor.GetCurrentPositionAllDevice(CurrentPosition);
                while ((abs(CurrentPosition[0] - home_position[0]) > 100) || (abs(CurrentPosition[0] - home_position[0]) > 100))
                {

                }

                motor.ActivateProfileVelocityModeAll();
            }
            // Update the previous button condition
            button_joystick_previous = button_joystick;
            button_home_previous = button_home;
            motor.ActivateProfileVelocityModeAll();
            // Set the motor velocity for the right/left motor based on the joystick position 
            // Use a deadband to prevent motor creep when the joystick is at the zero position
            if (abs((int)joystick_rl - 512) < 10)
            {
                motor_velocity = 0;
            }
            else
            {
                motor_velocity = ((int)joystick_rl - 512) * p_gain;
            }

            motor.MoveWithVelocityOne(motor_velocity);

            // Set the motor velocity for the up/down motor based on the joystick position
            // Use a deadband to prevent motor creep when the joystick is at the zero position
            if (abs((int)joystick_ud - 512) < 10)
            {
                motor_velocity = 0;
            }
            else
            {
                motor_velocity = ((int)joystick_ud - 512) * p_gain;
            }

            motor.MoveWithVelocityTwo(motor_velocity);
        }
    }
    serial_port.close();
    motor.DisableAllDevice();
    motor.CloseAllDevice();
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
