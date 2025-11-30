/**
 * CrossInput Interactive Test
 *
 * This file contains interactive tests for the CrossInput library.
 * These tests require user interaction and are meant to be run manually
 * to verify that input simulation is working correctly.
 *
 * WARNING: These tests will actually simulate keyboard and mouse input!
 * Make sure you have a safe environment before running them.
 */

#include "../include/CrossInput.h"

// Ensure the CrossInput namespace is used
using namespace CrossInput;
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

void sleep_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printHeader(const std::string &title)
{
    std::cout << "\n========================================" << std::endl;
    std::cout << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void waitForUser()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

// =============================================================================
// INTERACTIVE TESTS
// =============================================================================

void test_CursorMovement()
{
    printHeader("Cursor Movement Test");
    std::cout << "This test will move your cursor in a square pattern." << std::endl;
    std::cout << "Watch your cursor move!" << std::endl;
    waitForUser();

    CrossInput::Point start = CrossInput::GetCursorPosition();
    std::cout << "Starting position: (" << start.x << ", " << start.y << ")" << std::endl;
    if (start.x == 0 && start.y == 0)
    {
        std::cout << "(Note: Position is 0,0 - this may indicate pure Wayland without XWayland)" << std::endl;
    }

    // Test absolute positioning (works if portal grants absolute pointer)
    std::cout << "\nTest 1: Absolute positioning to corners of screen..." << std::endl;

    CrossInput::SetCursorPosition({500, 300});
    sleep_ms(500);
    std::cout << "Moved to (500, 300)" << std::endl;

    CrossInput::SetCursorPosition({600, 300});
    sleep_ms(500);
    std::cout << "Moved to (600, 300)" << std::endl;

    CrossInput::SetCursorPosition({600, 400});
    sleep_ms(500);
    std::cout << "Moved to (600, 400)" << std::endl;

    CrossInput::SetCursorPosition({500, 400});
    sleep_ms(500);
    std::cout << "Moved to (500, 400)" << std::endl;

    CrossInput::SetCursorPosition({500, 300});
    sleep_ms(500);
    std::cout << "Back to (500, 300)" << std::endl;

    // Test relative movement
    std::cout << "\nTest 2: Relative movement (works if rel pointer available)..." << std::endl;

    CrossInput::MoveCursor(100, 0);
    sleep_ms(500);
    std::cout << "Moved right (+100, 0)" << std::endl;

    CrossInput::MoveCursor(0, 100);
    sleep_ms(500);
    std::cout << "Moved down (0, +100)" << std::endl;

    CrossInput::MoveCursor(-100, 0);
    sleep_ms(500);
    std::cout << "Moved left (-100, 0)" << std::endl;

    CrossInput::MoveCursor(0, -100);
    sleep_ms(500);
    std::cout << "Moved up (0, -100)" << std::endl;

    CrossInput::Point end = CrossInput::GetCursorPosition();
    std::cout << "\nFinal position: (" << end.x << ", " << end.y << ")" << std::endl;

    std::cout << "Cursor movement test complete!" << std::endl;
}

void test_KeyPressMonitor()
{
    printHeader("Key Press Monitor Test");
    std::cout << "This test will monitor key presses for 10 seconds." << std::endl;
    std::cout << "Press some keys and see if they are detected!" << std::endl;
    waitForUser();

    std::vector<std::pair<CrossInput::KeyCode, std::string>> keysToMonitor = {
        {CrossInput::KeyCode::KEY_A, "A"},
        {CrossInput::KeyCode::KEY_S, "S"},
        {CrossInput::KeyCode::KEY_D, "D"},
        {CrossInput::KeyCode::KEY_W, "W"},
        {CrossInput::KeyCode::KEY_SPACE, "Space"},
        {CrossInput::KeyCode::KEY_SHIFT, "Shift"},
        {CrossInput::KeyCode::KEY_CONTROL, "Control"},
        {CrossInput::KeyCode::KEY_ESCAPE, "Escape"},
    };

    std::cout << "Monitoring keys: ";
    for (const auto &key : keysToMonitor)
    {
        std::cout << key.second << " ";
    }
    std::cout << "\n"
              << std::endl;

    auto startTime = std::chrono::steady_clock::now();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

        if (elapsed >= 10)
            break;

        std::string pressed;
        for (const auto &key : keysToMonitor)
        {
            if (CrossInput::IsKeyPressed(key.first))
            {
                pressed += key.second + " ";
            }
        }

        if (!pressed.empty())
        {
            std::cout << "\rPressed: " << pressed << "                    " << std::flush;
        }
        else
        {
            std::cout << "\rTime remaining: " << (10 - elapsed) << "s - Press monitored keys...   " << std::flush;
        }

        sleep_ms(50);
    }

    std::cout << "\n\nKey press monitor test complete!" << std::endl;
}

void test_KeyboardSimulation()
{
    printHeader("Keyboard Simulation Test");
    std::cout << "This test will simulate typing 'hello' after 3 seconds." << std::endl;
    std::cout << "Please click on a text editor or text field!" << std::endl;
    waitForUser();

    std::cout << "Typing in 3 seconds..." << std::endl;
    sleep_ms(1000);
    std::cout << "2..." << std::endl;
    sleep_ms(1000);
    std::cout << "1..." << std::endl;
    sleep_ms(1000);
    std::cout << "Typing!" << std::endl;

    // Type "hello"
    std::vector<CrossInput::KeyCode> helloKeys = {
        CrossInput::KeyCode::KEY_H,
        CrossInput::KeyCode::KEY_E,
        CrossInput::KeyCode::KEY_L,
        CrossInput::KeyCode::KEY_L,
        CrossInput::KeyCode::KEY_O,
    };

    for (auto key : helloKeys)
    {
        CrossInput::KeyDown(key);
        sleep_ms(50);
        CrossInput::KeyUp(key);
        sleep_ms(100);
    }

    std::cout << "Keyboard simulation test complete!" << std::endl;
    std::cout << "Did you see 'hello' typed in your text field?" << std::endl;
}

void test_MouseClickSimulation()
{
    printHeader("Mouse Click Simulation Test");
    std::cout << "This test will simulate mouse clicks at the current cursor position." << std::endl;
    std::cout << "Position your cursor where you want to click!" << std::endl;
    waitForUser();

    CrossInput::Point pos = CrossInput::GetCursorPosition();
    std::cout << "Current position: (" << pos.x << ", " << pos.y << ")" << std::endl;

    std::cout << "\nLeft click in 2 seconds..." << std::endl;
    sleep_ms(2000);
    CrossInput::MouseClick(CrossInput::MouseButton::Left);
    std::cout << "Left click sent!" << std::endl;

    sleep_ms(1000);

    std::cout << "\nRight click in 2 seconds..." << std::endl;
    sleep_ms(2000);
    CrossInput::MouseClick(CrossInput::MouseButton::Right);
    std::cout << "Right click sent!" << std::endl;

    std::cout << "\nMouse click simulation test complete!" << std::endl;
}

// =============================================================================
// MAIN MENU
// =============================================================================

void printMenu()
{
    printHeader("CrossInput Interactive Test Suite");
    std::cout << "Platform: " << CrossInput::GetPlatformName() << std::endl;
    std::cout << "\nSelect a test to run:" << std::endl;
    std::cout << "1. Cursor Movement Test" << std::endl;
    std::cout << "2. Key Press Monitor Test" << std::endl;
    std::cout << "3. Keyboard Simulation Test (CAUTION: simulates typing)" << std::endl;
    std::cout << "4. Mouse Click Simulation Test (CAUTION: simulates clicks)" << std::endl;
    std::cout << "5. Run All Tests" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "\nEnter your choice: ";
}

int main()
{
    std::string platform = CrossInput::GetPlatformName();

    if (platform.find("Hybrid") != std::string::npos)
    {
        std::cout << "NOTE: Running in Hybrid mode (Wayland + XWayland)." << std::endl;
        std::cout << "- Cursor movement & input: via libei (native Wayland)" << std::endl;
        std::cout << "- Cursor position & key state: via XWayland" << std::endl;
    }
    else if (platform.find("Wayland") != std::string::npos && platform.find("libei") == std::string::npos)
    {
        std::cout << "WARNING: Running on Wayland without libei support!" << std::endl;
        std::cout << "Consider running under X11 for full functionality." << std::endl;
    }
    else if (platform.find("Wayland") != std::string::npos)
    {
        std::cout << "NOTE: Running on Wayland with libei support." << std::endl;
    }

    if (platform == "Unsupported")
    {
        std::cout << "ERROR: Unsupported platform. Tests cannot run." << std::endl;
        return 1;
    }
    int choice;
    while (true)
    {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear newline

        switch (choice)
        {
        case 0:
            std::cout << "Goodbye!" << std::endl;
            return 0;
        case 1:
            test_CursorMovement();
            break;
        case 2:
            test_KeyPressMonitor();
            break;
        case 3:
            test_KeyboardSimulation();
            break;
        case 4:
            test_MouseClickSimulation();
            break;
        case 5:
            test_CursorMovement();
            test_KeyPressMonitor();
            test_KeyboardSimulation();
            test_MouseClickSimulation();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}
