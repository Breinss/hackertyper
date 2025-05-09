#include <bits/types/struct_timeval.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <dirent.h>
#include <regex>

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
    // Clear screen
    system("clear");
    
    // Set light gray text (DOS-like colors) without blue background
    std::cout << "\033[0m\033[37m";
    
    // Print DOS-like header
    std::cout << "C:\\>HACK.EXE" << std::endl;
    std::cout << "Microsoft(R) MS-DOS(R) Version 6.22" << std::endl;
    std::cout << "(C)Copyright Microsoft Corp 1981-1994." << std::endl << std::endl;
    std::cout << "Initializing system breach protocol..." << std::endl;
    std::cout << "Establishing secure connection..." << std::endl << std::endl;
}

// Function to display text with proper formatting
void displayText(const std::string& text) {
    std::istringstream stream(text);
    std::string line;
    std::cout << "\033[32;1m"; // Set bright green color
    
    while(std::getline(stream, line)) {
        std::cout << line << std::endl;
    }
    
    std::cout << "\033[0m"; // Reset color
}

int main(int argc, char* argv[]) {
    // Set default characters per keystroke
    int charsToAdd = 5; // Increased from 3 to 5 for more characters per keystroke
    
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
    
    // Read text from the randomly selected file
    std::string str = readTextFromFile(selectedFile);
    if (str.empty()) {
        std::cerr << "Failed to read text file or file is empty." << std::endl;
        return 1;
    }

    struct termios oldSettings, newSettings;

    int i = 0;
    std::string text = "";

    // Save current terminal settings
    tcgetattr(fileno(stdin), &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &newSettings);

    // Setup MS-DOS style interface
    setupMSDOSStyle();
    
    // Show prompt with blinking cursor
    std::cout << "C:\\HACK>";
    std::cout.flush();
    
    // Short delay to simulate system loading
    usleep(500000);

    // Main loop - run until user presses Ctrl+C
    bool running = true;
    while (running) {
        // Non-blocking check for input
        fd_set readfds;
        struct timeval tv;
        
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        tv.tv_sec = 0;
        tv.tv_usec = 1000; // Very small timeout

        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
            char c;
            if (read(STDIN_FILENO, &c, 1) > 0) {
                // Exit if Ctrl+C is pressed
                if (c == 3) {
                    running = false;
                } else {
                    // Clear screen
                    system("clear");
                    
                    // Reset text color for DOS-like look (no blue background)
                    std::cout << "\033[0m\033[37m";
                    
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
                    std::cout << "\033[37m_\033[0m" << std::endl;
                }
            }
        }
    }

    // Restore terminal settings and colors
    std::cout << "\033[0m";
    system("clear");
    tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
    return 0;
}