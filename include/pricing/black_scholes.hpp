#pragma once


#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <feed/event.hpp>

double black_scholes(double S, double K, double T, double sigma, double r, Right type); 

