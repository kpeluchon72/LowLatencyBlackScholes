#include <iostream>

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x / std::sqrt(2.0)
    );
}


double normalCDFfast(double x) {
    

}
