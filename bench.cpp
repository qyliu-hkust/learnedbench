
#include "utils/datautils.hpp"



int main(int argc, char const *argv[]) {
    
    // gen_uniform(argv[1], 2*1000*1000, 2, 1.0);
    vec_of_double_vec_t data;
    read_data(data, "data", 2*1000*1000, 2);

    for (int i=0; i<10; ++i) {
        for (double& x : data[i]) {
            std::cout << x << std::endl;
        }
    }
    
    return 0;
}
