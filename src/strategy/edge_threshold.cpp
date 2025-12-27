#include "strategy/edge_threshold.hpp"

#include <cmath>        // std::fabs etc 
#include <algorithm>    // std::max 
#include <stdexcept>

#include "pricing/black_scholes.hpp"  
                                      
Action EdgeThresholdStrategy::evaluate(const MarketState& ms,
                                       const OptionKey& target,
                                       double r)
{
    last = {};

    // need underlying + quote
    if (!ms.has_underlying()) {
        return Action::None;
    }
    
    auto q = ms.get_option_quote(target);
    if (!q.has_value()) {
        return Action::None;
    }
    

    // pull inputs: S, K, T, mid, theo
    double S = ms.latest_underlying().close;
    double K = static_cast<double>(target.strike_x100) / 100.0;
    double T = static_cast<double>(target.expiry_s - ms.now()) / 31536000.0;

    if (T < 0.0) T = 0.0;

    const double mid = (q->bid + q->ask) / 2.0;

    const double theo = black_scholes(S, K, T, q->iv, r, q->right);
    const double edge = theo - mid;

    last.valid = true;
    last.S = S; last.K = K; last.mid = mid; last.theo = theo; last.bid = q->bid; last.ask = q->ask;
    last.edge = edge; last.iv = q->iv; last.T = T;

    // decision logic with memory
    if (!pos.in_position) {
        if (edge >= cfg.entry_edge) {
            pos.in_position = true;
            pos.entry_price = mid;
            pos.entry_ts = ms.now();
            return Action::EnterLong;
        }
        return Action::None;
    } else {
        if (edge <= cfg.exit_edge) {
            pos.in_position = false;
            return Action::Exit;
        }
        return Action::None;
    }
}
