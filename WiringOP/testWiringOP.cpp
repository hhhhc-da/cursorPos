#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <regex>

#include "wiringPi.h"
#include "X11/Xlib.h"
#include "picojson.h"

using namespace std;
using namespace picojson;

// GPIOs storage: int[8] - maiButton
namespace
{
    Display *display;
    Window window;
    int screen;

    const unsigned numButton = 3;
    std::array<int, numButton> maiButton;
    std::array<int, numButton> maiButtonStatus;
    std::array<int, numButton> posX;
    std::array<int, numButton> posY;
}

void GPIOInit();
void readGPIOs();
void clickPosInit();
void maiClick(int mai, int s);
void click(int x, int y);
void raiseID(size_t id);

// int main(int argc, char *argv[])
int main()
{
    // now time
    auto timeBegin = chrono::steady_clock::now();

    cout << "Test WiringOP start..."
         << endl;

    // wirintPi initialize
    if (wiringPiSetup() == -1)
    {
        cout << "GPIO initialization fail in line:" << __LINE__ << endl;
        return 1;
    }

    cout << "GPIO setup OK" << endl;

    // default GPIO initialize
    GPIOInit();
    cout << "GPIO initialized OK" << endl;

    // size_t taskID = getID();
    // cout << "get id: " << taskID << endl;

    clickPosInit();
    cout << "read position file OK" << endl;

    raiseID(0x3200003);
    cout << "raise task." << endl;

    // read pins loop function
    readGPIOs();

    fstream log("Error.log");

    log.write("Error occured.\n", ios::trunc);

    auto timeEnd = chrono::steady_clock::now();
    chrono::duration<double> allTime = timeEnd - timeBegin;

    string errLog;
    ostringstream errStream(errLog);

    errStream << "Time: " << allTime.count() << "\n";

    errLog = errStream.str();

    log.write(errLog.c_str(), ios::app);

    // default error loop
    while (true)
    {
    }

    return 0;
}

void GPIOInit()
{
    for (int i = 0; i < numButton; ++i)
    {
        // pins defination
        maiButton[i] = i;

        // pins status defination
        maiButtonStatus[i] = 0;
    }

    for (int i = 0; i < numButton; ++i)
    {
        // Input mode + pull down
        pinMode(maiButton[i], INPUT);
        pullUpDnControl(maiButton[i], PUD_UP);
    }
}

void readGPIOs()
{
    cout << "start reading GPIOs" << endl;
    while (true)
    {
        for (int i = 0; i < numButton; ++i)
        {
            maiButtonStatus[i] = digitalRead(maiButton[i]);
            if (!maiButtonStatus[i])
            {
                cout << "read GPIO!" << endl;
            }
            // click to screen
            maiClick(maiButton[i], maiButtonStatus[i]);
            // click(posX[i], posY[i]);

            delay(30);
        }
    }
}

void maiClick(int mai, int s)
{
    if (s)
        return;

    click(posX[mai], posY[mai]);
}

void clickPosInit()
{
    ifstream ifs("pos.json");
    string jsonStr("");
    if (1)
    {
        string jsonLine;
        regex r("\n");
        while (getline(ifs, jsonLine))
        {
            regex_replace(jsonLine, r, "");
            jsonStr += jsonLine;
        }
    }
    value vData;
    string err = parse(vData, jsonStr);
    if (!err.empty())
    {
        cerr << "json model doesn't work" << endl;
        return;
    }

    if (vData.is<picojson::array>())
    {
        int i = 0;
        picojson::array &arr = vData.get<picojson::array>();
        for (const value &element : arr)
        {
            if (element.get("x").is<double>())
                posX[i] = element.get("x").get<double>();
            if (element.get("y").is<double>())
                posY[i] = element.get("y").get<double>();
            ++i;
        }
    }

    for (int i = 0; i < numButton; ++i)
    {
        cout << "pos X: " << posX[i] << "\tY: " << posY[i] << endl;
    }
}

void simulateMouseClick(int x, int y)
{
    int fixedX = x, fixedY = y;

    cout << "prepare pos: (" << fixedX << ", " << fixedY << ")" << endl;

    XEvent event;
    memset(&event, 0, sizeof(event));

    event.xbutton.button = Button1;
    event.xbutton.same_screen = True;
    event.xbutton.x = x;
    event.xbutton.y = y;
    event.xbutton.subwindow = window;

    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window, &event.xbutton.root,
                      &event.xbutton.subwindow, &event.xbutton.x_root,
                      &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }

    event.xbutton.x = fixedX;
    event.xbutton.y = fixedY;

    event.type = ButtonPress;
    if (XSendEvent(display, PointerWindow, True, ButtonPressMask, &event) == 0)
    {
        fprintf(stderr, "Error sending event\n");
    }

    event.type = ButtonRelease;
    if (XSendEvent(display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
    {
        fprintf(stderr, "Error sending event\n");
    }

    XFlush(display);
}

void raiseID(size_t id)
{
    display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "Unable to open display\n");
        return;
    }

    screen = DefaultScreen(display);
    window = RootWindow(display, screen);

    // 获取目标窗口的窗口ID，替换为实际的窗口ID
    Window targetWindow = id;

    // 设置目标窗口为活动窗口
    XRaiseWindow(display, targetWindow);

    XCloseDisplay(display);
}

void click(int x, int y)
{
    display = XOpenDisplay(NULL);

    simulateMouseClick(x, y);

    XCloseDisplay(display);
}