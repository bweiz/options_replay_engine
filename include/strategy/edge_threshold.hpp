#pragma once
#include <cstdint>
#include <optional>
#include <market/market_state.hpp>




// What main consumes:
enum class Action {
    None,
    EnterLong,
    Exit
};

// Knobs  tune:
struct StrategyConfig {
    double entry_edge{1.0};   // Enter if edge >= this
    double exit_edge{0.2};    // Exit if edge <= this
};


// Memory that persists across calls:
struct PositionState {
    bool in_position{false};
    double entry_price{0.0};
    std::int64_t entry_ts{0};
};

// store the last computed numbers so main can print them cleanly
struct LastSignal {
    bool valid{false};
    double S{0.0};
    double K{0.0};
    double mid{0.0};
    double theo{0.0};
    double edge{0.0};
    double iv{0.0};
    double T{0.0};
    double ask{0.0};
    double bid{0.0};
};

struct EdgeThresholdStrategy {
    StrategyConfig cfg{};
    PositionState pos{};
    LastSignal last{};  // updated whenever we can compute a quote

    // Core call: update last-signal, possibly flip position state, return action
    Action evaluate(const MarketState& ms, const OptionKey& target, double r);
};


