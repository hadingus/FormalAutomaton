#include <gtest/gtest.h>
#include "automaton.h"


TEST(Pain, 1) {
    Automaton a;
    a.add_finish(2);
    EXPECT_EQ(1 + 1, 2);
}
