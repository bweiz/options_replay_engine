#include <feed/csv.hpp>


#include <sstream>
#include <ctime>
#include <stdexcept>
#include <cstdlib>

// ------------- CSV LineSplitter ---------------------------
std::vector<std::string> splitLine(const std::string& line){
    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> parsedString;

    while(std::getline(ss, token, ',')) {
        parsedString.push_back(token);
    }

    return parsedString;
}
// ---------------------------------------------------------

// ----------- Convert UTC to NS ---------------------------
// Convert "YYYY-MM-DDTHH:MM:SSZ" to epoch seconds (UTC)
std::int64_t parse_utc_to_s(const std::string& s) {
    std::tm tm{};

    tm.tm_year = std::stoi(s.substr(0, 4)) - 1900;
    tm.tm_mon  = std::stoi(s.substr(5, 2)) - 1;
    tm.tm_mday = std::stoi(s.substr(8, 2));
    tm.tm_hour = std::stoi(s.substr(11,2));
    tm.tm_min  = std::stoi(s.substr(14,2));
    tm.tm_sec  = std::stoi(s.substr(17,2));

    std::time_t t = timegm(&tm);
    return static_cast<std::int64_t>(t);
}
// ---------------------------------------------------------

// ----------- Line Parsers --------------------------------
UnderlyingRow parse_underlying_row(const std::vector<std::string>& f) {
    if (f.size() != 6) {
        throw std::runtime_error("Underlying Row expected 6 fields, got " + 
                                    std::to_string(f.size()));
    }

    UnderlyingRow row{};

    row.ts = parse_utc_to_s(f[0]);

    row.open = std::stod(f[1]); 
    row.high = std::stod(f[2]);
    row.low = std::stod(f[3]);
    row.close = std::stod(f[4]);
    row.volume = std::stoll(f[5]);

    if (row.open < 0 || row.high < 0 || row.low < 0 || row.close < 0) {
        throw std::runtime_error("Underlying row: negative price");
    }
    if (row.high < row.low) {
        throw std::runtime_error("Underlying row: high < low");
    }

    return row;
}

OptionRow parse_option_row(const std::vector<std::string>& f) {
    if (f.size() != 7) {
        throw std::runtime_error("Option Row expected 7 fields, got " +
                                    std::to_string(f.size()));
    }

    OptionRow row{};

    row.ts = parse_utc_to_s(f[0]);

    row.expiry_s = parse_utc_to_s(f[1] + "T00:00:00Z");

    row.strike = std::stod(f[2]);
    

    if (f[3] == "C") row.right = Right::Call;
    else if (f[3] == "P") row.right = Right::Put;
    else throw std::runtime_error("Option Row must be C or P, got "  + f[3]);

    row.bid = std::stod(f[4]);
    row.ask = std::stod(f[5]);
    row.iv = std::stod(f[6]);
    
    if (row.bid < 0 || row.ask < 0 || row.ask < row.bid) {
        throw std::runtime_error("Option row: invalid bid/ask");
    }
    if (row.iv <= 0) {
        throw std::runtime_error("Option row: iv must be > 0");
    }

    return row;
}

