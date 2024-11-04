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
#include <random>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <set>

void typewriterEffect(const std::string& text, int delayMs = 30) {
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    std::cout << std::endl;
}

// Template class for handling game statistics
template<typename T>
class Stat {
private:
    T current;
    T maximum;
    std::string name;

public:
    Stat(const std::string& statName, T initial) 
        : current(initial), maximum(initial), name(statName) {}

    T getCurrent() const { return current; }
    T getMaximum() const { return maximum; }
    std::string getName() const { return name; }

    void modify(T amount) {
        current = std::min(maximum, current + amount);
        current = std::max(T(0), current);
    }

    friend std::ostream& operator<<(std::ostream& os, const Stat<T>& stat) {
        os << stat.name << ": " << stat.current << "/" << stat.maximum;
        return os;
    }
};

// ANSI escape codes for colors and formatting
namespace AnsiArt {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string BLUE = "\033[34m";
    const std::string YELLOW = "\033[33m";
    const std::string CLEAR_SCREEN = "\033[2J\033[H";

    // Template function for centering text
    template<typename T>
    void printCentered(const T& text, int width = 80) {
        std::string str = std::string(text);
        int padding = (width - str.length()) / 2;
        std::cout << std::string(padding, ' ') << str << std::endl;
    }

    class AsciiArt {
    public:
        static void drawSpacestation() {
            std::cout << R"(
     _____
    /=====/\
   /=====/  \
  /=====/    \
 /=====/      \
(=================)
 \====/        /
  \==/        /
   \/________/
)" << std::endl;
        }

        static void drawMonolith() {
            std::cout << R"(
    ____________
   |            |
   |            |
   |            |
   |            |
   |            |
   |            |
   |            |
   |____________|
)" << std::endl;
        }
    };
}

// Abstract base class demonstrating polymorphism
class GameObject {
protected:
    std::string name;
    std::string description;

public:
    // Default constructor
    GameObject() : name("Unknown"), description("No description") {}
    
    // Parameterized constructor
    GameObject(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
    
    // Copy constructor
    GameObject(const GameObject& other) 
        : name(other.name), description(other.description) {}
    
    // Virtual destructor
    virtual ~GameObject() = default;

    // Pure virtual function demonstrating polymorphism
    virtual void display() const = 0;
    virtual void update() {}

    // Getters
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
};

// Item class demonstrating inheritance
class Item : public GameObject {
private:
    bool isUsable;
    bool isPickable;
    std::function<void()> useEffect;
    std::string useDescription;

public:
    Item(const std::string& n, const std::string& desc, bool usable = false, bool pickable = true) 
        : GameObject(n, desc), isUsable(usable), isPickable(pickable), useDescription("No specific use instructions.") {}

    void setUseEffect(std::function<void()> effect, const std::string& useDesc) {
        useEffect = effect;
        useDescription = useDesc;
        isUsable = true;
    }

    bool canUse() const { return isUsable; }
    bool canPickup() const { return isPickable; }
    std::string getUseDescription() const { return useDescription; }
    
    void use() {
        if (isUsable && useEffect) {
            useEffect();
        }
    }

    void display() const override {
        std::cout << AnsiArt::YELLOW << "Item: " << name << AnsiArt::RESET << std::endl;
        std::cout << description << std::endl;
        if (isUsable) {
            std::cout << "Usage: " << useDescription << std::endl;
        }
        if (isPickable) {
            std::cout << "(Can be picked up)" << std::endl;
        }
    }
};

class Character : public GameObject {
protected:
    Stat<int> health;
    Stat<int> energy;
    std::vector<std::shared_ptr<Item>> inventory;

public:
    Character(const std::string& n, const std::string& desc, int h, int e) 
        : GameObject(n, desc), health("Health", h), energy("Energy", e) {}

    virtual void display() const override {
        std::cout << AnsiArt::GREEN << "Name: " << name << AnsiArt::RESET << std::endl;
        std::cout << health << std::endl;
        std::cout << energy << std::endl;
        std::cout << "Description: " << description << std::endl;
    }

    void takeDamage(int damage) {
        try {
            if (damage < 0) {
                throw std::invalid_argument("Damage cannot be negative!");
            }
            health.modify(-damage);
        } catch (const std::exception& e) {
            std::cerr << "Error in takeDamage: " << e.what() << std::endl;
        }
    }

    void addItem(std::shared_ptr<Item> item) {
        inventory.push_back(item);
    }

    const std::vector<std::shared_ptr<Item>>& getInventory() const {
        return inventory;
    }
};

class Player : public Character {
private:
    int experience;
    std::map<std::string, bool> questFlags;
    std::set<std::string> discoveredInteractions;
    int totalSteps;
    int itemsCollected;

public:
    Player(const std::string& n) 
        : Character(n, "A maintenance worker on Europa", 100, 100), 
          experience(0), totalSteps(0), itemsCollected(0) {}

    void display() const override {
        Character::display();
        std::cout << "\nExperience: " << experience << std::endl;
        std::cout << "Total steps taken: " << totalSteps << std::endl;
        std::cout << "Items collected: " << itemsCollected << std::endl;
        
        std::cout << "\nInventory:" << std::endl;
        if (inventory.empty()) {
            std::cout << "Empty" << std::endl;
        } else {
            for (const auto& item : inventory) {
                std::cout << "- " << item->getName() << std::endl;
            }
        }

        std::cout << "\nDiscovered interactions:" << std::endl;
        if (discoveredInteractions.empty()) {
            std::cout << "None yet" << std::endl;
        } else {
            for (const auto& interaction : discoveredInteractions) {
                std::cout << "- " << interaction << std::endl;
            }
        }
    }

    void incrementSteps() { totalSteps++; }
    void incrementItemsCollected() { itemsCollected++; }
    void addDiscoveredInteraction(const std::string& interaction) {
        discoveredInteractions.insert(interaction);
    }

    void gainExperience(int exp) {
        if (exp > 0) {
            experience += exp;
            std::cout << "Gained " << exp << " experience!" << std::endl;
        }
    }

    void setQuestFlag(const std::string& flag) {
        questFlags[flag] = true;
    }

    bool hasQuestFlag(const std::string& flag) const {
        return questFlags.count(flag) > 0;
    }
};

class Location {
private:
    std::string name;
    std::string description;
    std::map<std::string, std::string> interactions;
    std::vector<std::shared_ptr<Item>> items;
    std::vector<std::string> availableInteractions;

public:
    Location(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}

    void addInteraction(const std::string& key, const std::string& response) {
        interactions[key] = response;
        availableInteractions.push_back(key);
    }

    const std::vector<std::string>& getAvailableInteractions() const {
        return availableInteractions;
    }

    void addItem(std::shared_ptr<Item> item) {
        items.push_back(item);
    }

    void removeItem(const std::string& itemName) {
        auto newEnd = std::remove_if(items.begin(), items.end(),
            [&itemName](const std::shared_ptr<Item>& item) {
                return item->getName() == itemName;
            });
        items.erase(newEnd, items.end());
    }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    
    std::string interact(const std::string& key) const {
        auto it = interactions.find(key);
        return (it != interactions.end()) ? it->second : "Nothing interesting happens.";
    }

    std::vector<std::shared_ptr<Item>> getItems() const {
        return items;
    }
};

class Game {
private:
    std::unique_ptr<Player> player;
    int chapter;
    std::vector<Location> locations;
    bool gameOver;
    int currentLocation;

    void displayTitle() {
        std::cout << AnsiArt::CLEAR_SCREEN;
        std::cout << AnsiArt::BLUE;
        AnsiArt::printCentered("================================");
        AnsiArt::printCentered("SPACE DYSTOPIA: THE LAST FRONTIER");
        AnsiArt::printCentered("================================");
        AnsiArt::AsciiArt::drawSpacestation();
        std::cout << AnsiArt::RESET << std::endl;
    }

    void initializeLocations() {
        locations = {
            Location("Maintenance Bay", "A sterile white room filled with repair equipment and spare parts."),
            Location("HAL Terminal Room", "A quiet room with a single terminal. A red light pulses steadily."),
            Location("Monolith Chamber", "A mysterious black rectangular object stands in the center."),
            Location("Airlock", "The gateway between the station and the void of space.")
        };

        // Create items with detailed use effects
        auto datapad = std::make_shared<Item>("Datapad", "A tablet containing classified information", true);
        datapad->setUseEffect([this]() {
            std::cout << "You carefully read through the classified information..." << std::endl;
            std::cout << "The data reveals coordinates for a potentially habitable planet beyond Pluto." << std::endl;
            std::cout << "This could be humanity's last hope for survival." << std::endl;
            player->setQuestFlag("read_classified_info");
        }, "Access classified information about signals from beyond Pluto");

        auto spacesuit = std::make_shared<Item>("Spacesuit", "An emergency EVA suit", true);
        spacesuit->setUseEffect([this]() {
            if (currentLocation == 3) { // Airlock
                std::cout << "You carefully put on the spacesuit, checking all seals..." << std::endl;
                std::cout << "The suit's systems come online, showing green across the board." << std::endl;
                std::cout << "You're now ready for extravehicular activity." << std::endl;
                player->setQuestFlag("spacesuit_equipped");
            } else {
                std::cout << "You need to be at the airlock to use this." << std::endl;
            }
        }, "Put on the suit when you're ready for EVA (only at airlock)");

        auto keycard = std::make_shared<Item>("Keycard", "A security keycard with level 2 clearance", true);
        keycard->setUseEffect([this]() {
            if (currentLocation == 1) { // Terminal Room
                std::cout << "You swipe the keycard through the terminal..." << std::endl;
                std::cout << "Access granted to restricted files." << std::endl;
                player->gainExperience(15);
            } else {
                std::cout << "There's nowhere to use the keycard here." << std::endl;
            }
        }, "Use at terminals to access restricted areas");

        // Add items to locations
        locations[0].addItem(datapad);
        locations[0].addItem(keycard);
        locations[3].addItem(spacesuit);

        // Set up detailed interactions for each location
        locations[0].addInteraction("examine tools", 
            "You methodically search through the tools and equipment. Among them, you find a hidden datapad.");
        locations[0].addInteraction("check workbench",
            "The workbench is cluttered with various repair tools and spare parts.");
        locations[0].addInteraction("look under desk",
            "You find some old maintenance logs and a keycard that might be useful.");
        
        locations[1].addInteraction("talk to computer", 
            "The AI responds in a calm voice: 'I'm sorry, but I can't let you share that information, " + 
            player->getName() + ". This conversation can serve no purpose anymore.'");
        locations[1].addInteraction("examine terminal",
            "The terminal displays various system diagnostics and security protocols.");
        locations[1].addInteraction("check cables",
            "The cables seem to lead to a hidden compartment behind the terminal.");
        
        locations[2].addInteraction("touch monolith", 
            "As your fingers brush the surface, you feel a strange vibration. Images of a distant habitable world flash through your mind.");
        locations[2].addInteraction("examine base",
            "The base of the monolith has strange markings that seem to pulse with an inner light.");
        locations[2].addInteraction("walk around monolith",
            "As you circle the monolith, you notice how it seems to absorb all reflections.");
        
        locations[3].addInteraction("check airlock controls",
            "The airlock controls are functioning normally. Safety protocols are active.");
        locations[3].addInteraction("examine window",
            "Through the reinforced window, you can see the icy surface of Europa stretching to the horizon.");
        locations[3].addInteraction("inspect emergency gear",
            "The emergency gear station contains a spacesuit and other EVA equipment.");
    }

public:
    Game() : chapter(1), gameOver(false), currentLocation(0) {
        try {
            displayTitle();
            std::cout << "\nEnter your character's name: ";
            std::string playerName;
            std::getline(std::cin, playerName);
            
            if (playerName.empty()) {
                throw std::invalid_argument("Name cannot be empty!");
            }
            
            player = std::make_unique<Player>(playerName);
            initializeLocations();
        } catch (const std::exception& e) {
            std::cerr << "Error during game initialization: " << e.what() << std::endl;
            throw;
        }
    }

    void displayLocation() {
        std::cout << AnsiArt::BLUE << "\nLocation: " << locations[currentLocation].getName() 
                  << AnsiArt::RESET << std::endl;
        std::cout << locations[currentLocation].getDescription() << std::endl;
        
        // Display available items
        auto items = locations[currentLocation].getItems();
        if (!items.empty()) {
            std::cout << "\nYou see:" << std::endl;
            for (const auto& item : items) {
                std::cout << "- " << item->getName() << ": " << item->getDescription() << std::endl;
            }
        }

        // Display available interactions
        std::cout << "\nPossible interactions:" << std::endl;
        for (const auto& interaction : locations[currentLocation].getAvailableInteractions()) {
            std::cout << "- " << interaction << std::endl;
        }
    }

    void displayEndGameStats() {
        std::cout << AnsiArt::YELLOW << "\n=== Final Statistics ===" << AnsiArt::RESET << std::endl;
        player->display();
        
        std::cout << "\nChapter reached: " << chapter << std::endl;
        std::cout << "Locations explored: " << currentLocation + 1 << "/" << locations.size() << std::endl;
        
        std::cout << "\nQuest progress:" << std::endl;
        std::cout << "- Read classified info: " << (player->hasQuestFlag("read_classified_info") ? "Yes" : "No") << std::endl;
        std::cout << "- Touched monolith: " << (player->hasQuestFlag("touched_monolith") ? "Yes" : "No") << std::endl;
        std::cout << "- Equipped spacesuit: " << (player->hasQuestFlag("spacesuit_equipped") ? "Yes" : "No") << std::endl;
    }

    void pickupItem() {
        auto items = locations[currentLocation].getItems();
        if (items.empty()) {
            std::cout << "There are no items to pick up here." << std::endl;
            return;
        }

        std::cout << "\nAvailable items to pick up:" << std::endl;
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i + 1 << ". " << items[i]->getName() << ": " << items[i]->getDescription() << std::endl;
        }

        std::cout << "Choose item to pick up (1-" << items.size() << ") or 0 to cancel: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice > 0 && choice <= static_cast<int>(items.size())) {
            auto item = items[choice - 1];
            if (item->canPickup()) {
                player->addItem(item);
                locations[currentLocation].removeItem(item->getName());
                player->incrementItemsCollected();
                std::cout << "Picked up " << item->getName() << std::endl;
                player->gainExperience(5);
            } else {
                std::cout << "This item cannot be picked up." << std::endl;
            }
        }
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

            while (!gameOver) {
                displayLocation();
                std::cout << "\nOptions:\n";
                std::cout << "1. Examine area\n";
                std::cout << "2. Check status\n";
                std::cout << "3. Move to another location\n";
                std::cout << "4. Interact with environment\n";
                std::cout << "5. Pick up item\n";
                std::cout << "6. Use item\n";
                std::cout << "7. Quit\n";
                
                int choice;
                std::cout << "\nEnter your choice (1-7): ";
                std::cin >> choice;
                std::cin.ignore();
                
                switch (choice) {
                    case 1:
                        if (currentLocation == 2) {
                            AnsiArt::AsciiArt::drawMonolith();
                        }
                        std::cout << locations[currentLocation].getDescription() << std::endl;
                        break;
                    case 2:
                        player->display();
                        break;
                    case 3: {
                        std::cout << "\nAvailable locations:\n";
                        for (size_t i = 0; i < locations.size(); ++i) {
                            std::cout << i + 1 << ". " << locations[i].getName() << std::endl;
                        }
                        std::cout << "Choose location (1-" << locations.size() << ") or 0 to cancel: ";
                        int loc;
                        std::cin >> loc;
                        if (loc >= 1 && loc <= static_cast<int>(locations.size())) {
                            currentLocation = loc - 1;
                            player->incrementSteps();
                            player->gainExperience(5);
                        }
                        break;
                    }
                    case 4: {
                        const auto& availableInteractions = locations[currentLocation].getAvailableInteractions();
                        std::cout << "\nAvailable interactions:" << std::endl;
                        for (size_t i = 0; i < availableInteractions.size(); ++i) {
                            std::cout << i + 1 << ". " << availableInteractions[i] << std::endl;
                        }
                        
                        std::cout << "Choose interaction (1-" << availableInteractions.size() << ") or 0 to cancel: ";
                        int interactionChoice;
                        std::cin >> interactionChoice;
                        std::cin.ignore();
                        
                        if (interactionChoice >= 1 && interactionChoice <= static_cast<int>(availableInteractions.size())) {
                            std::string action = availableInteractions[interactionChoice - 1];
                            std::string result = locations[currentLocation].interact(action);
                            typewriterEffect(result);
                            player->addDiscoveredInteraction(action);
                            
                            // Special interaction effects
                            if (action == "touch monolith") {
                                player->setQuestFlag("touched_monolith");
                                player->gainExperience(20);
                            } else if (action == "examine tools" || action == "look under desk") {
                                player->gainExperience(10);
                            } else if (action == "inspect emergency gear") {
                                if (!player->hasQuestFlag("found_spacesuit")) {
                                    player->setQuestFlag("found_spacesuit");
                                    player->gainExperience(15);
                                }
                            }
                        }
                        break;
                    }
                    case 5: {
                        pickupItem();
                        break;
                    }
                    case 6: {
                        const auto& inventory = player->getInventory();
                        if (inventory.empty()) {
                            std::cout << "You don't have any items to use." << std::endl;
                            break;
                        }
                        
                        std::cout << "\nYour items:" << std::endl;
                        for (size_t i = 0; i < inventory.size(); ++i) {
                            std::cout << i + 1 << ". " << inventory[i]->getName() << std::endl;
                            std::cout << "   " << inventory[i]->getUseDescription() << std::endl;
                        }
                        
                        std::cout << "Choose item to use (1-" << inventory.size() << ") or 0 to cancel: ";
                        int itemChoice;
                        std::cin >> itemChoice;
                        std::cin.ignore();
                        
                        if (itemChoice >= 1 && itemChoice <= static_cast<int>(inventory.size())) {
                            auto item = inventory[itemChoice - 1];
                            if (item->canUse()) {
                                item->use();
                                player->gainExperience(10);
                            } else {
                                std::cout << "This item cannot be used." << std::endl;
                            }
                        }
                        break;
                    }
                    case 7: {
                        std::cout << "\nAre you sure you want to quit? (y/n): ";
                        char confirm;
                        std::cin >> confirm;
                        if (confirm == 'y' || confirm == 'Y') {
                            gameOver = true;
                            displayEndGameStats();
                        }
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid choice!");
                }

                // Check for chapter completion
                if (player->hasQuestFlag("read_classified_info") && 
                    player->hasQuestFlag("touched_monolith") && 
                    player->hasQuestFlag("spacesuit_equipped") && 
                    chapter == 1) {
                    
                    chapter++;
                    std::cout << AnsiArt::YELLOW;
                    typewriterEffect("\nChapter " + std::to_string(chapter) + ": The Escape", 50);
                    typewriterEffect("You have gathered the necessary items and knowledge.");
                    typewriterEffect("Now you must find a way to escape Europa Station...");
                    std::cout << AnsiArt::RESET;
                    player->gainExperience(50);
                }

                // Clear the screen between turns
                if (!gameOver) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    std::cout << AnsiArt::CLEAR_SCREEN;
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << AnsiArt::RED << "Error: " << e.what() << AnsiArt::RESET << std::endl;
        }
    }
};

int main() {
    try {
        Game game;
        game.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
