#pragma once

#include "external.hpp"

/**
 * Get session title based HTTP error code
 */
std::string getTitle(int code);

/**
 * Get session subtitle based HTTP error code
 */
std::string getSubTitle(int code);