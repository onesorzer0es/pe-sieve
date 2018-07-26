#pragma once

#include <Windows.h>
#include <sstream>

char* get_file_name(char *full_path);

char* get_directory(IN char *full_path, OUT char *out_buf, IN const size_t out_buf_size);

char* get_subpath_ptr(char *modulePath, char* searchedPath);

bool convert_to_wow64_path(char *szModName);

std::string to_lowercase(std::string);

//add escape characters to path separators
std::string escape_path_separators(std::string path);
