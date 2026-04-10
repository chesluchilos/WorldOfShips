#include "CommandHandler.hpp"

CommandHandler::CommandHandler(const std::string& filename) : config(filename) {}

void CommandHandler::loadConfig() {
    loadMappingFromFile(config);
}


void CommandHandler::loadDefaultMapping() {

    // Select
    keyMapping['W'] = Command::MOVE_UP;
    keyMapping['S'] = Command::MOVE_DOWN;
    keyMapping['A'] = Command::MOVE_LEFT;
    keyMapping['D'] = Command::MOVE_RIGHT;
    keyMapping['\n'] = Command::ENTER;

    // Menu
    keyMapping['P'] = Command::PLAY;
    keyMapping['C'] = Command::SAVE;
    keyMapping['V'] = Command::LOAD;
    keyMapping['Q'] = Command::EXIT;
    keyMapping[' '] = Command::CONTINUE;

    // Place
    keyMapping['R'] = Command::PLACE;
    keyMapping['O'] = Command::ORIENTATION;

    // Always
    keyMapping['F'] = Command::FIELD_SWITCH;
    keyMapping['X'] = Command::CANCEL;

    // Turn
    keyMapping['E'] = Command::END_TURN;
    keyMapping['Z'] = Command::ATTACK;
    keyMapping['U'] = Command::USE_ABILITY;
    keyMapping['H'] = Command::SHOW_ABILITIES;
    keyMapping['M'] = Command::MENU;

    // Other
    keyMapping['Y'] = Command::YES;
    keyMapping['N'] = Command::NO;

    for (const auto& pair : keyMapping) {
        reverseKeyMapping[pair.second] = pair.first;
    }
}


void CommandHandler::loadMappingFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw ConfigFileException();
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::exception& e) {
        throw LoadFileException();
    }

    for (auto& [key_str, command_str] : j.items()) {
        Command command;
        if (command_str == "MOVE_UP") command = Command::MOVE_UP;
        else if (command_str == "MOVE_DOWN") command = Command::MOVE_DOWN;
        else if (command_str == "MOVE_LEFT") command = Command::MOVE_LEFT;
        else if (command_str == "MOVE_RIGHT") command = Command::MOVE_RIGHT;
        else if (command_str == "ENTER") command = Command::ENTER;
        else if (command_str == "PLAY") command = Command::PLAY;
        else if (command_str == "SAVE") command = Command::SAVE;
        else if (command_str == "LOAD") command = Command::LOAD;
        else if (command_str == "EXIT") command = Command::EXIT;
        else if (command_str == "CONTINUE") command = Command::CONTINUE;
        else if (command_str == "PLACE") command = Command::PLACE;
        else if (command_str == "ORIENTATION") command = Command::ORIENTATION;
        else if (command_str == "FIELD_SWITCH") command = Command::FIELD_SWITCH;
        else if (command_str == "CANCEL") command = Command::CANCEL;
        else if (command_str == "END_TURN") command = Command::END_TURN;
        else if (command_str == "ATTACK") command = Command::ATTACK;
        else if (command_str == "USE_ABILITY") command = Command::USE_ABILITY;
        else if (command_str == "SHOW_ABILITIES") command = Command::SHOW_ABILITIES;
        else if (command_str == "MENU") command = Command::MENU;
        else if (command_str == "YES") command = Command::YES;
        else if (command_str == "NO") command = Command::NO;
        else {
            throw UnknownCommandException();
        }

        char key = key_str[0];

        if (keyMapping.count(key)) {
            throw SecondKeyException();
        }

        if (reverseKeyMapping.count(command)) {
            throw SecondCommandException();
        }
        
        keyMapping[key] = command;
        if (key == '\n') {
            reverseKeyMapping[command] = "ENTER";
        } else if (key == ' ') {
            reverseKeyMapping[command] = "SPACE";
        } else {
            reverseKeyMapping[command] = std::string(1, key);
        }
    }

    validateMapping();
}

void CommandHandler::validateMapping() {
    for (int i = static_cast<int>(Command::MOVE_UP); i <= static_cast<int>(Command::NO); ++i) {
        Command command = static_cast<Command>(i);
        if (reverseKeyMapping.find(command) == reverseKeyMapping.end()) {
            throw NoKeyException();
        }
    }
}

Command CommandHandler::getCommand() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); 
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (!keyMapping.empty() && std::islower(keyMapping.begin()->first)) {
        ch = std::tolower(ch);
    } else if (!keyMapping.empty() && std::isupper(keyMapping.begin()->first)) {
        ch = std::toupper(ch);
    }
    return keyMapping[ch];
}

std::string CommandHandler::getFileName() {
    std::string filename;
    std::getline(std::cin, filename);
    return filename;
}

std::unordered_map<Command, std::string>& CommandHandler::getReverseKeyMapping() {
    return reverseKeyMapping;
}