#include <feed/csv_reader.hpp>

#include <fstream>
#include <optional>
#include <string>

#include <feed/csv.hpp>      // splitLine, parse_underlying_row, parse_option_row
#include <feed/event.hpp>    // Event, EventType, UnderlyingRow, OptionRow

// ------------ Event Creation -----------------------------
std::optional<Event> read_next_underlying(std::ifstream& f) {
     

    std::string line;
    while (std::getline(f, line)){ 
        if(line.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }
        Event event{};
        auto field = splitLine(line);
        
        UnderlyingRow row = parse_underlying_row(field);
        event.ts = row.ts;
        event.type = EventType::Underlying;
        event.underlying = row;

        return event;
    }
    return {};
}

std::optional<Event> read_next_option(std::ifstream& f) {
    std::string line;
    while(std::getline(f, line)) {
        if(line.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }
        Event event{};
        auto field = splitLine(line);

        OptionRow row = parse_option_row(field);
        event.ts = row.ts;
        event.type = EventType::Option;
        event.option = row;

        return event;
    }
    return {};
}

