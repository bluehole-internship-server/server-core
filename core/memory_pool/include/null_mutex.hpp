//
// null_mutex.hpp
//

#pragma once

class null_mutex {
public:
    inline void lock() {};
    inline void unlock() {};
};
