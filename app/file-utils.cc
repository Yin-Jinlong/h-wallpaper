#include <file-utils.h>

bool file_exists(const std::string &name) {
    struct stat buffer{0};
    return (stat(name.c_str(), &buffer) == 0);
}

bool file_create_empty(const std::string &name) {
    std::ofstream file(name, std::ios::out);
    if (file.is_open()) {
        file.close();
        return true;
    }
    return false;
}

bool file_write(const std::string name, std::string content) {
    std::ofstream file(name, std::ios::out);
    if (file.is_open()) {
        file << content;
        file.close();
        return true;
    }
    return false;
}
