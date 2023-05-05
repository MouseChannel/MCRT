#include <cstddef>
static constexpr int align_up(int x, size_t a)
{
    return int((x + (int(a) - 1)) & ~int(a - 1));
}
