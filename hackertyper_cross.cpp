#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <string>
#include <regex>
#include <dirent.h>
#include <libgen.h>  // Add this for dirname()
#include <unistd.h>  // Add this for readlink()

// OS-specific includes
#ifdef _WIN32
    #include <conio.h>  // For _kbhit() and _getch()
    #include <Windows.h> // For Windows-specific functions
#else
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
#endif
struct fullpath;

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
                struct fullpath;
                std::string fullpath = directory + "/" + filename;
                std::cout << "Found file: " << fullpath << std::endl;
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

// Get executable directory
std::string getExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        std::string path(result, count);
        char* dir = dirname(&path[0]);
        return std::string(dir);
    }
    return "."; // Fallback to current directory if we can't determine
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

// Function to display a progress bar
void showProgressBar(const std::string& label, int duration_ms) {
    std::cout << label;
    const int width = 30;
    for (int i = 0; i < width; i++) {
        std::cout << "█";
        std::cout.flush();
        sleep_ms(duration_ms / width);
    }
    std::cout << " [COMPLETE]" << std::endl;
}

// Function to simulate typing text
void typeText(const std::string& text, int delay_ms) {
    for (char c : text) {
        std::cout << c;
        std::cout.flush();
        sleep_ms(delay_ms);
    }
    std::cout << std::endl;
}

// Function to display a fake error
void showFakeError() {
    #ifdef _WIN32
    setTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    #else
    std::cout << "\033[31;1m"; // Set bright red color on Unix/Linux
    #endif
    
    std::cout << "\n*** ERROR 0x" << std::hex << (rand() % 0xFFFF) << ": Connection terminated" << std::endl;
    std::cout << "*** Recalibrating network parameters..." << std::endl;
    sleep_ms(1000);
    std::cout << "*** Attempting bypass sequence..." << std::endl;
    sleep_ms(800);
    std::cout << "*** Rerouting through secondary node..." << std::endl;
    sleep_ms(1200);
    std::cout << "*** Connection reestablished" << std::endl << std::endl;
    
    resetTextColor();
}

// Function to display matrix-style rain effect
void showMatrixRain(int duration_ms) {
    clearScreen();
    setGreenText();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> charDist(33, 126); // ASCII printable characters
    std::uniform_int_distribution<> posDist(0, 79);    // Screen width
    
    int end_time = duration_ms;
    int current_time = 0;
    int interval = 50; // Update interval in milliseconds
    
    while (current_time < end_time) {
        // Clear screen occasionally
        if (current_time % 500 == 0) {
            clearScreen();
        }
        
        // Display random characters
        for (int i = 0; i < 10; i++) {
            int x = posDist(gen);
            char c = charDist(gen);
            // Move cursor to random position and print char
            std::cout << "\033[" << (current_time/100) % 24 << ";" << x << "H" << c;
        }
        
        std::cout.flush();
        sleep_ms(interval);
        current_time += interval;
    }
    
    clearScreen();
    resetTextColor();
}

// Function to simulate IP scanning
void simulateIPScan() {
    setGreenText();
    std::cout << "\nINITIATING NETWORK SCAN...\n" << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ipDist(1, 255);
    
    for (int i = 0; i < 8; i++) {
        std::string ip = std::to_string(ipDist(gen)) + "." + 
                          std::to_string(ipDist(gen)) + "." + 
                          std::to_string(ipDist(gen)) + "." + 
                          std::to_string(ipDist(gen));
        std::cout << "Probing " << ip << "... ";
        sleep_ms(200);
        
        if (i == 5 || i == 7) { // Make a couple of IPs "vulnerable"
            std::cout << "VULNERABLE" << std::endl;
            sleep_ms(300);
            std::cout << "  └─ Port 22: OPEN (SSH)" << std::endl;
            sleep_ms(100);
            std::cout << "  └─ Port 80: OPEN (HTTP)" << std::endl;
            if (i == 7) {
                sleep_ms(100);
                std::cout << "  └─ Port 3306: OPEN (MySQL)" << std::endl;
                sleep_ms(300);
                std::cout << "\nTARGET SELECTED: " << ip << std::endl;
            }
        } else {
            std::cout << "SECURE" << std::endl;
        }
    }
    
    resetTextColor();
}

// Function to show exit reminder
void showExitReminder() {
    int origRow, origCol;
    // Save current cursor position (simplified)
    origRow = 23; // Assuming a 24-line terminal
    origCol = 0;
    
    // Go to bottom of screen
    std::cout << "\033[" << origRow << ";0H";
    setGrayText();
    std::cout << "[ Press ESC to exit ]";
    std::cout.flush();
    
    // Restore cursor position
    std::cout << "\033[" << origRow - 3 << ";" << origCol << "H";
    resetTextColor();
}

// Function to set up MS-DOS retro style
void setupMSDOSStyle() {
    clearScreen();
    
    // Set light gray text (DOS-like colors)
    setGrayText();
    std::cout << std::endl;

    // Print DOS-like header
    std::cout << "C:\\>HACK.EXE" << std::endl;
    std::cout << "Microsoft(R) MS-DOS(R) Version 6.22" << std::endl;
    std::cout << "(C)Copyright Microsoft Corp 1981-1994." << std::endl << std::endl;
    
    // Show progress indicators
    typeText("Initializing system breach protocol...", 30);
    showProgressBar("Loading encryption modules: ", 1200);
    showProgressBar("Establishing secure connection: ", 800);
    
    // Add IP scanning for immersion
    simulateIPScan();
    
    // Matrix effect transition
    showMatrixRain(2000);
    
    // Show final screen
    clearScreen();
    setGrayText();
    std::cout << "C:\\>HACK.EXE" << std::endl;
    std::cout << "BREACH PROTOCOL INITIALIZED" << std::endl;
    std::cout << "SYSTEM ACCESS: GRANTED" << std::endl << std::endl;
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
    
    // Get the executable directory
    std::string exeDir = getExecutableDir();
    
    // Define search paths in order of preference
    std::vector<std::string> searchPaths = {
        ".",                          // Current directory
        exeDir,                       // Executable directory
        "/usr/local/share/hackertyper", // System-wide data directory
        "/usr/share/hackertyper"      // Alternative system-wide data directory
    };
    
    std::vector<std::string> hackerTextFiles;
    
    // Try each search path until we find files
    for (const auto& path : searchPaths) {
        hackerTextFiles = listMatchingFiles(path, "hackertext[0-9]*.txt");
        if (!hackerTextFiles.empty()) {
            std::cout << "Found files in: " << path << std::endl;
            break;
        }
    }
    
    // If no files found, look for the original file in each path
    if (hackerTextFiles.empty()) {
        for (const auto& path : searchPaths) {
            std::string originalFile = path + "/hackertext.txt";
            std::ifstream test(originalFile);
            if (test.good()) {
                hackerTextFiles.push_back(originalFile);
                std::cout << "Using default file: " << originalFile << std::endl;
                break;
            }
        }
    }
    
    // If still no files found, give up
    if (hackerTextFiles.empty()) {
        std::cerr << "Error: Cannot find any hackertext files." << std::endl;
        std::cerr << "Please make sure hackertext.txt exists in one of the search paths." << std::endl;
        return 1;
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
    int keyPressCount = 0;
    std::uniform_int_distribution<> errorDist(1, 100);
    
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
                
                // Occasionally show fake errors (about 5% chance after 10 keypresses)
                keyPressCount++;
                if (keyPressCount > 10 && errorDist(gen) <= .5) {
                    showFakeError();
                    keyPressCount = 0;
                }
                
                // Display text with proper formatting
                displayText(text);
                
                // Add blinking cursor at the end
                setGrayText();
                std::cout << "_" << std::endl;
                
                // Show exit reminder
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