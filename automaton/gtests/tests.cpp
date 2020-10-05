#include <gtest/gtest.h>
#include "automaton.h"
#include <fstream>


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
}

TEST(AutomatonTest, AddingVertices) {
    Automaton a("1abc", 0);

    EXPECT_EQ(a.v_size(), 1);
    a.add_vertex(1);
    a.add_vertex(2);
    a.add_vertex(3);
    EXPECT_EQ(a.v_size(), 4);
    a.add_vertex(2);
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

    EXPECT_EQ(a.get_next_vertex(0, 'a').size(), 1);
    EXPECT_EQ(a.get_next_vertex(0, 'a')[0], 1);
    a.add_edge(0, 'a', 3);
    EXPECT_EQ(a.get_next_vertex(0, 'a').size(), 2);

    a.add_edge(0, 'a', 3);
    EXPECT_EQ(a.get_next_vertex(0, 'a').size(), 2);
    EXPECT_EQ(a.get_next_vertex(0, 'c').empty(), true);
}

TEST(AutomatonTest, AddingTerminal) {
    Automaton a("1abc", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(0, 'b', 2);
    a.add_edge(1, 'c', 2);
    a.add_edge(0, 'a', 3);

    a.add_terminal(1);
    a.add_terminal(2);

    EXPECT_EQ(a.get_terminals().size(), 2);
    a.add_terminal(1);
    EXPECT_EQ(a.get_terminals().size(), 2);
    EXPECT_EQ(a.is_terminal(1), true);
    EXPECT_EQ(a.is_terminal(0), false);
    a.clear_terminals();
    EXPECT_EQ(a.get_terminals().size(), 0);
}

TEST(AutomatonTest, DetectingWords) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(1, 'b', 0);
    a.add_edge(0, 'b', 2);
    a.add_edge(2, 'a', 0);
    a.add_edge(0, 'a', 3);
    a.add_edge(0, 'b', 3);

    a.add_terminal(0);
    a.add_terminal(3);

    EXPECT_EQ(a.has_word("bba"), false);
    EXPECT_EQ(a.has_word("aba"), true);
    EXPECT_EQ(a.has_word(""), true);
    EXPECT_EQ(a.has_word("ababababbaabbabaabb"), true);
    EXPECT_EQ(a.has_word("bbaab"), false);
    EXPECT_EQ(a.has_word("abababba"), true);
    EXPECT_EQ(a.has_word("b"), true);
    EXPECT_EQ(a.has_word("aaab"), false);
}

TEST(AutomatonTest, CheckDeterminisation) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(1, 'b', 0);
    a.add_edge(0, 'b', 2);
    a.add_edge(2, 'a', 0);
    a.add_edge(0, 'a', 3);
    a.add_edge(0, 'b', 3);

    a.add_terminal(0);
    a.add_terminal(3);

    a.make_deterministic();

    for (int v = 0; v < a.v_size(); ++v) {
        EXPECT_LE(a.get_next_vertex(v, 'a').size(), 1);
        EXPECT_LE(a.get_next_vertex(v, 'b').size(), 1);
    }

    EXPECT_EQ(a.has_word("bba"), false);
    EXPECT_EQ(a.has_word("aba"), true);
    EXPECT_EQ(a.has_word(""), true);
    EXPECT_EQ(a.has_word("ababababbaabbabaabb"), true);
    EXPECT_EQ(a.has_word("bbaab"), false);
    EXPECT_EQ(a.has_word("abababba"), true);
    EXPECT_EQ(a.has_word("b"), true);
    EXPECT_EQ(a.has_word("aaab"), false);
}

TEST(AutomatonTest, CheckCompleteAndAdditional) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(1, 'b', 2);
    a.add_edge(2, 'a', 3);
    a.add_edge(3, 'b', 2);
    a.add_edge(2, 'a', 4);
    a.add_edge(4, 'a', 4);
    a.add_edge(0, 'a', 4);
    a.add_edge(0, 'a', 5);
    a.add_edge(0, 'b', 5);
    a.add_edge(5, 'a', 6);
    a.add_edge(5, 'b', 6);
    a.add_edge(6, 'a', 5);
    a.add_edge(6, 'b', 5);

    a.add_terminal(0);
    a.add_terminal(2);
    a.add_terminal(4);
    a.add_terminal(6);

    a.make_deterministic();
    a.make_complete();
    a.make_complement();

    for (int v = 0; v < a.v_size(); ++v) {
        EXPECT_EQ(a.get_next_vertex(v, 'a').size(), 1);
        EXPECT_EQ(a.get_next_vertex(v, 'b').size(), 1);
    }

    EXPECT_EQ(a.get_terminals().size(), 1);

    EXPECT_EQ(a.has_word("abab"), false);
    EXPECT_EQ(a.has_word("abababaaaa"), false);
    EXPECT_EQ(a.has_word(""), false);
    EXPECT_EQ(a.has_word("baa"), true);
    EXPECT_EQ(a.has_word("abbba"), true);
    EXPECT_EQ(a.has_word("b"), true);
    EXPECT_EQ(a.has_word("bbbbbbbbbaaaaaaaaababbabab"), false);
}

TEST(AutomatonTest, CheckMinimalTest1) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 1);
    a.add_edge(0, 'b', 3);
    a.add_edge(1, 'a', 2);
    a.add_edge(1, 'b', 4);
    a.add_edge(2, 'a', 0);
    a.add_edge(2, 'b', 5);
    a.add_edge(3, 'a', 4);
    a.add_edge(3, 'b', 6);
    a.add_edge(4, 'a', 5);
    a.add_edge(4, 'b', 7);
    a.add_edge(5, 'a', 3);
    a.add_edge(5, 'b', 8);
    a.add_edge(6, 'a', 7);
    a.add_edge(6, 'b', 0);
    a.add_edge(7, 'a', 8);
    a.add_edge(7, 'b', 1);
    a.add_edge(8, 'a', 6);
    a.add_edge(8, 'b', 2);

    a.add_terminal(0);
    a.add_terminal(4);
    a.add_terminal(8);

    a.make_deterministic();
    a.make_complete();
    a.make_minimal();

    EXPECT_EQ(a.v_size(), 3);
    EXPECT_EQ(a.ed_size(), 6);

    EXPECT_EQ(a.get_terminals().size(), 1);

    EXPECT_EQ(a.has_word(""), true);
    EXPECT_EQ(a.has_word("aab"), false);
    EXPECT_EQ(a.has_word("ab"), true);
    EXPECT_EQ(a.has_word("bbabb"), true);
    EXPECT_EQ(a.has_word("bbaab"), false);
    EXPECT_EQ(a.has_word("abbabbabbabba"), true);
    EXPECT_EQ(a.has_word("aabb"), true);
}

TEST(AutomatonTest, CheckMinimalTest2) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 7);
    a.add_edge(0, 'b', 1);
    a.add_edge(1, 'b', 0);
    a.add_edge(1, 'a', 7);
    a.add_edge(7, 'b', 2);
    a.add_edge(7, 'a', 2);
    a.add_edge(2, 'b', 5);
    a.add_edge(5, 'a', 5);
    a.add_edge(5, 'b', 5);
    a.add_edge(2, 'a', 4);
    a.add_edge(4, 'a', 5);
    a.add_edge(3, 'a', 4);
    a.add_edge(3, 'b', 5);
    a.add_edge(4, 'b', 6);
    a.add_edge(6, 'a', 6);
    a.add_edge(6, 'b', 5);

    a.add_terminal(5);
    a.add_terminal(6);

    a.make_deterministic();
    a.make_complete();
    a.make_minimal();

    EXPECT_EQ(a.v_size(), 5);
    EXPECT_EQ(a.has_word("bbbaab"), true);
    EXPECT_EQ(a.has_word("ababaaaa"), true);
    EXPECT_EQ(a.has_word("baaa"), false);
}

TEST(AutomatonTest, CheckEqualutyTest1) {
    Automaton a("1ab", 0);
    a.add_edge(0, 'a', 7);
    a.add_edge(0, 'b', 1);
    a.add_edge(1, 'b', 0);
    a.add_edge(1, 'a', 7);
    a.add_edge(7, 'b', 2);
    a.add_edge(7, 'a', 2);
    a.add_edge(2, 'b', 5);
    a.add_edge(5, 'a', 5);
    a.add_edge(5, 'b', 5);
    a.add_edge(2, 'a', 4);
    a.add_edge(4, 'a', 5);
    a.add_edge(3, 'a', 4);
    a.add_edge(3, 'b', 5);
    a.add_edge(4, 'b', 6);
    a.add_edge(6, 'a', 6);
    a.add_edge(6, 'b', 5);

    a.add_terminal(5);
    a.add_terminal(6);

    Automaton b("1ab", 2);
    b.add_edge(2, 'b', 2);
    b.add_edge(2, 'a', 1);
    b.add_edge(1, 'a', 0);
    b.add_edge(1, 'b', 0);
    b.add_edge(0, 'b', 5);
    b.add_edge(0, 'a', 4);
    b.add_edge(4, 'a', 5);
    b.add_edge(4, 'b', 5);
    b.add_edge(5, 'a', 5);
    b.add_edge(5, 'b', 5);

    b.add_terminal(5);

    bool res = a.is_same(b);

    EXPECT_EQ(a.is_same(b), true);
}

TEST(AutomatonTest, CheckEqualutyTest2) {
    Automaton a("1ab", 0), b("1ab", 0);

    a.add_edge(0, 'a', 0);
    a.add_edge(0, 'b', 1);
    a.add_edge(1, 'a', 2);
    a.add_edge(1, 'b', 0);
    a.add_edge(2, 'a', 1);
    a.add_edge(2, 'b', 3);
    a.add_edge(3, 'a', 2);
    a.add_edge(3, 'b', 0);

    a.add_terminal(3);

    b.add_edge(0, 'a', 1);
    b.add_edge(0, 'b', 1);
    b.add_edge(1, 'a', 1);
    b.add_edge(1, 'b', 2);
    b.add_edge(2, 'a', 3);
    b.add_edge(2, 'b', 2);
    b.add_edge(3, 'a', 0);
    b.add_edge(3, 'b', 1);

    b.add_terminal(3);
    EXPECT_EQ(a.is_same(b), false);
}

