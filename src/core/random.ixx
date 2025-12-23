module;
#include <random>
#include <chrono>

export module random;
import types;

std::default_random_engine s_rand_engine;

export namespace random {    
    void init() {
        s_rand_engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
    void seed(cU32 s) {
        s_rand_engine.seed(s);
    }
    I32 number(cI32 min, cI32 max) {
        std::uniform_int_distribution<I32> dist(min, max);
        return dist(s_rand_engine);
    }
    F64 real_number(cF64 min, cF64 max) {
        std::uniform_real_distribution<F64> dist(min, max);
        return dist(s_rand_engine);
    }
}