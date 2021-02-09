/************************************************************************************************/
// AVCapture2.cpp : Defines the class behaviors for the application.
/************************************************************************************************/
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "AVCapture2.h"
#include "MainFrm.h"
#include <afxwin.h>
#include <stdio.h>
#include <iostream>
#include "CMaxonMotor.h"
#include "Definitions.h"
#include "serial.h"
#include <chrono>
#include <Windows.h>
#include <cmath>
#include <vector>
#include <fcntl.h>

#pragma comment(lib, "EposCmd.lib")
// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

using namespace std;

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAVCapture2App

BEGIN_MESSAGE_MAP(CAVCapture2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAVCapture2App::OnAppAbout)
END_MESSAGE_MAP()


struct MotorParams
{
	int joystick = 0;
	int joystick_2 = 0;
    std::string speed = "NULL";
};

// CAVCapture2App construction

CAVCapture2App::CAVCapture2App()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("AVCapture2.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CAVCapture2App object
CAVCapture2App theApp;

// CAVCapture2App initialization

UINT MotorThread(LPVOID pParam)
{
	MotorParams* motorparams = (MotorParams*) pParam;

    long CurrentPosition[2];
    long motor_velocity = 0;
    const std::string port = "COM3";
    uint32_t  baud = 115200;
    const int size = 7;
    std::string exalt_state = { 0, size };
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

    serial::Serial serial_port(port, baud, serial::Timeout::simpleTimeout(300));
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
        if (serial_port.getBytesize() >= 7) {
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
                    motorparams->speed = "LOW";
                }
                else if (p_gain == 12) {
                    p_gain = 20;
                    std::cout << "Speed Setting: FAST" << endl;
                    motorparams->speed = "FAST";
                }
                else if (p_gain == 5) {
                    p_gain = 12;
                    std::cout << "Speed Setting: NORMAL" << endl;
                    motorparams->speed = "NORMAL";
                }
                else {
                    p_gain = 20;
                    std::cout << "Speed Setting: FAST" << endl;
                    motorparams->speed = "FAST";
                }
            }
            if (button_home == 0 && button_home_previous == 1)
            {
                std::cout << "Home" << endl;
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

	return 0;   // thread completed successfully
}


BOOL CAVCapture2App::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	WCHAR wPath[1024] = {0};
	GetCurrentDirectory(1024, wPath);

	int len = WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), NULL, 0, NULL, NULL);  
	char* chPath = new char[len + 1];  
	WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), chPath, len, NULL, NULL);  
	chPath[len] = '\0';  

	char chDrive[64] = {0};
	_splitpath(chPath, chDrive, NULL, NULL, NULL);

	CString strDrive(chDrive);


	MWCaptureInitInstance();

	int nVideoCount = ::MWGetChannelCount();
	if (nVideoCount <= 0) {
		AfxMessageBox(_T("Can't find capture device!\n"));
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

    MotorParams* pmotorparams = new MotorParams();

	CWinThread* pthread;
    TRACE("This is a debug string of text in MFC");
	pthread = (CWinThread*) AfxBeginThread(MotorThread, (LPVOID)&pmotorparams, THREAD_PRIORITY_NORMAL, 0, 0, 0);

	return TRUE;
}

int CAVCapture2App::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	MWCaptureExitInstance();

	return CWinApp::ExitInstance();
}

// CAVCapture2App message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CAVCapture2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAVCapture2App message handlers

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
	CloseHandle(hDumpFile);  
}


LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	CreateDumpFile(L"AVCapture2.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}

