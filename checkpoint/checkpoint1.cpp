#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <map>
#include <sstream>

// ANSI escape codes for colors and formatting
namespace AnsiArt {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string BLUE = "\033[34m";
    const std::string YELLOW = "\033[33m";
    const std::string CLEAR_SCREEN = "\033[2J\033[H";

    // Template function specialization for const char*
    template<typename T>
    size_t getLength(const T& text) {
        return std::string(text).length();
    }

    template<typename T>
    void printCentered(const T& text, int width = 80) {
        int padding = (width - getLength(text)) / 2;
        std::cout << std::string(padding, ' ') << text << std::endl;
    }
}

// Rest of the code remains the same as before
class GameObject {
protected:
    std::string name;
    std::string description;

public:
    GameObject() : name("Unknown"), description("No description") {}
    
    GameObject(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
    
    GameObject(const GameObject& other) 
        : name(other.name), description(other.description) {}
    
    virtual ~GameObject() = default;

    virtual void display() const = 0;

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
};

class Character : public GameObject {
protected:
    int health;
    int maxHealth;

public:
    Character(const std::string& n, const std::string& desc, int h) 
        : GameObject(n, desc), health(h), maxHealth(h) {}

    virtual void display() const override {
        std::cout << AnsiArt::GREEN << "Name: " << name << AnsiArt::RESET << std::endl;
        std::cout << "Health: " << health << "/" << maxHealth << std::endl;
        std::cout << "Description: " << description << std::endl;
    }

    void takeDamage(int damage) {
        if (damage < 0) {
            throw std::invalid_argument("Damage cannot be negative!");
        }
        health = std::max(0, health - damage);
    }
};

class Player : public Character {
private:
    std::vector<std::string> inventory;

public:
    Player(const std::string& n) 
        : Character(n, "A maintenance worker on Europa", 100) {}

    void addItem(const std::string& item) {
        inventory.push_back(item);
    }

    void display() const override {
        Character::display();
        std::cout << "\nInventory:" << std::endl;
        for (const auto& item : inventory) {
            std::cout << "- " << item << std::endl;
        }
    }
};

class Location {
private:
    std::string name;
    std::string description;
    std::map<std::string, std::string> interactions;

public:
    Location(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}

    void addInteraction(const std::string& key, const std::string& response) {
        interactions[key] = response;
    }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    
    std::string interact(const std::string& key) const {
        auto it = interactions.find(key);
        return (it != interactions.end()) ? it->second : "Nothing interesting happens.";
    }
};

class Game {
private:
    std::unique_ptr<Player> player;
    int chapter;
    std::vector<Location> locations;
    bool hasMonolith;
    bool hasAIAccess;
    int currentLocation;

    void displayTitle() {
        std::cout << AnsiArt::CLEAR_SCREEN;
        std::cout << AnsiArt::BLUE;
        AnsiArt::printCentered("================================");
        AnsiArt::printCentered("SPACE DYSTOPIA: THE LAST FRONTIER");
        AnsiArt::printCentered("================================");
        std::cout << AnsiArt::RESET << std::endl;
    }

    void initializeLocations() {
        locations = {
            Location("Maintenance Bay", "A sterile white room filled with repair equipment and spare parts."),
            Location("HAL Terminal Room", "A quiet room with a single terminal. A red light pulses steadily."),
            Location("Monolith Chamber", "A mysterious black rectangular object stands in the center."),
            Location("Airlock", "The gateway between the station and the void of space.")
        };

        locations[0].addInteraction("examine tools", 
            "Among the tools, you find a tablet containing classified information about a signal from beyond Pluto.");
        
        locations[1].addInteraction("talk to computer", 
            "The AI responds in a calm voice: 'I'm sorry, but I can't let you share that information, " + 
            player->getName() + ". This conversation can serve no purpose anymore.'");
        
        locations[2].addInteraction("touch monolith", 
            "As your fingers brush the surface, you feel a strange vibration. Images of a distant habitable world flash through your mind.");

        locations[3].addInteraction("check supplies",
            "You find an emergency spacesuit and enough oxygen for a short EVA (Extra-Vehicular Activity).");
    }

    void typewriterEffect(const std::string& text, int delay_ms = 30) {
        for (char c : text) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
        std::cout << std::endl;
    }

public:
    Game() : chapter(1), hasMonolith(false), hasAIAccess(false), currentLocation(0) {
        displayTitle();
        std::cout << "\nEnter your character's name: ";
        std::string playerName;
        std::getline(std::cin, playerName);
        player = std::make_unique<Player>(playerName);
        initializeLocations();
    }

    void displayLocation() {
        std::cout << AnsiArt::BLUE << "\nLocation: " << locations[currentLocation].getName() 
                  << AnsiArt::RESET << std::endl;
        std::cout << locations[currentLocation].getDescription() << std::endl;
    }

    void run() {
        try {
            displayTitle();
            std::cout << AnsiArt::YELLOW;
            typewriterEffect("\nChapter " + std::to_string(chapter) + ": The Discovery", 50);
            std::cout << AnsiArt::RESET;
            
            typewriterEffect("You are " + player->getName() + 
                           ", a maintenance worker on Europa Station.");
            typewriterEffect("Something doesn't feel right today. The station's AI has been acting... strange.");

            while (true) {
                displayLocation();
                std::cout << "\nOptions:\n";
                std::cout << "1. Examine area\n";
                std::cout << "2. Check status\n";
                std::cout << "3. Move to another location\n";
                std::cout << "4. Interact with environment\n";
                std::cout << "5. Quit\n";
                
                int choice;
                std::cout << "\nEnter your choice (1-5): ";
                std::cin >> choice;
                std::cin.ignore();
                
                switch (choice) {
                    case 1: {
                        std::cout << R"(
    [Examining current area...]
         ___________
        /          /|
       /          / |
      /          /  |
     /__________/   |
    |          |    |
    |          |    |
    |          |    /
    |          |   /
    |          |  /
    |          | /
    |          |/
    ------------
)" << std::endl;
                        std::cout << locations[currentLocation].getDescription() << std::endl;
                        break;
                    }
                    case 2:
                        player->display();
                        break;
                    case 3: {
                        std::cout << "\nAvailable locations:\n";
                        for (size_t i = 0; i < locations.size(); ++i) {
                            std::cout << i + 1 << ". " << locations[i].getName() << std::endl;
                        }
                        std::cout << "Choose location (1-" << locations.size() << "): ";
                        int loc;
                        std::cin >> loc;
                        if (loc >= 1 && loc <= static_cast<int>(locations.size())) {
                            currentLocation = loc - 1;
                            if (currentLocation == 1 && !hasAIAccess) {
                                typewriterEffect("The AI's voice echoes through the room...");
                                typewriterEffect("'Welcome to the terminal room. What brings you here today?'");
                                hasAIAccess = true;
                            }
                        }
                        break;
                    }
                    case 4: {
                        std::cout << "What would you like to do? (e.g., 'examine tools', 'talk to computer'): ";
                        std::string action;
                        std::getline(std::cin, action);
                        std::string result = locations[currentLocation].interact(action);
                        typewriterEffect(result);
                        
                        if (action == "touch monolith" && !hasMonolith) {
                            hasMonolith = true;
                            player->addItem("Monolith Knowledge");
                            typewriterEffect("You've gained insight into humanity's next step...");
                        }
                        break;
                    }
                    case 5:
                        return;
                    default:
                        throw std::runtime_error("Invalid choice!");
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << AnsiArt::RED << "Error: " << e.what() << AnsiArt::RESET << std::endl;
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
