#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <feed/event.hpp>

// Split a CSV line into fields using ',' as delimiter.
// (No quote-handling yet; assumes simple CSV.)
std::vector<std::string> splitLine(const std::string& line);

// Parse UTC timestamp string like "YYYY-MM-DDTHH:MM:SSZ" into epoch seconds (UTC).
std::int64_t parse_utc_to_s(const std::string& s);

// Parse a vector of fields into strongly-typed rows.
// These throw std::runtime_error on malformed rows.
UnderlyingRow parse_underlying_row(const std::vector<std::string>& fields);
OptionRow parse_option_row(const std::vector<std::string>& fields);





