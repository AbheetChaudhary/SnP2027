#include <ostream>
#include <stdio.h>

extern "C" {
    // int
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi(std::ostream& os, int n) {
        char buf[16]; snprintf(buf, sizeof(buf), "%d", n);
        return os << (const char*)buf;
    }

    // unsigned int
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEj(std::ostream& os, unsigned int n) {
        char buf[16]; snprintf(buf, sizeof(buf), "%u", n);
        return os << (const char*)buf;
    }

    // long
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEl(std::ostream& os, long n) {
        char buf[32]; snprintf(buf, sizeof(buf), "%ld", n);
        return os << (const char*)buf;
    }

    // unsigned long
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEm(std::ostream& os, unsigned long n) {
        char buf[32]; snprintf(buf, sizeof(buf), "%lu", n);
        return os << (const char*)buf;
    }

    // float
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf(std::ostream& os, float n) {
        char buf[32]; snprintf(buf, sizeof(buf), "%f", n);
        return os << (const char*)buf;
    }

    // double
    std::ostream& _ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd(std::ostream& os, double n) {
        char buf[32]; snprintf(buf, sizeof(buf), "%f", n);
        return os << (const char*)buf;
    }
}
