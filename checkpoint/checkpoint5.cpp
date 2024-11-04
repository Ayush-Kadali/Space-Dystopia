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
#include <queue>

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

template<typename T>
class QuestObjective {
private:
    std::string description;
    T target;
    T current;
    bool completed;

public:
    QuestObjective(const std::string& desc, T targetValue)
        : description(desc), target(targetValue), current(T()), completed(false) {}

    void updateProgress(T value) {
        current = value;
        completed = (current >= target);
    }

    bool isCompleted() const { return completed; }
    std::string getDescription() const { return description; }
    T getProgress() const { return current; }
    T getTarget() const { return target; }
};

class Quest {
private:
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<QuestObjective<int>>> objectives;
    bool completed;

public:
    Quest(const std::string& n, const std::string& desc)
        : name(n), description(desc), completed(false) {}

    void addObjective(const std::string& desc, int target) {
        objectives.push_back(std::make_shared<QuestObjective<int>>(desc, target));
    }

    void updateObjective(size_t index, int value) {
        if (index < objectives.size()) {
            objectives[index]->updateProgress(value);
            checkCompletion();
        }
    }

    bool isCompleted() const { return completed; }
    std::string getName() const { return name; }

private:
    void checkCompletion() {
        completed = std::all_of(objectives.begin(), objectives.end(),
            [](const auto& obj) { return obj->isCompleted(); });
    }
};

// New Combat System demonstrating inheritance and polymorphism
class CombatEntity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    CombatEntity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}
    
    virtual ~CombatEntity() = default;
    virtual int calculateDamage() const = 0;
    virtual void takeDamage(int damage) {
        health = std::max(0, health - std::max(0, damage - defense));
    }

    bool isAlive() const { return health > 0; }
    std::string getName() const { return name; }
    int getHealth() const { return health; }
};

// Enhanced Player class with combat capabilities
class CombatPlayer : public CombatEntity {
private:
    int level;
    int experience;
    std::vector<std::string> abilities;

public:
    CombatPlayer(const std::string& n)
        : CombatEntity(n, 100, 15, 5), level(1), experience(0) {
        abilities = {"Quick Attack", "Defensive Stance"};
    }

    int calculateDamage() const override {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-2, 2);
        return attack + dis(gen);
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= level * 100) {
            levelUp();
        }
    }

private:
    void levelUp() {
        level++;
        health += 10;
        attack += 5;
        defense += 3;
        experience = 0;
        std::cout << "\nLevel Up! Now level " << level << std::endl;
        std::cout << "Health +" << 10 << std::endl;
        std::cout << "Attack +" << 5 << std::endl;
        std::cout << "Defense +" << 3 << std::endl;
    }
};

// New Enemy class demonstrating inheritance
class Enemy : public CombatEntity {
private:
    std::string type;
    std::vector<std::string> dropItems;

public:
    Enemy(const std::string& n, const std::string& t, int h, int a, int d)
        : CombatEntity(n, h, a, d), type(t) {}

    int calculateDamage() const override {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-1, 1);
        return attack + dis(gen);
    }

    void addDropItem(const std::string& item) {
        dropItems.push_back(item);
    }

    std::vector<std::string> getDrops() const {
        return dropItems;
    }
};

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

    }

    void incrementSteps() { totalSteps++; }
    void incrementItemsCollected() { itemsCollected++; }

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
    std::vector<Location> locations;
    bool gameOver;
    int currentLocation;
    std::vector<Quest> quests;
    std::queue<std::string> messageLog;
    std::vector<std::shared_ptr<CombatEntity>> enemies;
    bool hasEscaped;

    void displayTitle() {
        std::cout << AnsiArt::CLEAR_SCREEN;
        std::cout << AnsiArt::BLUE;
        AnsiArt::printCentered("================================");
        AnsiArt::printCentered("SPACE DYSTOPIA: THE LAST FRONTIER");
        AnsiArt::printCentered("================================");
        AnsiArt::AsciiArt::drawSpacestation();
        std::cout << AnsiArt::RESET << std::endl;
    }

    void initializeQuests() {
        Quest mainQuest("Escape Europa", "Find a way to escape and reveal the truth");
        mainQuest.addObjective("Access classified data", 1);
        mainQuest.addObjective("Bypass security", 1);
        mainQuest.addObjective("Escape via airlock", 1);
        quests.push_back(mainQuest);
    }

    void initializeEnemies() {
        auto securityBot = std::make_shared<Enemy>("Security Bot", "Robot", 50, 10, 3);
        auto eliteGuard = std::make_shared<Enemy>("Elite Guard Bot", "Robot", 75, 15, 5);
        enemies.push_back(securityBot);
        enemies.push_back(eliteGuard);
    }

    void initializeLocations() {
        locations = {
            Location("Maintenance Bay", "A sterile white room filled with repair equipment."),
            Location("Terminal Room", "A quiet room with a terminal. Red light pulses steadily."),
            Location("Security Post", "A heavily guarded area with advanced security bots."),
            Location("Airlock", "The gateway between the station and the void of space.")
        };


        auto datapad = std::make_shared<Item>("Datapad", "A tablet containing classified information", true);
        auto keycard = std::make_shared<Item>("Keycard", "A security keycard", true);
        auto spacesuit = std::make_shared<Item>("Spacesuit", "Required for space travel", true);
        auto emp = std::make_shared<Item>("EMP Device", "Can disable security systems", true);


        
        // Create items with detailed use effects
        datapad->setUseEffect([this]() {
            std::cout << "You carefully read through the classified information..." << std::endl;
            std::cout << "The data reveals coordinates for a potentially habitable planet beyond Pluto." << std::endl;
            player->setQuestFlag("read_classified_info");
            player->gainExperience(20);
        }, "Access classified information about the mysterious signals");


        keycard->setUseEffect([this]() {
            if (currentLocation == 1) { // Terminal Room
                std::cout << "You swipe the keycard through the terminal..." << std::endl;
                player->setQuestFlag("terminal_access_granted");
                player->gainExperience(15);
            } else {
                std::cout << "There's nowhere to use the keycard here." << std::endl;
            }
        }, "Use at terminals to gain access");

        spacesuit->setUseEffect([this]() {
            if (currentLocation == 3) { // Airlock
                std::cout << "You put on the spacesuit, checking all seals..." << std::endl;
                player->setQuestFlag("spacesuit_equipped");
                player->gainExperience(10);
            } else {
                std::cout << "You should wait until you're at the airlock." << std::endl;
            }
        }, "Required for EVA activities");


        // Add items to locations
        locations[0].addItem(datapad);
        locations[1].addItem(keycard);
        locations[3].addItem(spacesuit);
        locations[2].addItem(emp);

        locations[0].addInteraction("examine workbench",
            "You find various repair tools and a hidden datapad.");
        
        // Single hack terminal interaction
        locations[1].addInteraction("hack terminal", 
            "You begin hacking the terminal... Security has been alerted!");
        locations[1].addInteraction("examine terminal",
            "The terminal displays various system diagnostics.");
        
        locations[2].addInteraction("examine security",
            "The security systems are active but might be vulnerable to EMPs.");
        
        locations[3].addInteraction("check airlock",
            "The airlock appears functional. A spacesuit would be required for EVA.");
        locations[3].addInteraction("activate airlock",
            "The airlock cycles... This is your chance to escape!");
    }


        /*
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
        locations[1].addInteraction("hack terminal", 
            "As you attempt to hack the terminal, a security bot detects your presence!");
        locations[1].addInteraction("hack terminal", 
            "You begin hacking the terminal... Security has been alerted!");
        
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
        locations[3].addInteraction("activate airlock",
            "The airlock cycles... This is your chance to escape!");
    }
    */

    bool checkWinCondition() {
        return player->hasQuestFlag("read_classified_info") && 
               player->hasQuestFlag("security_defeated") && 
               player->hasQuestFlag("spacesuit_equipped");
    }

    void handleCombat(std::shared_ptr<CombatEntity> enemy) {
        std::cout << "\nCombat with " << enemy->getName() << " initiated!" << std::endl;
        
        auto playerCombat = std::make_shared<CombatPlayer>(player->getName());
        
        while (enemy->isAlive() && playerCombat->isAlive()) {
            // Player turn
            std::cout << "\n1. Attack\n2. Use EMP (if available)\n";
            int choice;
            std::cin >> choice;
            std::cin.ignore();

            int playerDamage = 0;
            if (choice == 2 && player->getInventory().size() > 0) {
                // EMP does extra damage to robots
                playerDamage = playerCombat->calculateDamage() * 2;
                std::cout << "EMP deployed successfully!" << std::endl;
            } else {
                playerDamage = playerCombat->calculateDamage();
            }

            enemy->takeDamage(playerDamage);
            typewriterEffect("You deal " + std::to_string(playerDamage) + " damage!");
            

        if (!enemy->isAlive()) {
            typewriterEffect("You defeated " + enemy->getName() + "!");
            player->setQuestFlag("security_defeated");
            player->gainExperience(50);
        }

            // Enemy turn
            int enemyDamage = enemy->calculateDamage();
            playerCombat->takeDamage(enemyDamage);
            typewriterEffect(enemy->getName() + " deals " + std::to_string(enemyDamage) + " damage!");
            
            std::cout << "\nYour Health: " << playerCombat->getHealth() << std::endl;
            std::cout << enemy->getName() << "'s Health: " << enemy->getHealth() << std::endl;
        }
    }

public:
    // Default constructor with initialization list demonstrating exception handling
    Game() : gameOver(false), currentLocation(0), hasEscaped(false) {
        try {
            displayTitle();
            std::cout << "\nEnter your name: ";
            std::string playerName;
            std::getline(std::cin, playerName);
            
            if (playerName.empty()) {
                throw std::invalid_argument("Name cannot be empty!");
            }
            
            player = std::make_unique<Player>(playerName);
            initializeLocations();
            initializeQuests();
            initializeEnemies();
            
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
        
        std::cout << "Locations explored: " << currentLocation + 1 << "/" << locations.size() << std::endl;
        
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
            typewriterEffect("\nWelcome to Space Station Europa. Your mission: Escape and reveal the truth.");
            
            while (!gameOver && !hasEscaped) {
                displayLocation();
                
                std::cout << "\nOptions:\n";
                std::cout << "1. Move to another location\n";
                std::cout << "2. Interact with environment\n";
                std::cout << "3. Pick up item\n";
                std::cout << "4. Check inventory\n";
                std::cout << "5. Check status\n";
                std::cout << "6. Quit\n";
                
                int choice;
                std::cout << "\nEnter your choice (1-8): ";
                std::cin >> choice;
                std::cin.ignore();
                
                switch (choice) {
                    case 1: {
                        std::cout << "\nAvailable locations:\n";
                        for (size_t i = 0; i < locations.size(); ++i) {
                            std::cout << i + 1 << ". " << locations[i].getName() << std::endl;
                        }
                        std::cout << "Choose location (1-" << locations.size() << "): ";
                        int loc;
                        std::cin >> loc;
                        if (loc >= 1 && loc <= static_cast<int>(locations.size())) {
                            currentLocation = loc - 1;
                            player->incrementSteps();
                        }
                        break;
                    }
                    case 2: {
                        const auto& availableInteractions = locations[currentLocation].getAvailableInteractions();
                        std::cout << "\nAvailable interactions:" << std::endl;
                        for (size_t i = 0; i < availableInteractions.size(); ++i) {
                            std::cout << i + 1 << ". " << availableInteractions[i] << std::endl;
                        }
                        
                        if (!availableInteractions.empty()) {
                            std::cout << "Choose interaction: ";
                            int interactionChoice;
                            std::cin >> interactionChoice;
                            std::cin.ignore();
                            
                            if (interactionChoice >= 1 && interactionChoice <= static_cast<int>(availableInteractions.size())) {
                                std::string action = availableInteractions[interactionChoice - 1];
                                std::string result = locations[currentLocation].interact(action);
                                typewriterEffect(result);
                                
                                if (action == "hack terminal") {
                                    player->setQuestFlag("terminal_hacked");
                                    handleCombat(enemies[0]);
                                } else if (action == "activate airlock" && 
                                         player->hasQuestFlag("terminal_hacked") && 
                                         player->hasQuestFlag("security_defeated")) {
                                    player->setQuestFlag("airlock_escaped");
                                    hasEscaped = true;
                                    typewriterEffect("Congratulations! You've escaped and can now reveal the truth!");
                                    gameOver = true;
                                }
                            }
                        } else {
                            std::cout << "No interactions available here." << std::endl;
                        }
                        break;
                    }
                    case 3:
                        pickupItem();
                        break;
                    case 4:
                        player->display();
                        break;
                    case 5: {
                        std::cout << "\nStatus Report:" << std::endl;
                        std::cout << "Terminal Hacked: " << (player->hasQuestFlag("terminal_hacked") ? "Yes" : "No") << std::endl;
                        std::cout << "Security Defeated: " << (player->hasQuestFlag("security_defeated") ? "Yes" : "No") << std::endl;
                        std::cout << "Escaped: " << (player->hasQuestFlag("airlock_escaped") ? "Yes" : "No") << std::endl;
                        break;
                    }
                    case 6:
                        gameOver = true;
                        break;
                    default:
                        std::cout << "Invalid choice." << std::endl;
                }


                // Special interaction handling
               if (currentLocation == 1 && // Terminal Room
                    player->hasQuestFlag("terminal_access_granted") &&
                    !player->hasQuestFlag("security_defeated")) {
                    std::cout << "\nA Security Bot has detected your presence!" << std::endl;
                    handleCombat(enemies[0]); // Fight the security bot
                }

                if (currentLocation == 3 && // Airlock
                    player->hasQuestFlag("security_defeated") &&
                    player->hasQuestFlag("spacesuit_equipped")) {
                    hasEscaped = true;
                    typewriterEffect("Congratulations! You've successfully escaped!");
                    gameOver = true;
                }

                // Quest progress checking
                if (player->hasQuestFlag("read_classified_info") &&
                    player->hasQuestFlag("touched_monolith") &&
                    player->hasQuestFlag("spacesuit_equipped")) {
                    quests[0].updateObjective(0, 1); // Update main quest progress
                }

                if (!gameOver) {
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    std::cout << AnsiArt::CLEAR_SCREEN;
                }

                if (hasEscaped) {
                    std::cout << AnsiArt::GREEN << "\nVICTORY!" << AnsiArt::RESET << std::endl;
                    displayEndGameStats();
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
