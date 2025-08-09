#include <stdexcept>
typedef unsigned long long hash_t;

hash_t hash_integral(hash_t integral);
hash_t hash_string(const char* str);

struct no_hash: public std::logic_error {
    no_hash(const char* message):
        std::logic_error(message) 
    {}

    static void throw_for_type(const std::type_info& type) {
        std::string message = std::string("no hash for type ") + type.name();
        throw no_hash(message.c_str());
    }
};

template <typename T>
hash_t hash(T obj) {
    no_hash::throw_for_type(typeid(T));
    return 0; //for linter
}

template <>
hash_t hash<char>(char obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<unsigned char>(unsigned char obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<int>(int obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<unsigned int>(unsigned int obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<short>(short obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<unsigned short>(unsigned short obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<long long>(long long obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<hash_t>(hash_t obj) {
    return hash_integral(obj);
}

template <>
hash_t hash<const char*>(const char* obj) {
    return hash_string(obj);
}

template <>
hash_t hash<const std::string&>(const std::string& obj) {
    return hash_string(obj.c_str());
}

hash_t hash_integral(hash_t integral)
{
    integral = (integral ^ (integral >> 30)) * 0xbf58476d1ce4e5b9UL;
    integral = (integral ^ (integral >> 27)) * 0x94d049bb133111ebUL;
    integral = integral ^ (integral >> 31);
    return integral;
}

hash_t hash_string(const char* str)
{
    hash_t hashVal = 0;
    while(*str != '\0')
    {
        hashVal = (hashVal << 4) + *(str++);
        unsigned long g = hashVal & 0xF0000000L;
        if(g != 0) hashVal ^= g >> 24;
        hashVal &= ~g;
        str += 1;
    }
    return hashVal;
}
