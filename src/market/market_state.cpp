#include <iostream>
#include <unordered_map>
#include <cmath>
#include <functional>
#include <market/market_state.hpp>
/*
 * MarketState:
 *   mutable snapshot of market as we replay events in order
 */
std::size_t OptionKeyHash::operator()(const OptionKey& k) const noexcept {
        std::size_t h1 = std::hash<std::int64_t>{}(k.expiry_s);
        std::size_t h2 = std::hash<std::int64_t>{}(k.strike_x100);
        std::size_t h3 = std::hash<int>{}(static_cast<int>(k.right));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
}

OptionKey MarketState::make_key(const OptionRow& row) {
    OptionKey key{};

    key.expiry_s = row.expiry_s;

    key.strike_x100 = static_cast<std::int64_t>(std::llround(row.strike * 100));

    key.right = row.right;
    return key;
}

void MarketState::apply(const Event& ev) {
    now_ts = ev.ts;

    if (ev.type == EventType::Underlying) {
        underlying_ = ev.underlying;
        return;
    }
    else if (ev.type == EventType::Option) {
        OptionKey key = make_key(ev.option);
        option_quotes_[key] = ev.option;
        return;
    }
}

const UnderlyingRow& MarketState::latest_underlying() const {
    if (!underlying_.has_value()) {
        throw std::runtime_error("MarketState: underlying not available yet");
    }
    return *underlying_;
}

std::optional<OptionRow> MarketState::get_option_quote(const OptionKey& key) const {
    auto it = option_quotes_.find(key);
    if (it == option_quotes_.end()) {
        return std::nullopt;
    }
    return it->second;
}

