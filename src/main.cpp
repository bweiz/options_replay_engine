#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <stdexcept>

enum class Right { Call, Put };

struct UnderlyingRow {
    std::int64_t ts;
    double open;
    double high;
    double low;
    double close;
    std::int64_t volume;
};
struct OptionRow {
    std::int64_t ts;
    std::int64_t expiry_s;
    double strike;
    Right right;
    double bid;
    double ask;
    double iv;
};

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
int64_t parse_utc_to_s(const std::string& s) {
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

char right_to_char(Right r) {
    return (r == Right::Call) ? 'C' : 'P';
}
// ---------------------------------------------------------
int main()
{
    const std::vector<std::string> u_labels = {"ts","open","high","low","close","volume"};
    const std::vector<std::string> o_labels = {"ts","expiry","strike","right","bid","ask","iv"};

    std::ifstream underlying("data/underlying.csv");
    if (!underlying) {
        std::cerr << "Failed to open underlying \n";
        return 1;
    }
    
    std::string line1;
    std::getline(underlying, line1);            // Skip first line
    
    std::cout << "UNDERLYING fields\n";
    while (std::getline(underlying, line1)) {
        if (line1.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }
        //std::vector<std::string> underlyingLine = parseLine(line1);
        auto u_field = splitLine(line1); 
        if (u_field.size() != u_labels.size()) {
            std::cerr << "Underlying field count mismatch: got " << u_field.size()
              << ", expected " << u_labels.size() << "\n";
            return 1;
        }

        UnderlyingRow u_row = parse_underlying_row(u_field);
        std::cout << "ts=" << u_row.ts
          << " open=" << u_row.open
          << " high=" << u_row.high
          << " low=" << u_row.low
          << " close=" << u_row.close
          << " volume=" << u_row.volume << "\n";
    }
    
    

    std::ifstream options("data/options.csv");

    if (!options) {
        std::cerr << "Failed to open options \n";
        return 1;
    }
    
    std::string line2;

    std::getline(options, line2);            // Skip first line
    
    std::cout << "OPTIONS fields\n";
    while (std::getline(options, line2)) {
        if (line2.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }
        //std::vector<std::string> optionsLine = parseLine(line2);
        auto o_field = splitLine(line2);
        if (o_field.size() != o_labels.size()) {
            std::cerr << "Options field count mismatch: got " << o_field.size()
              << ", expected " << o_labels.size() << "\n";
            return 1;
        }

        OptionRow o_row = parse_option_row(o_field);
        std::cout << "ts=" << o_row.ts
          << "  expiry=" << o_row.expiry_s
          << "  strike=" << o_row.strike
          << " right=" << right_to_char(o_row.right)
          << " bid=" << o_row.bid
          << " ask=" << o_row.ask
          << " iv=" << o_row.iv << "\n";
    }
    return 0;
}
