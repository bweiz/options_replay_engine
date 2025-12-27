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
#include <strategy/edge_threshold.hpp>

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
    EdgeThresholdStrategy strat;
    strat.cfg.entry_edge = 1.6;
    strat.cfg.exit_edge = 1.4;

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
                Action a = strat.evaluate(ms, target, 0.03);
                const bool DEBUG_STEP = true;
                if (DEBUG_STEP) {
                    std::cout
                        << "now=" << ms.now()
                        << " hasU=" << ms.has_underlying()
                        << " optCount=" << ms.option_count();

                    // Did we compute a quote + pricing this step?
                    if (strat.last.valid) {
                        std::cout
                            << " S=" << strat.last.S
                            << " K=" << strat.last.K
                            << " mid=" << strat.last.mid
                            << " theo=" << strat.last.theo
                            << " edge=" << strat.last.edge
                            << " iv=" << strat.last.iv
                            << " T=" << strat.last.T;
                    } else {
                        std::cout << " (no signal: missing underlying or quote)";
                    }

                    // What did the strategy decide?
                    std::cout << " action=";
                    switch (a) {
                        case Action::None:      std::cout << "None"; break;
                        case Action::EnterLong: std::cout << "ENTER"; break;
                        case Action::Exit:      std::cout << "EXIT"; break;
                    }
                    std::cout << '\n';
                } 
            }
        }
        
    }
    
    return 0;
}
