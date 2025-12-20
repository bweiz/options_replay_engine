#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <stdexcept>
#include <optional>
#include <feed/event.hpp>
#include <market/market_state.hpp>
#include <feed/csv.hpp>
#include <feed/csv_reader.hpp>


int main()
{
                                           

// ---- Open file, skip first -------------------------------------------
    std::ifstream underlying("data/underlying.csv");
    if (!underlying) {
        std::cerr << "Failed to open underlying \n";
        return 1;
    }
    
    std::string line1;
    std::getline(underlying, line1);            // Skip first line
                                                //
    std::ifstream options("data/options.csv");

    if (!options) {
        std::cerr << "Failed to open options \n";
        return 1;
    }
    
    std::string line2;
    std::getline(options, line2);            // Skip first line   
// ---------------------------------------------------------------------                         
    std::optional<Event> u_next{};
    std::optional<Event> o_next{};
    u_next = read_next_underlying(underlying);
    o_next = read_next_option(options);
    MarketState ms;
    while (u_next.has_value() || o_next.has_value()) {
        if (u_next.has_value() && o_next.has_value()) {
            Event* u_ev = &u_next.value();
            Event* o_ev = &o_next.value();
            if (u_ev->ts <= o_ev->ts) {
                ms.apply(*u_ev); 
                u_next = read_next_underlying(underlying);
            }
            else {
                ms.apply(*o_ev);
                o_next = read_next_option(options);
            }
        }
        else if (u_next.has_value() && !o_next.has_value()) {
            Event* u_ev = &u_next.value();
            ms.apply(*u_ev); 
            u_next = read_next_underlying(underlying);
        }
        else {
            Event* o_ev = &o_next.value(); 
            ms.apply(*o_ev);
            o_next = read_next_option(options);
        }
        std::cout << ms.now() <<  " hasU: " << ms.has_underlying() 
                  << " count: " << ms.option_count() <<  '\n';
    }
    
    return 0;
}
