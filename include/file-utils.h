#pragma once

#include <pre.h>
#include <fstream>

bool file_exists(const std::string &name);

bool file_create_empty(const std::string &name);

bool file_write(const std::string name, std::string content);
