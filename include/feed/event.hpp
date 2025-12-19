




enum class Right { Call, Put };

enum class EventType { Option, Underlying };

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

struct Event {
    std::int64_t ts;
    EventType type;
    UnderlyingRow underlying;
    OptionRow option;
};
