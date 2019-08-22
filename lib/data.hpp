#pragma once

#include "utils.hpp"
#include <fmt/format.h>
#include <string>
#include <vector>
#include "spdlog/spdlog.h"

double get_data_value(std::string indicator,
                      std::string country = "",
                      std::string state = "",
                      int year = 2012,
                      int month = 1,
                      std::string unit = "");

