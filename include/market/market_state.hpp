#pragma once

#include <cstdint>
#include <optional>
#include <unordered_map>

#include <../include/feed/event.hpp>

/*
 * MarketState:
 *   mutable snapshot of market as we replay events in order
 */

struct OptionKey {
    std::int64_t expiry_s{};
    std::int64_t strike_x100{};
    Right right{Right::Call};

    bool operator==(const OptionKey& other) const = default;
};

struct OptionKeyHash {
    std::size_t operator()(const OptionKey& k) const noexcept;
};


struct MarketState {
public:
    void apply(const Event& ev);

    std::int64_t now() const { return now_ts; }

    bool has_underlying() { return underlying_.has_value(); }
    const UnderlyingRow& latest_underlying() const; // throws if missing

    static OptionKey make_key(const OptionRow& row);

    std::optional<OptionRow> get_option_quote(const OptionKey& key) const;

    std::size_t option_count() const { return option_quotes_.size(); }
private:
    std::int64_t now_ts{0};                         // Timestamp of most recent event
    std::optional<UnderlyingRow> underlying_;       // latest underlying row
    
    std::unordered_map<OptionKey, OptionRow, OptionKeyHash> option_quotes_;
};
