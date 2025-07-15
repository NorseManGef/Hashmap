unsigned long long hash_integral(unsigned long long integral)
{
    integral = (integral ^ (integral >> 30)) * 0xbf58476d1ce4e5b9UL;
    integral = (integral ^ (integral >> 27)) * 0x94d049bb133111ebUL;
    integral = integral ^ (integral >> 31);
    return integral;
}

unsigned long long hash_string(const char* str)
{
    unsigned long long hashVal = 0;
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
