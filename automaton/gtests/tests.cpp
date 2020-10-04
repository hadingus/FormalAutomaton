#include <gtest/gtest.h>
#include "automaton.h"


TEST(VertexTest, SimpleTest) {
    Vertex v(3);
    EXPECT_EQ(v.get_id(), 3);

    v.add_edge('a', 2);
    v.add_edge('b', 5);
    v.add_edge('b', 1);

    EXPECT_EQ(v.get_edge('a').size(), 1);
    EXPECT_EQ(v.get_edge('b').size(), 2);

    v.add_edge('a', 2);

    EXPECT_EQ(v.get_edge('a').size(), 1);
}

TEST(AutomatonTest, SigmaTest) {
    Automaton a("1abc", 0);
    EXPECT_EQ(a.get_sigma(), "1abc");
    a.set_sigma("1frd");
    EXPECT_EQ(a.get_sigma(), "1frd");
}

TEST(AutomatonTest, AddingVertices) {
    Automaton a("1abc", 0);

    EXPECT_EQ(a.v_size(), 1);
    a.add_Vertex(1);
    a.add_Vertex(2);
    a.add_Vertex(3);
    EXPECT_EQ(a.v_size(), 4);
    a.add_Vertex(2);
    EXPECT_EQ(a.v_size(),4);

    a.set_start(10);
    EXPECT_EQ(a.v_size(), 5);
    EXPECT_EQ(a.get_start(), 10);
    a.set_start(0);
    EXPECT_EQ(a.get_start(), 0);
}

TEST(AutomatonTest, AddingEdges) {
    Automaton a("1abc", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(0, 'b', 2);
    a.add_edge(1, 'c', 2);

    EXPECT_EQ(a.v_size(), 3);
    EXPECT_EQ(a.ed_size(), 3);

    EXPECT_EQ(a.get_next_Vertex(0, 'a').size(), 1);
    EXPECT_EQ(a.get_next_Vertex(0, 'a')[0], 1);
    a.add_edge(0, 'a', 3);
    EXPECT_EQ(a.get_next_Vertex(0, 'a').size(), 2);

    a.add_edge(0, 'a', 3);
    EXPECT_EQ(a.get_next_Vertex(0, 'a').size(), 2);
    EXPECT_EQ(a.get_next_Vertex(0, 'c').empty(), true);
}

TEST(AutomatonTest, AddingFinishes) {
    Automaton a("1abc", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(0, 'b', 2);
    a.add_edge(1, 'c', 2);
    a.add_edge(0, 'a', 3);

    a.add_finish(1);
    a.add_finish(2);

    EXPECT_EQ(a.get_finishes().size(), 2);
    a.add_finish(1);
    EXPECT_EQ(a.get_finishes().size(), 2);
    EXPECT_EQ(a.is_finish(1), true);
    EXPECT_EQ(a.is_finish(0), false);
    a.clear_finishes();
    EXPECT_EQ(a.get_finishes().size(), 0);
}