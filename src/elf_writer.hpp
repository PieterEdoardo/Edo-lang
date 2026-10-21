#pragma once
#include <cstdint>
#include <string>
#include <vector>

void writeElfExecutable(const std::string& outputPath, const std::vector<uint8_t>& code);