#include <string.hh>

uint64_t strlen(const char* string)
{
    uint64_t result = 0;
    while (*string++ != '\0') result++;
    return result;
}

char* strcat(char* destination, const char* source)
{
    char* ptr = destination + strlen(destination);

    while (*source != '\0') *ptr++ = *source++;
    *ptr = '\0';

    return ptr;
}

char* ultoa(uint64_t value, char* destination, int radix)
{
    static const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p, *q;

    q = destination;
    do
    {
        *q++ = digits[value % radix];
        value /= radix;
    }
    while (value);
    *q = '\0';

    p = destination;
    q--;

    while (p < q)
    {
        char c = *p;
        *p++ = *q;
        *q-- = c;
    }

    return destination;
}
