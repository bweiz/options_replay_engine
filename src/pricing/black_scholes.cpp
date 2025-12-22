
#include <pricing/black_scholes.hpp>

double norm_cdf(double x) {
    return (0.5*(1 + std::erf(x/std::sqrt(2))));
}
double black_scholes(double S, double K, double T, double sigma, double r, Right type) 
{
    
    if (sigma <= 0) {
        throw std::runtime_error("Bad sigma: " + std::to_string(sigma));
    }
    if (K <= 0) {
        throw std::runtime_error("Bad strike: " + std::to_string(K));
    }
    if (S <= 0) {
        throw std::runtime_error("Bad price: " + std::to_string(S));
    }
    double optionPrice{};
    if (T <= 0) {
        if (type == Right::Call) {
            optionPrice = std::max(S - K, 0.0); 
        }
        else {
            optionPrice = std::max(K - S, 0.0);
        }
        return optionPrice;
    }
    double d1{ (std::log(S/K) + (r + (0.5*(sigma * sigma)))*T)/(sigma*std::sqrt(T)) };
    double d2{ d1 - (sigma*std::sqrt(T)) };
    double df { std::exp(-r * T) };
    
    if (type == Right::Call) {
        optionPrice = (S * norm_cdf(d1)) - (K * df * norm_cdf(d2));
    }
    else {
        optionPrice = (K * df * norm_cdf(-d2)) - (S * norm_cdf(-d1));
    }
    return optionPrice;
}
