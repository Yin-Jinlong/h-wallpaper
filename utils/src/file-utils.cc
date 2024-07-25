#include <file-utils.h>

bool file_exists(const std::string &name) {
    struct stat buffer {
        0
    };
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

BYTE *file_read(const std::string &name, size_t &len) {
    if (!file_exists(name))
        return nullptr;
    std::ifstream in(name, std::ios::in | std::ios::binary);
    if (in.is_open()) {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        BYTE *buffer = (BYTE *) malloc(size);
        in.read((char *) buffer, size);
        in.close();
        len = size;
        return buffer;
    }
    return nullptr;
}

bool file_write(const std::string &name, char *content, std::streamsize size) {
    std::ofstream file(name, std::ios::out | std::ios::binary);
    if (file.is_open()) {
        file.write(content, size);
        file.close();
        return true;
    }
    return false;
}

bool file_delete(const std::string &name) {
    return remove(name.c_str()) == 0;
}
