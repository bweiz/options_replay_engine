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
#include <pricing/black_scholes.hpp>


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
    OptionKey target;

    target.expiry_s = 1737072000;
    target.strike_x100 = 10000;
    target.right = Right::Call;

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
        if (!ms.has_underlying()) {
            continue;
        }
        else {
            auto quote_update = ms.get_option_quote(target);
            if (!quote_update.has_value()) {
                continue;
            }
            else {
                OptionRow row = *quote_update;      // Reference to optional type 
                double mid = (row.bid + row.ask) / 2;
                double T = static_cast<double>(target.expiry_s - ms.now()) / 31536000;
                double theo = black_scholes(ms.latest_underlying().close, target.strike_x100/100.0, T, row.iv, 0.03, row.right);
                double edge{ theo - mid };
                std::cout << "now: " << ms.now() << " S: " << ms.latest_underlying().close << " K: " << target.strike_x100/100.0 << " mid: " << mid
                    << " theo: " << theo << " edge: " << edge << " iv: " << row.iv
                    << " T: " << T << '\n';
            }
        }
        std::cout << ms.now() <<  " hasU: " << ms.has_underlying() 
                  << " count: " << ms.option_count() <<  '\n';
    }
    
    return 0;
}
