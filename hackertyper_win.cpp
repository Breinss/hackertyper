#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <string>
#include <conio.h>  // For _kbhit() and _getch()
#include <Windows.h> // For Windows-specific functions
#include <regex>
#include <dirent.h>

// Function to list all matching files in a directory
std::vector<std::string> listMatchingFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> result;
    DIR* dir;
    struct dirent* entry;
    std::regex filePattern(pattern);
    
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;
            if (std::regex_match(filename, filePattern)) {
                result.push_back(directory + "/" + filename);
            }
        }
        closedir(dir);
    }
    
    return result;
}

// Function to read text from file
std::string readTextFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to set up MS-DOS retro style
void setupMSDOSStyle() {
    // Clear screen (Windows)
    system("cls");
    
    // Set console text color to light gray (DOS-like)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    
    // Print DOS-like header
    std::cout << "C:\\>HACK.EXE" << std::endl;
    std::cout << "Microsoft(R) MS-DOS(R) Version 6.22" << std::endl;
    std::cout << "(C)Copyright Microsoft Corp 1981-1994." << std::endl << std::endl;
    std::cout << "Initializing system breach protocol..." << std::endl;
    std::cout << "Establishing secure connection..." << std::endl << std::endl;
}

// Function to display text with proper formatting
void displayText(const std::string& text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Set bright green color (10 is green, FOREGROUND_INTENSITY makes it bright)
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    
    std::istringstream stream(text);
    std::string line;
    
    while(std::getline(stream, line)) {
        std::cout << line << std::endl;
    }
    
    // Reset color
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int main(int argc, char* argv[]) {
    // Set default characters per keystroke
    int charsToAdd = 5;
    
    // Allow command-line override
    if (argc > 1) {
        charsToAdd = std::atoi(argv[1]);
        if (charsToAdd <= 0) charsToAdd = 5;
    }
    
    // Get all hackertext files
    std::vector<std::string> hackerTextFiles = listMatchingFiles(".", "hackertext[0-9]*.txt");
    
    // If no files found, look for the original file
    if (hackerTextFiles.empty()) {
        hackerTextFiles.push_back("hackertext.txt");
    }
    
    // Randomly select a file
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> fileDist(0, hackerTextFiles.size() - 1);
    std::string selectedFile = hackerTextFiles[fileDist(gen)];
    
    // Read text from file
    std::string str = readTextFromFile(selectedFile);
    if (str.empty()) {
        std::cerr << "Failed to read text file or file is empty." << std::endl;
        return 1;
    }

    int i = 0;
    std::string text = "";

    // Setup MS-DOS style interface
    setupMSDOSStyle();
    
    // Show prompt with blinking cursor
    std::cout << "C:\\HACK>";
    std::cout.flush();
    
    // Short delay to simulate system loading
    Sleep(500);

    // Main loop - run until user presses Esc
    bool running = true;
    while (running) {
        // Check for keyboard input (non-blocking)
        if (_kbhit()) {
            char c = _getch();
            
            // Exit if Escape key is pressed
            if (c == 27) {
                running = false;
            } else {
                // Clear screen
                system("cls");
                
                // Reset text color for DOS-like look
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                
                // DOS-like header
                std::cout << "C:\\HACK>DECRYPT.EXE" << std::endl;
                std::cout << "SCANNING NETWORK..." << std::endl << std::endl;
                
                // Add more text when any key is pressed
                for (int j = 0; j < charsToAdd && i < str.length(); j++, i++) {
                    text += str[i];
                }
                
                if (i >= str.length()) {
                    i = 0;
                }
                
                // Display text with proper formatting
                displayText(text);
                
                // Add blinking cursor at the end
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                std::cout << "_" << std::endl;
            }
        }
        
        // Sleep a bit to prevent high CPU usage
        Sleep(10);
    }

    // Reset console colors
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    system("cls");
    
    return 0;
}