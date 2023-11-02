#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unistd.h>

// Constants
const int DEFAULT_K_STREETS = 10;
const int DEFAULT_K_SEGMENTS = 5;
const int DEFAULT_K_WAIT = 5;
const int DEFAULT_K_COORDINATES = 20;
const int MAX_ATTEMPTS = 25;

// Function to generate random integer in [min, max]
int getRandomInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Function to generate a random (x, y) coordinate
std::pair<int, int> generateCoordinate(int k) {
    int x = getRandomInt(-k, k);
    int y = getRandomInt(-k, k);
    return std::make_pair(x, y);
}

// Function to check if a coordinate is already in the list
bool isCoordinateInList(const std::vector<std::pair<int, int>>& coordinates, const std::pair<int, int>& coord) {
    for (const auto& c : coordinates) {
        if (c == coord) {
            return true;
        }
    }
    return false;
}

// Function to generate a random street name
std::string generateStreetName() {
    // Generate a random name for the street
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string name = "Street ";
    for (int i = 0; i < 6; i++) {
        name += alphabet[getRandomInt(0, 25)];
    }
    return name;
}

int main(int argc, char* argv[]) {
    int k_streets = DEFAULT_K_STREETS;
    int k_segments = DEFAULT_K_SEGMENTS;
    int k_wait = DEFAULT_K_WAIT;
    int k_coordinates = DEFAULT_K_COORDINATES;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-s" && i + 1 < argc) {
            k_streets = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-n" && i + 1 < argc) {
            k_segments = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-l" && i + 1 < argc) {
            k_wait = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-c" && i + 1 < argc) {
            k_coordinates = std::atoi(argv[i + 1]);
        }
    }

    // Initialize random seed
    std::srand(std::time(0));

    // Loop to generate street specifications
    for (int streetIndex = 1; streetIndex <= k_streets; streetIndex++) {
        // Sleep for a random duration
        int waitTime = getRandomInt(5, k_wait);
        sleep(waitTime);

        // Generate random street name
        std::string streetName = generateStreetName();

        // Generate unique random coordinates for line segments
        std::vector<std::pair<int, int>> coordinates;
        for (int segmentIndex = 0; segmentIndex < k_segments; segmentIndex++) {
            int attempts = 0;
            std::pair<int, int> coord;
            do {
                coord = generateCoordinate(k_coordinates);
                attempts++;
            } while (isCoordinateInList(coordinates, coord) && attempts < MAX_ATTEMPTS);

            if (attempts >= MAX_ATTEMPTS) {
                std::cerr << "Error: failed to generate valid input for " << MAX_ATTEMPTS << " simultaneous attempts" << std::endl;
                return 1;
            }

            coordinates.push_back(coord);
        }

        // Print the street specification
        std::cout << "add \"" << streetName << "\"";
        for (const auto& coord : coordinates) {
            std::cout << " (" << coord.first << "," << coord.second << ")";
        }
        std::cout << std::endl;
    }

    return 0;
}