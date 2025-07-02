#ifndef CORRUPTION_HPP
#define CORRUPTION_HPP
#include <vector>
#include <random>


namespace crp {

std::default_random_engine eng;
std::uniform_real_distribution<double> dist_x{0., 10.};
auto corruption (std::vector <int> binary){
    for (int n{0}; n< (64*64); n+=1 ){
        if (dist_x(eng)>9){
            binary[n]*=-1;
        }
    
    }
    return binary;
}
} // namespace crp

#endif