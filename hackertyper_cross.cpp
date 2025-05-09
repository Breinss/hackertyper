#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <string>
#include <regex>
#include <dirent.h>

// OS-specific includes
#ifdef _WIN32
    #include <conio.h>  // For _kbhit() and _getch()
    #include <Windows.h> // For Windows-specific functions
#else
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
#endif

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

// Cross-platform functions for terminal handling
#ifdef _WIN32
// Windows version
void setupTerminal() {
    // Nothing needed for Windows
}

void resetTerminal() {
    // Nothing needed for Windows
}

bool kbhit() {
    return _kbhit();
}

char getch() {
    return _getch();
}

void sleep_ms(int ms) {
    Sleep(ms);
}

void clearScreen() {
    system("cls");
}

void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void setGreenText() {
    setTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

void setGrayText() {
    setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void resetTextColor() {
    setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

#else
// Linux/Unix version
struct termios oldSettings, newSettings;

void setupTerminal() {
    tcgetattr(fileno(stdin), &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &newSettings);
}

void resetTerminal() {
    tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
}

bool kbhit() {
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0;
}

char getch() {
    char buf = 0;
    if (read(STDIN_FILENO, &buf, 1) > 0) {
        return buf;
    }
    return 0;
}

void sleep_ms(int ms) {
    usleep(ms * 1000);
}

void clearScreen() {
    system("clear");
}

void setGreenText() {
    std::cout << "\033[32;1m"; // Set bright green color
}

void setGrayText() {
    std::cout << "\033[0m\033[37m"; // Set light gray text
}

void resetTextColor() {
    std::cout << "\033[0m"; // Reset color
}
#endif

// Function to set up MS-DOS retro style
void setupMSDOSStyle() {
    clearScreen();
    
    // Set light gray text (DOS-like colors)
    setGrayText();
    
    // Print DOS-like header
    std::cout << "C:\\>HACK.EXE" << std::endl;
    std::cout << "Microsoft(R) MS-DOS(R) Version 6.22" << std::endl;
    std::cout << "(C)Copyright Microsoft Corp 1981-1994." << std::endl << std::endl;
    std::cout << "Initializing system breach protocol..." << std::endl;
    std::cout << "Establishing secure connection..." << std::endl << std::endl;
}

// Function to display text with proper formatting
void displayText(const std::string& text) {
    setGreenText();
    
    std::istringstream stream(text);
    std::string line;
    
    while(std::getline(stream, line)) {
        std::cout << line << std::endl;
    }
    
    resetTextColor();
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
    
    // Read text from the randomly selected file
    std::string str = readTextFromFile(selectedFile);
    if (str.empty()) {
        std::cerr << "Failed to read text file or file is empty." << std::endl;
        return 1;
    }

    int i = 0;
    std::string text = "";

    // Set up terminal for non-blocking input
    setupTerminal();

    // Setup MS-DOS style interface
    setupMSDOSStyle();
    
    // Show prompt with blinking cursor
    std::cout << "C:\\HACK>";
    std::cout.flush();
    
    // Short delay to simulate system loading
    sleep_ms(500);

    // Main loop - run until user presses Esc (Windows) or Ctrl+C (Linux)
    bool running = true;
    while (running) {
        // Check for keyboard input (non-blocking)
        if (kbhit()) {
            char c = getch();
            
            // Exit if Escape key is pressed (Windows) or Ctrl+C (Linux)
            #ifdef _WIN32
            if (c == 27) {
                running = false;
            }
            #else
            if (c == 3) {  // Ctrl+C
                running = false;
            }
            #endif
            else {
                // Clear screen
                clearScreen();
                
                // Reset text color for DOS-like look
                setGrayText();
                
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
                setGrayText();
                std::cout << "_" << std::endl;
            }
        }
        
        // Sleep a bit to prevent high CPU usage
        sleep_ms(10);
    }

    // Reset terminal settings and colors
    resetTextColor();
    clearScreen();
    resetTerminal();
    
    return 0;
}