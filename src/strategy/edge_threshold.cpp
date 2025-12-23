#include "strategy/edge_threshold.hpp"

#include <cmath>        // std::fabs etc (and your BS math will need <cmath>)
#include <algorithm>    // std::max (if your BS uses it)
#include <stdexcept>

#include "pricing/black_scholes.hpp"  // you provide: double black_scholes(...)
                                      



