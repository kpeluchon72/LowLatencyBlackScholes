#include "greeks.hpp"
#include <iostream>
#include <cmath>
#include "errorfunc.hpp"
#include <iomanip>


double deltaCall(double d1) {
    return polynomialRationalCDF(d1);
};

double deltaPut(double d1) {
    return polynomialRationalCDF(d1) - 1;
};

double actDeltaCall(double d1) {
    return 0.5 * std::erfc(-d1 / std::sqrt(2.0));
};

double actDeltaPut(double d1) {
    return 0.5 * std::erfc(-d1 / std::sqrt(2.0)) - 1;
};



// Test

int main(){
    
    for (int i = 0; i < 10; ++i) {
        std::cout << "Deltacall:     " << std::setprecision(10) << deltaCall(i) << "\n";
        std::cout << "Deltaput:      " << std::setprecision(10) << deltaPut(i) << "\n";
        std::cout << "Act Deltacall: " << std::setprecision(10) << actDeltaCall(i) << "\n";
        std::cout << "Act Deltaput:  " << std::setprecision(10) << actDeltaPut(i) << "\n";
    }

    return 0;
};

