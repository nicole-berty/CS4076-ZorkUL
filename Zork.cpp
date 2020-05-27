/*************************
 * @author: Nicole Berty
 * All the header files are listed first, then all the cpp files. Each file's name is in a single comment, eg. //game.cpp
 * Each concept is marked like this in the code: //1. Destructors
 * Here is a list of each concept and where they are implemented:
 * 1. Destructors:
 *          - enemy.h/enemy.cpp (declared and defined)
 *          - player.h/player.cpp (declared and defined)
 *          - character.h/character.cpp (declared and defined)
 * 2. Inheritance (including virtual methods) and cascading constructors
 *          - character.h/character.cpp (base class)
 *          - player.h/player.cpp (derived class of Character)
 *          - enemy.h/enemy.cpp (derived class of Character)
 *          - item.h/item.cpp (base class)
 *          - weapon.h/weapon.cpp (derived class of Item)
 * 3. Templates
 *          - item.h/item.cpp (method setWeight(const T& weight) declared and defined)
 * 4. Unary Operator Overloading
 *          - player.h/player.cpp (declared and defined)
 *          - game.cpp (example usage)
 * 5. Binary Operator Overloading
 *          - enemy.h/enemy.cpp (declared and defined)
 *          - item.h/item.cpp (declared and defined)
 *          - room.cpp (examples of enemy and item binary operator usage)
 * 6. Friends
 *          - character.h (friend class Game)
 *          - enemy.h (friend function operator==)
 *          - item.h (friend function operator==)
 *          - room.h (friend classes Game, enterRoomListener)
 * 7. Virtual functions and polymorphism
 *          - item.h (base class, virtual function getLongDescription())
 *          - weapon.h/weapon.cpp (derived class, different implementation of virtual function getLongDescription())
 *          - player.cpp (example of runtime polymorphism occurring in getItems() function)
 *              (declaration Item *sword = new Weapon() is in game.h/.cpp)
 * 8. Abstract classes and pure virtual functions
 *          - character.h/character.cpp (is abstract base class, has 2 pure virtual functions setHealth and setStamina)
 *          - enemy.h/enemy.cpp (derived class, has implementation of pure virtual functions setHealth and setStamina)
 *          - player.h/player.cpp (derived class, has implementation of pure virtual functions setHealth and setStamina)
 * 9. Initialiser List
 *          - character.cpp
 *          - enemy.cpp
 *          - item.cpp
 *          - player.cpp
 *          - weapon.cpp
 * 10. Dynamic and static dispatch
 *          - item.h/item.cpp (base class, virtual function getLongDescription() declared and defined)
 *          - weapon.h/weapon.cpp (derived class, different implementation of virtual function getLongDescription())
 *          - player.cpp (example of dynamic dispatch occurring in getItems() function)
 *  
 * ***********************/

#include<string>
#include <map>
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include<cstdlib> 
#include<ctime>
using namespace std;

// EventListener.h
/** Parent class of all listeners. (It is abstract and can't be instantiated) */
class EventListener
{
public:
    // "void *" is a generic pointer to anything!
    // 10x worse than regular pointers in potential problems
    virtual void run(void *args) = 0;
};

//attacklistener.h
class Game;

class AttackListener : public EventListener
{
public:
    AttackListener(Game *game);
    void run(void *args) override;
private:
    Game  *game;
    string enemyName;
};

//character.h
// No need to include room since we're only working with pointers.
// This is how we tell the compiler "there will be a room eventually".
// This is called a "forward declaration".
class Room;

//2. Inheritance (including virtual methods) and cascading constructors (Base Class)
class Character
{
public:

    string getName();
    int    getHealth();

    //6. Friends
    friend class Game;

protected:
    Character(string name);
    ~Character();
    string name;
    int health;
    int stamina;
    Room *currentRoom;

    int    getStamina();
    Room  *getCurrentRoom();

    void setName(string name);
    void setCurrentRoom(Room *next); 

    //8. Abstract classes and pure virtual functions
    virtual void setHealth(int setHealth) = 0;
    virtual void setStamina(int stamina) = 0;
};

//characterdeathlistener.h

class Game;
// A listener for the bad ending
class CharacterDeathListener : public EventListener
{
public:
    CharacterDeathListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//curselistener.h
class Game;

// A listener for the cursed item being picked up
class CurseListener : public EventListener
{
public:
    CurseListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//defeatlistener.h
class Game;

// A listener for the bad ending
class DefeatListener : public EventListener
{
public:
    DefeatListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//enemy.h

//2. Inheritance (including virtual methods) and cascading constructors
class Enemy : public Character
{
public:
    Enemy(string name, string description);

    //1. Destructors
    ~Enemy();

    //8. Abstract classes and pure virtual functions (derived class)
    virtual void setHealth(int setHealth);
    virtual void setStamina(int stamina);

    void setDescription(string description);
    string getDescription();

private:
    string description;
    
    //5. Binary Operator Overloading
    //6. Friends
    friend bool operator== (const Enemy &e1, const Enemy &e2);
};

//enterroomlistener.h
class Game;

// A listener for the good ending
class EnterRoomListener : public EventListener
{
public:
    EnterRoomListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

// EventManager.h (Implemented as a Singleton)
/** This class manages the event loop and all the event calls */
struct EventManager // Structs in C++ are the same as classes, but default to "public" instead of "private".
{
    // static inside a class or struct works exactly like in java
    // Only one instance per class.
    static EventManager &getInstance(); // Gets an instance of the class (an object)

    // Making sure I can't have copies of this instance.
    EventManager(EventManager const& copy);            // Not Implemented (Copy constructor)
    EventManager& operator=(EventManager const& copy); // Not Implemented (Assignment operator)

    // Registers an event.
    void listen(string event_name, EventListener *listener);

    // Emits an event
    // "void *" is a generic pointer to anything.
    // !!! Highly unsafe, please don't do this at home without the presence of your parents !!!
    void trigger(string event_name, void *args = nullptr);

    // Returns true if still running
    bool is_running();

    // Stops the event loop
    void stop();

    // Reads the input and calls an input event
    void check_events();

    // constantly checks for new inputs until the program ends.
    void event_loop();

private:
    // True if running
    bool running;

    // map<K, V> is a collection of V's indexed by S.
    // In this case, we have lists/vectors of events indexed by their names.
    map<string, vector<EventListener*>> registeredEvents;

    // Only EventManager can call the constructor, so the only way to get an instance
    // is by calling getInstance.
    EventManager();
};

//exitlistener.h
class Game;

// A listener for the exit command
class ExitListener : public EventListener
{
public:
    ExitListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//item.h
class Item {
protected:
	string description;
	float weightGrams;
	int multiplier;

public:
    Item (string description, float inWeight, int multiplier = 0);
	float getWeight();
	int getMultiplier();
	string getShortDescription();

	//7. Virtual functions and polymorphism
	//10. Dynamic and static dispatch (derived class is weapon.h)
	virtual string getLongDescription();

	//3. Templates
	template <typename T>
	void setWeight(const T& weightGrams);

	//5. Binary Operator Overloading
	//6. Friends
	friend bool operator== (const Item &item1, const Item &item2);
};

//player.h

//2. Inheritance (including virtual methods) and cascading constructors
class Player : public Character
{
public:
    Player(string newName);

    //1. Destructors
    ~Player();

    vector < Item* > itemInventory;

    //8. Abstract classes and pure virtual functions (derived class)
    virtual void setHealth(int setHealth);
    virtual void setStamina(int stamina);

    void addItem(Item*);
    void emptyInventory();
    void removeItem(Item*);
    string getItems();
    bool hasItem(string); 

    //4. Unary Operator Overloading
    void operator--();
    void operator++();
};

//game.h
class Game
{
public:
    Game();
    void reset(bool show_update = true);

    void map();
    void info();
    void go(string direction);
    void teleport();
    void take(string itemName);
    void use(string itemName);
    void inventory();
    void attack(string name);
    void enemyMove();

    void update_screen();

    Player &getPlayer();
    void setOver(bool over);
    bool is_over();
    void setCurrentRoom(Room *next);

private:
    Player      player;
    vector<Room *> rooms;
    bool           gameOver;
    Enemy          mojo;
    Item           *key;
    Item           *sword;
    
    bool        enemyCheck();
};

//golistener.h
class Game;

// A listener for the Go command
class GoListener : public EventListener
{
public:
    GoListener(Game *game);
    void run(void *args) override;
private:
    Game  *game;
    string direction;
};

//infolistener.h
class Game;

class InfoListener : public EventListener
{
public:
    InfoListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//inputlistener.h
// No need to include if we're just using pointers.
class Game;

// A listener that gets called when a new input is received.
struct InputListener : EventListener // Structs default to public even in inheritance.
{
    InputListener(Game *game);
    void run(void *args) override;

private:
    // We store a game pointer for easy access.
    Game *game;
    vector<string> args;
};

//inventorylistener.h
class Game;

// A listener for the Inventory command
class InventoryListener : public EventListener
{
public:
    InventoryListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//maplistener.h
class Game;
// A listener for the map command
class MapListener : public EventListener
{
public:
    MapListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//restartlistener.h
class Game;

// A listener for the good ending
class RestartListener : public EventListener
{
public:
    RestartListener(Game *game);
    void run(void *args) override;
private:
   Game *game;
};

//room.h
class Room
{
private:
    string name;
    map<string, Room *> exits;

    Room(string name);
    vector<Item*> itemsInRoom;
    vector<Enemy*> enemiesInRoom;

    int numberOfItems();
    vector<Item*> getItems();
    void addItem(Item *inItem);
    void removeItem(Item*);
    string displayItems();
    string getName();
    void setExits(Room *north, Room *east, Room *south, Room *west);
    Room *getExit(string direction);
    bool hasItem(Item*);
    bool hasEnemy(Enemy*);

    vector<Enemy*>getEnemies();
    void addEnemy(Enemy *enemy);
    string displayEnemies();
    void removeEnemy(Enemy*);

    public:
    
    //6. Friends
    friend class Game;
    friend class EnterRoomListener;
};

//takelistener.h
class Game;
// A listener for the Take command
class TakeListener : public EventListener
{
public:
    TakeListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
    string itemName;
};

//teleportlistener.h
class Game;
// A listener for the Teleport command
class TeleportListener : public EventListener
{
public:
    TeleportListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
};

//uselistener.h
class Game;

// A listener for the Use command
class UseListener : public EventListener
{
public:
    UseListener(Game *game);
    void run(void *args) override;
private:
    Game *game;
    string itemName;
};

//victorylistener.h
class Game;
// A listener for the good ending
class VictoryListener : public EventListener
{
public:
    VictoryListener(Game *game);
    void run(void *args) override;
private:
   Game *game;
};

//weapon.h
//2. Inheritance (including virtual methods) and cascading constructors
class Weapon : public Item {

public:

    Weapon (string description, float inWeight, int multiplier);

//7. Virtual functions and polymorphism
//10. Dynamic and static dispatch
    virtual string getLongDescription();

};

//attacklistener.cpp
AttackListener::AttackListener(Game *game)
{
    this->game = game;
    enemyName = "";
}

void AttackListener::run(void *args_ptr)
{
    if (game->is_over()) {
        return;
    }

    vector<string> *args = (vector<string> *) args_ptr;

    if (args->size() > 1) {
        game->attack(args->at(1));
    } else {
        cout << "Need to specify an enemy to attack!" << endl;
    }
}

//character.cpp

//9. Initializer list
Character::Character(string name) : name(name)
{ 
    health      = 100;
    stamina     = 100;
    currentRoom = nullptr;
    cout << "Character created (base class)" << endl;
}

//1. Destructors 
Character::~Character() {
  cout << "Character destructor: Goodbye cruel world!" << endl;
}

string Character::getName()
{
    return name;
}

int Character::getHealth()
{
    return health;
}

int Character::getStamina()
{
    return stamina;
}

Room *Character::getCurrentRoom()
{
    return currentRoom;
}

void Character::setName(string name)
{
    this->name = name;
}

void Character::setCurrentRoom(Room *next)
{
    currentRoom = next;
}

//characterdeathlistener.cpp

CharacterDeathListener::CharacterDeathListener(Game *game)
{
    this->game = game;
}

void CharacterDeathListener::run(void *args)
{
    if (game->is_over()) {
        return;
    }

    Player *character = (Player *) args;

    if (character->getName() == game->getPlayer().getName()) {
        EventManager::getInstance().trigger("defeat");
    }
}

//curselistener.cpp
CurseListener::CurseListener(Game *game)
{
    this->game = game;
}

void CurseListener::run(void *args)
{
    Player *player1 = (Player *) args;
    player1->setHealth(player1->getHealth() - 20);
    cout << "You've lost some health points due to the cursed item." << endl;
}

//defeatlistener.cpp
DefeatListener::DefeatListener(Game *game)
{
    this->game = game;
}

void DefeatListener::run(void *)
{
    cout << endl;
    cout << "Defeat!" << endl;
    game->setOver(true);
}

//enemy.cpp

//2. Inheritance (including virtual methods) and cascading constructors
//9. Initializer list
Enemy::Enemy(string name, string description) : Character(name), description(description)
{
    cout << "Enemy created (child class)" << endl;
}

//1. Destructors
Enemy::~Enemy() {
    cout << "Enemy destructor: Bye bye" << endl;
}

string Enemy::getDescription()
{
    return description;
}

//8. Abstract classes and pure virtual functions
void Enemy::setDescription(string description)
{
    this->description = description;
}

//8. Abstract classes and pure virtual functions
void Enemy::setHealth(int health)
{
    if (health <= 0) {
        health = 0;
    }

    this->health = health;
}

void Enemy::setStamina(int stamina)
{
    if (stamina <= 0) {
        stamina = 0;
    }

    this->stamina = stamina;
}

//5. Binary Operator Overloading
bool operator==(const Enemy &e1, const Enemy &e2) {
    return e1.name == e2.name;
}

//enterroomlistener.cpp

EnterRoomListener::EnterRoomListener(Game *game)
{
    this->game = game;
}

void EnterRoomListener::run(void *args)
{
    if (game->is_over()) {
        return;
    }

    Room *room = (Room *) args;

    if (room->getName() == "J") {
        EventManager::getInstance().trigger("victory");
    }
}

//eventmanager.cpp
/** Converts strings from UPPERCASE or MiXedCasE to lowercase. */
// This is a regular old C-style function not a method.
string tolower(string &s)
{
    string ns;
    for (auto c : s) {
        ns += tolower(c);
    }
    return ns;
}

EventManager::EventManager()
{
    running = true;
}

EventManager &EventManager::getInstance()
{
    // static inside functions creates an instance of this variable for all the calls of this function, and initializes only once.
    static EventManager instance;
    return instance;
}

void EventManager::listen(string event_name, EventListener *listener)
{
    registeredEvents[tolower(event_name)].push_back(listener);
}

void EventManager::trigger(string event_name, void *args)
{
    for (auto listener : registeredEvents[tolower(event_name)]) {
        listener->run(args);
    }
}

bool EventManager::is_running()
{
    return running;
}

void EventManager::stop()
{
    running = false;
}

void EventManager::check_events()
{
    string buffer;
    vector<string> words;

    cout << "> ";               // print prompt
    getline(cin, buffer, '\n');	// read a line from cin to "buffer"
    buffer = tolower(buffer);

    string::size_type pos = 0, last_pos = 0;

    // Break "buffer" up by spaces
    bool finished = false;
    while (!finished) {
        pos = buffer.find_first_of(' ', last_pos);	// find and remember first space.
        if (pos == string::npos ) {			// if we found the last word,
            words.push_back(buffer.substr(last_pos));	// add it to vector "words"
            finished = true;				// and finish searching.
        } else {					// otherwise add to vector and move on to next word.
            words.push_back(buffer.substr(last_pos, pos - last_pos));
            last_pos = pos + 1;
        }
    }

    trigger("input", &words);
}

void EventManager::event_loop()
{
    EventManager &eventManager = EventManager::getInstance();

    while (eventManager.is_running()) {
        eventManager.check_events();
    }
}

//exitlistener.cpp

ExitListener::ExitListener(Game *game)
{
    this->game = game;
}

void ExitListener::run(void *)
{
    EventManager::getInstance().stop();
}

//player.cpp

//2. Inheritance (including virtual methods) and cascading constructors
//9. Initializer list
Player::Player(string newName): Character(newName) {
    cout << "Player created (child class)" << endl;
}


//1. Destructors
Player::~Player() {
    cout << "Player destructor: The end is nigh" << endl;
}

//8. Abstract classes and pure virtual functions
void Player::setHealth(int health)
{
    if (health <= 0) {
        health = 0;
        EventManager::getInstance().trigger("characterDeath", this);
    }

    this->health = health;
}

//8. Abstract classes and pure virtual functions
void Player::setStamina(int stamina)
{
    if (stamina <= 0) {
        stamina = 0;
        EventManager::getInstance().trigger("characterDeath", this);
    }

    this->stamina = stamina;
}

string Player::getItems() { 
    string ret = "\nInventory:\n";
    for (vector<Item*>::iterator it = itemInventory.begin(); it != itemInventory.end(); it++)
    //7. Virtual Functions and Polymorphism (example of runtime polymorphism occurring)
    //10. Dynamic and static dispatch
    //(Item has virtual func getLongDescription, weapon inherits from Item and overrides this method)
        ret += "\t"+ (*it)->getLongDescription() + "\n";
    return ret;
}

void Player::addItem(Item *newItem) {
    if (! (std::find(itemInventory.begin(), itemInventory.end(), newItem) != itemInventory.end() )) {
        itemInventory.push_back(newItem);
        std::cout << "You have picked up a new item! It has been added to your inventory.\n"; 
    }
}

bool Player::hasItem(string item) {
    for (unsigned int i = 0; i < itemInventory.size(); i++) {
        if (itemInventory.at(i)->getShortDescription().compare(item) == 0) {
                return true;
        }
    }
    return false;
}

void Player::emptyInventory() { this->itemInventory.clear(); }

void Player::removeItem(Item *newItem) {
        for (unsigned int i = 0; i < itemInventory.size(); i++) {
            //5. Binary Operator Overloading (Item class)
            if(itemInventory.at(i) == newItem) {
                itemInventory.erase(itemInventory.begin() + i);
            }
        }
}

//4. Unary Operator Overloading
void Player::operator--() {
    health = health - 20;
}

//4. Unary Operator Overloading
void Player::operator++() {
    health = health + 20;
}

//game.cpp
Game::Game() :
    player("Hero"),
    mojo("mojo", "is a moving enemy")
{
    srand(time(nullptr));
    
    // Commands
    EventManager::getInstance().listen("go",        new GoListener(this));
    EventManager::getInstance().listen("map",       new MapListener(this));
    EventManager::getInstance().listen("info",      new InfoListener(this));
    EventManager::getInstance().listen("restart",   new RestartListener(this));
    EventManager::getInstance().listen("teleport",  new TeleportListener(this));
    EventManager::getInstance().listen("exit",      new ExitListener(this));
    EventManager::getInstance().listen("take",      new TakeListener(this));
    EventManager::getInstance().listen("use",       new UseListener(this));
    EventManager::getInstance().listen("inventory", new InventoryListener(this));
    EventManager::getInstance().listen("attack",    new AttackListener(this));

    // State changes
    EventManager::getInstance().listen("characterDeath", new CharacterDeathListener(this));
    EventManager::getInstance().listen("enterRoom",      new EnterRoomListener(this));
    EventManager::getInstance().listen("victory",        new VictoryListener(this));
    EventManager::getInstance().listen("defeat",         new DefeatListener(this));
    EventManager::getInstance().listen("curse",          new CurseListener(this));

    rooms.push_back(new Room("A")); // 0
    rooms.push_back(new Room("B")); // 1
    rooms.push_back(new Room("C")); // 2
    rooms.push_back(new Room("D")); // 3
    rooms.push_back(new Room("E")); // 4
    rooms.push_back(new Room("F")); // 5
    rooms.push_back(new Room("G")); // 6
    rooms.push_back(new Room("H")); // 7
    rooms.push_back(new Room("I")); // 8
    rooms.push_back(new Room("J")); // 9

    //                 N         E         S         W
    rooms[0]->setExits(rooms[4], rooms[2], rooms[7], rooms[1]);
    rooms[1]->setExits(nullptr,  rooms[0], nullptr,  nullptr);
    rooms[2]->setExits(nullptr,  nullptr,  nullptr,  rooms[0]);
    rooms[3]->setExits(nullptr,  rooms[4], nullptr,  nullptr);
    rooms[4]->setExits(nullptr,  rooms[5], rooms[0], rooms[3]);
    rooms[5]->setExits(nullptr,  nullptr,  nullptr,  rooms[4]);
    rooms[6]->setExits(nullptr,  rooms[7], nullptr,  nullptr);
    rooms[7]->setExits(rooms[0], rooms[8], rooms[9], rooms[6]);
    rooms[8]->setExits(nullptr,  nullptr,  nullptr,  rooms[7]);
    rooms[9]->setExits(rooms[7], nullptr,  nullptr,  nullptr);

    reset();
}

void Game::reset(bool show_update)
{
    gameOver = false;

    for (unsigned int i = 0; i < rooms.size(); i++) {
          rooms.at(i)->itemsInRoom.clear();  
          rooms.at(i)->enemiesInRoom.clear();
    }
        
    key = new Item("key", 22);
    key->setWeight(27);
    rooms[2]->addItem(key);
    rooms[6]->addItem(new Item("cursed_item", 15.56));
    rooms[3]->addItem(new Item("potion", 10.25));

    sword = new Weapon("sword", 150, 5);
    rooms[5]->addItem(sword);

    rooms[2]->addEnemy(new Enemy("stationary-man", "is a non-moving enemy"));
    
    mojo.setCurrentRoom(rooms[3]);
    mojo.setHealth(100);
    mojo.setStamina(100);

    player.setCurrentRoom(rooms[0]);
    player.setHealth(100);
    player.setStamina(100);

    player.emptyInventory();
    
    cout << "Welcome to Zork!" << endl;
    cout << "To pick up items, type take x where x is the name of the item exactly as it is written in the room description, eg. take cursed_item\n";
    cout << "To attack enemies, type attack x where x is the enemy name exactly as it is\nwritten in the room description" << endl;
    if (show_update) {
        update_screen();
    }
}

void Game::setOver(bool over)
{
    this->gameOver = over;
}

void Game::map()
{
    cout << "Subtitle: [player] !enemy! [player & Enemy!" << endl;

    vector<string> roomNames= {" A ", " B ", " C ", " D ", " E ", " F ", " G ", " H ", " I ", " J "};

    for (int i = 0; i < roomNames.size(); i++)
    {
        if (player.getCurrentRoom() == rooms.at(i))
        {
            roomNames.at(i) = "[" + rooms.at(i)->getName() + "]";
        }
        if (mojo.getCurrentRoom() == rooms.at(i) && enemyCheck()) {
            roomNames.at(i) = "!" + rooms.at(i)->getName() + "!";
        }
        if (rooms.at(i)->getEnemies().size() > 0) {
            roomNames.at(i) = "!" + rooms.at(i)->getName() + "!";
        }
        if(player.getCurrentRoom() == rooms.at(i) && ((mojo.getCurrentRoom() == rooms.at(i) && enemyCheck()) || rooms.at(i)->getEnemies().size() > 0)) {
            roomNames.at(i) = "[" + rooms.at(i)->getName() + "!";
        }
    }
    cout << " " << roomNames.at(3) << " -  " << roomNames.at(4) << "  - " << roomNames.at(5) << endl;
    cout << "         |   " << endl;
    cout << " " << roomNames.at(1) << " -  " << roomNames.at(0) << "  - " << roomNames.at(2) << endl;
    cout << "         |    " << endl;
    cout << " " << roomNames.at(6) << " -  " << roomNames.at(7) << "  - " << roomNames.at(8) << endl;
    cout << "         |    " << endl;
    cout << "        "<< roomNames.at(9) << endl;
}

void Game::info()
{
    cout << "Available commands:" << endl;
    cout << " - go <direction>"   << endl;
    cout << " - teleport"         << endl;
    cout << " - take <itemName>"  << endl;
    cout << " - inventory"        << endl;
    cout << " - map"              << endl;
    cout << " - info"             << endl;
    cout << " - attack <enemyName>" << endl;
    cout << " - use <itemName>" << endl;
    cout << endl;
    cout << "The more items you have in your inventory, the more stamina you use when you move!" << endl;
    cout << "If you have a weapon, you will be more likely to hurt the enemy when attacking" << endl;
}

bool Game::enemyCheck() {
    if(mojo.getHealth() <= 0 || mojo.getStamina() <= 0) {
        return false;
    }
    return true;
}

void Game::enemyMove() {
    if(enemyCheck()) {
        string direction = "";
        Room *enemyNext = mojo.getCurrentRoom()->getExit(direction);

        while(enemyNext == nullptr) {
            int random = rand() % 4;
            string directions[4] = {"north", "east", "south", "west"};
            enemyNext = mojo.getCurrentRoom()->getExit(directions[random]);
        }
        mojo.setCurrentRoom(enemyNext);
        mojo.setStamina(mojo.getStamina() - 4);
        if(!enemyCheck()) {
            cout << "An enemy died due to a lack of stamina!";
        }
    }
}

void Game::go(string direction)
{
    Room *next = player.getCurrentRoom()->getExit(direction);

    if(next == rooms[6] && !(player.hasItem("key"))) {
        cout << "You need a key to enter this room." << endl;
        cout << "Search for it in another room and then you can enter this one." << endl;
    } else {
        if (next != nullptr) {
            enemyMove();
            player.setCurrentRoom(next);
            if(player.itemInventory.size() > 0) {
                double sumWeight = 0;
                for(unsigned int i = 0; i < player.itemInventory.size(); i++) {
                    sumWeight += player.itemInventory.at(i)->getWeight();
                }
                if(sumWeight > 100) {
                    player.setStamina(player.getStamina() - 7);
                } else if(sumWeight >= 30) {
                    player.setStamina(player.getStamina() - 5);
                } else {
                    player.setStamina(player.getStamina() - 3);
                }
            } else {
                player.setStamina(player.getStamina() - 1);
            }
            EventManager::getInstance().trigger("enterRoom", next);
        } else {
            cout << "You hit a wall" << endl;
        }
    }
}

void Game::teleport()
{
    int selected = rand() % rooms.size();
    while(rooms[selected] == rooms [6] && !(player.hasItem("key"))) {
        selected = rand() % rooms.size();
    }
    player.setCurrentRoom(rooms[selected]);
    player.setStamina(player.getStamina() - 30);
    EventManager::getInstance().trigger("enterRoom", rooms[selected]);

    enemyMove();
}

void Game::use(string itemName) {
    if(itemName.compare("potion") == 0 && player.getHealth() <= 80) {
        //4. Unary operator overloading (example)
        ++player;
        cout << "You have used a replenishing potion!" << endl;
    } else {
        cout << "You must have 80 or less health points to use the health potion." << endl;
    }
}

void Game::take(string itemName) {
    vector<Item*> items = player.getCurrentRoom()->getItems();
    if(items.size() == 0) {
           cout << "No items in room.";
    } else {
            for(unsigned int i = 0; i < items.size(); i++) {
                if(items.at(i)->getShortDescription().compare(itemName) == 0) {
                    Item *newItem = (Item *) items.at(i);
                    player.addItem(newItem);
                    player.getCurrentRoom()->removeItem(newItem);
                    if(newItem->getShortDescription().compare("cursed_item") == 0) {
                        cout << "Oh no! You've picked up a cursed item.\n" << endl;
                        EventManager::getInstance().trigger("curse", &player);
                    }
                    if(newItem->getShortDescription().compare("potion") == 0) {
                        this->use("potion");
                    }
                }
            }
    }    
    cout << player.getItems() << endl;
}

void Game::attack(string name) {
    vector<Enemy*> enemies = player.getCurrentRoom()->getEnemies();
    int winner;
       if(enemies.size() > 0 || (mojo.getCurrentRoom() == player.getCurrentRoom() && enemyCheck())) {
            if(player.hasItem("sword")) {
                winner = (rand() % 20) + sword->getMultiplier();
            } else {
               winner = rand() % 20;
            }
            if(winner < 10) {
                //4. Unary Operator Overloading (example)
                --player;
                if(player.getHealth() <= 0) player.setHealth(0);
                    cout << "\nYou were injured by the enemy!\n";
                } else {
                    if(name.compare("mojo") == 0 && mojo.getCurrentRoom() == player.getCurrentRoom() && enemyCheck()) {
                        mojo.setHealth(mojo.getHealth() - 15);
                        if(enemyCheck()) {
                            cout << "\nYou injured the enemy!\n";
                        } else {
                            cout << "\nYou killed the enemy!\n"; 
                        }
                    } else {
                        for(unsigned int i = 0; i < enemies.size(); i++) {
                            if(enemies.at(i)->getName().compare(name) == 0 ) {
                                enemies.at(i)->setHealth(enemies.at(i)->getHealth() - 20);
                                if(enemies.at(i)->getHealth() <= 0) {
                                    player.getCurrentRoom()->removeEnemy(enemies.at(i));
                                    cout << "\nYou killed the enemy!\n";
                                } else {
                                    cout << "\nYou injured the enemy!\n";
                                }
                            }
                        }
                    } 
                }
        
        } else {
            cout << "\nNo enemies to attack\n";
    }
}

void Game::inventory()
{
    cout << player.getItems() << endl;
}

bool Game::is_over()
{
    return gameOver;
}

Player &Game::getPlayer()
{
    return player;
}

void Game::update_screen()
{
    if (!gameOver) {
        Room *currentRoom = player.getCurrentRoom();

        cout << endl;
        cout << "You are in " << currentRoom->getName() << endl;
        cout << player.getCurrentRoom()->displayItems() << endl;
        
        vector<Enemy*> enemies = player.getCurrentRoom()->getEnemies();
        cout << "Enemies in room = ";
        if(enemies.size() != 0) {
          for(unsigned int i = 0; i < enemies.size(); i++) {
            cout << enemies.at(i)->getName() << " - ";
            cout << "HP: " << enemies.at(i)->getHealth() << " ST: " << enemies.at(i)->getStamina() << "     ";
          }
          if(player.getCurrentRoom() == mojo.getCurrentRoom()) {
                if(enemyCheck()) {
                    cout << "Mojo - HP: " << mojo.getHealth() << " ST: " << mojo.getStamina();
                } else {
                    cout << "";
                }
            }
            cout << endl;
        } else {
            if(player.getCurrentRoom() != mojo.getCurrentRoom()) {
                cout << "none" << endl;
            } else {
                if(enemyCheck()) {
                    cout << "Mojo - HP: " << mojo.getHealth() << " ST: " << mojo.getStamina() << endl;
                } else {
                    cout << "none" << endl;
                }
            }
        }

        cout << "Exits:";
        if (currentRoom->getExit("north") != nullptr) { cout << " north"; }
        if (currentRoom->getExit("east")  != nullptr) { cout << " east";  }
        if (currentRoom->getExit("south") != nullptr) { cout << " south"; }
        if (currentRoom->getExit("west")  != nullptr) { cout << " west";  }
        cout << endl;

        cout << "HP: " << player.getHealth() << " ST: " << player.getStamina() << endl;

    } else {
        cout << "Type \"restart\" or \"exit\"." << endl;
    }
}


//golistener.cpp
GoListener::GoListener(Game *game)
{
    this->game = game;
    direction  = "";
}

void GoListener::run(void *args_ptr)
{
    if (game->is_over()) {
        return;
    }

    vector<string> *args = (vector<string> *) args_ptr;

    if (args->size() > 1) {
        game->go(args->at(1));
    } else {
        cout << "Need a direction!" << endl;
    }
}

//infolistener.cpp
InfoListener::InfoListener(Game *game)
{
    this->game = game;
}

void InfoListener::run(void *)
{
    game->info();
}

//inputlistener.cpp

InputListener::InputListener(Game *game)
{
    this->game = game;
}

void InputListener::run(void *args_ptr)
{
    // Let's make this thing a bit safer by "translating" the pointer back to what we expect
    // This is exactly what makes this "void *" pointer dangerous.
    vector<string> *args = (vector<string> *) args_ptr;

    EventManager &eventManager = EventManager::getInstance();

    if (args->size() > 0) {
        // If arg[0] is "input", we are going to ignore the input.
        // If we do not ignore the input, it's an infinite loop.
        if (args->at(0) == "input") {
            return;
        }

        eventManager.trigger(args->at(0), args);

        if (eventManager.is_running()) {
            game->update_screen();
        }
    } else {
        eventManager.trigger("no_command", nullptr);
    }
}

//inventorylistener.cpp
InventoryListener::InventoryListener(Game *game)
{
    this->game = game;
}

void InventoryListener::run(void *args_ptr)
{
    game->inventory();
}

//item.cpp
//9. Initializer list
Item::Item (string inDescription, float inWeightGrams, int multiplier) : description(inDescription), multiplier(multiplier) {
	setWeight(inWeightGrams);
}

//3. Templates
template <typename T>
void Item::setWeight(const T& inWeightGrams)
{
    if (inWeightGrams > 9999 || inWeightGrams < 0)
       cout << "weight invalid, must be 0<weight<9999" ;
    else
	   weightGrams = inWeightGrams;
}
template void Item::setWeight<int>(const int&);
template void Item::setWeight<float>(const float&);

float Item::getWeight() {
    return weightGrams;
}

int Item::getMultiplier() {
    return multiplier;
}

string Item::getShortDescription()
{
	return description;
}

string Item::getLongDescription()
{
    string temp = to_string(getWeight());
    temp.erase(temp.find_last_not_of('0') + 1, std::string::npos);
    temp.erase(temp.find_last_not_of('.') + 1, std::string::npos );
    return description + ", weight: " + temp + "g.\n";
}

//5. Binary Operator Overloading
bool operator==(const Item &item1, const Item &item2) {
    return item1.description == item2.description;
}

//maplistener.cpp

MapListener::MapListener(Game *game)
{
    this->game = game;
}

void MapListener::run(void *)
{
    game->map();
}

//restartlistener.cpp
RestartListener::RestartListener(Game *game)
{
    this->game = game;
}

void RestartListener::run(void *)
{
    game->reset(false);
}

//room.cpp
Room::Room(string name) :
    name(name)
{
    exits["north"] = nullptr;
    exits["east"]  = nullptr;
    exits["south"] = nullptr;
    exits["west"]  = nullptr;
}

string Room::getName()
{
    return name;
}

void Room::setExits(Room *north, Room *east, Room *south, Room *west)
{
    exits["north"] = north;
    exits["east"]  = east;
    exits["south"] = south;
    exits["west"]  = west;
}

Room *Room::getExit(string direction)
{
    return exits[direction];
}

void Room::addItem(Item *inItem) {
    itemsInRoom.push_back(inItem);
}

string Room::displayItems() {
    string tempString = "Items in room = ";
    int sizeItems = (itemsInRoom.size());
    if (itemsInRoom.size() < 1) {
        tempString = "No items in room";
        }
    else if (itemsInRoom.size() > 0) {
       int x = (0);
        for (int n = sizeItems; n > 0; n--) {
            tempString = tempString + itemsInRoom[x]->getShortDescription() + "  " ;
            x++;
            }
        }
    return tempString;
    }

int Room::numberOfItems() {
    return itemsInRoom.size();
}

vector<Item*> Room::getItems() { return itemsInRoom; }

void Room::removeItem(Item *newItem) {
        for (unsigned int i = 0; i < itemsInRoom.size(); i++) {
            //5. Binary Operator Overloading (item class)
            if(itemsInRoom.at(i) == newItem) {
                itemsInRoom.erase(itemsInRoom.begin() + i);
            }
        }
}

bool Room::hasItem(Item *inItem) {
    for (unsigned int i = 0; i < itemsInRoom.size(); i++) {
        //5. Binary Operator Overloading (item class)
        if (itemsInRoom.at(i) == inItem) {
                return true;
        }
    }
    return false;
}

bool Room::hasEnemy(Enemy *enemy) {
    for (unsigned int i = 0; i < enemiesInRoom.size(); i++) {
        //5. Binary Operator Overloading (enemy class)
        if (enemiesInRoom.at(i) == enemy) {
                return true;
        }
    }
    return false;
}

void Room::addEnemy(Enemy *enemy) {
    enemiesInRoom.push_back(enemy);
}

vector<Enemy*> Room::getEnemies() { return this->enemiesInRoom; }

string Room::displayEnemies() {
    string tempString = "Enemies in room = ";
    int sizeEnemies = (enemiesInRoom.size());
    if (enemiesInRoom.size() < 1) {
        tempString = "No enemies in room";
        }
    else if (enemiesInRoom.size() > 0) {
       int x = (0);
        for (int n = sizeEnemies; n > 0; n--) {
            tempString = tempString + enemiesInRoom[x]->getName() + "  " ;
            x++;
            }
        }
    return tempString;
}

void Room::removeEnemy(Enemy *enemy) {
        for (unsigned int i = 0; i < enemiesInRoom.size(); i++) {
            //5. Binary Operator Overloading (enemy class)
             if(enemiesInRoom.at(i) == enemy) {
                enemiesInRoom.erase(enemiesInRoom.begin() + i);
            }
        }
}

//takelistener.cpp
TakeListener::TakeListener(Game *game)
{
    this->game = game;
    itemName = "";
}

void TakeListener::run(void *args_ptr)
{
    if (game->is_over()) {
        return;
    }

    vector<string> *args = (vector<string> *) args_ptr;

    if (args->size() > 1) {
        game->take(args->at(1));
    } else {
        cout << "Need to choose an item to take!" << endl;
    }
}

//teleportlistener.cpp
TeleportListener::TeleportListener(Game *game)
{
    this->game = game;
}

void TeleportListener::run(void *)
{
    if (game->is_over()) {
        return;
    }

    this->game->teleport();
}

//uselistener.cpp 
UseListener::UseListener(Game *game)
{
    this->game = game;
    itemName = "";
}

void UseListener::run(void *args_ptr)
{
    if (game->is_over()) {
        return;
    }

    vector<string> *args = (vector<string> *) args_ptr;

    if (args->size() > 1) {
        game->use(args->at(1));
    } else {
        cout << "Need to choose an item to use!" << endl;
    }
}

//victorylistener.cpp
VictoryListener::VictoryListener(Game *game)
{
    this->game = game;
}

void VictoryListener::run(void *)
{
    cout << endl;
    cout << "Victory!" << endl;
    game->setOver(true);
}

//weapon.cpp
//2. Inheritance (including virtual methods) and cascading constructors
//9. Initializer list
Weapon::Weapon(string inDescription, float inWeightGrams, int multiplier) : Item(inDescription, inWeightGrams, multiplier) {
	
}

//7. Virtual functions and polymorphism
//10. Dynamic and static dispatch
string Weapon::getLongDescription()
{
    string temp = to_string(getWeight());
    temp.erase(temp.find_last_not_of('0') + 1, std::string::npos);
    temp.erase(temp.find_last_not_of('.') + 1, std::string::npos );
    return description + ", is a weapon, weight: " + temp + "g, " + "multiplier: " + to_string(multiplier) + "\n";
}

//main.cpp
int main()
{
    Game game;
    EventManager::getInstance().listen("input", new InputListener(&game));
    EventManager::getInstance().event_loop();
    return EXIT_SUCCESS;
}