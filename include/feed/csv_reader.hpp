#pragma once

#include <fstream>
#include <optional>
#include <cstdint>

#include <feed/event.hpp> // Event

// Reads the next non-empty underlying row from the CSV stream and returns it as an Event.
// Returns std::nullopt when EOF is reached.
std::optional<Event> read_next_underlying(std::ifstream& f);

// Reads the next non-empty option row from the CSV stream and returns it as an Event.
// Returns std::nullopt when EOF is reached.
std::optional<Event> read_next_option(std::ifstream& f);
