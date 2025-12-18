#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> parseLine(const std::string& line){
    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> parsedString;

    while(std::getline(ss, token, ',')) {
        parsedString.push_back(token);
    }

    return parsedString;
}
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
    if (std::getline(underlying, line1)) {
        //std::vector<std::string> underlyingLine = parseLine(line1);
        auto u_field = parseLine(line1);
        if (u_field.size() != u_labels.size()) {
            std::cerr << "Underlying field count mismatch: got " << u_field.size()
              << ", expected " << u_labels.size() << "\n";
            return 1;
}
        for (size_t x = 0; x < u_field.size(); x++)
        {
            std::cout << x << " (" << u_labels[x] << ") " << u_field[x] << '\n';
        }
    }
    
    

    std::ifstream options("data/options.csv");

    if (!options) {
        std::cerr << "Failed to open options \n";
        return 1;
    }
    
    std::string line2;

    std::getline(options, line2);            // Skip first line
    
    std::cout << "OPTIONS fields\n";
    if (std::getline(options, line2)) {
        //std::vector<std::string> optionsLine = parseLine(line2);
        auto o_field = parseLine(line2);
        if (o_field.size() != o_labels.size()) {
            std::cerr << "Underlying field count mismatch: got " << o_field.size()
              << ", expected " << o_labels.size() << "\n";
            return 1;
}
        for (size_t y = 0; y < o_field.size(); y++)
        {
            std::cout << y << " (" << o_labels[y] << ") " << o_field[y] << '\n';
        }
    }
    return 0;
}
