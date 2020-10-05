//
// Created by Евгений Тищенко on 04.10.2020.
//

#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <set>
#include <queue>

class Vertex {
private:
    std::map<char, std::set<int>> _connected_vertices;
    int _id;
public:
    typedef typename std::map<char, std::set<int>>::iterator iterator;
    typedef typename std::map<char, std::set<int>>::const_iterator const_iterator;

    Vertex();
    explicit Vertex(int val);
    Vertex(const Vertex& other);
    Vertex& operator=(const Vertex& other);

    void add_edge(char c, int to);
    std::vector<int> get_edge(char c) const;
    int get_id() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

class Automaton {
    std::string _sigma;
    int _start;
    size_t _edges;
    std::set<int> _terminals;
    std::map<int, Vertex> _graph;

    void _calc_buckets(std::vector<std::vector<int>>&);
    bool _dfs(int v, const std::string &s, int pos) const;
public:
    typedef typename std::map<int, Vertex>::iterator iterator;
    typedef typename std::map<int, Vertex>::const_iterator const_iterator;

    Automaton();
    Automaton(const std::string &sig, int _start);
    Automaton(const Automaton& other);

    Automaton& operator=(const Automaton& other);
    void add_Vertex(int v);
    void add_edge(int from, char c, int to);
    void add_terminal(int s);
    void set_sigma(const std::string &str);
    void set_start(int start);

    std::set<int> get_terminals() const;
    int get_start() const;
    std::string get_sigma() const;
    std::vector<int> get_next_Vertex(int from, char c) const;

    size_t v_size() const;
    size_t ed_size() const;
    bool is_terminal(int f) const;
    void clear_terminals();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    void make_deterministic();
    void make_full();
    void make_additional();
    void make_minimal();
    bool has_word(const std::string &s) const;
    bool is_same(Automaton other) const;
};

//input and output

std::ostream& operator<<(std::ostream &out, const Automaton &a);
std::ostream& operator<<(std::ostream &out, const Vertex& v);
std::istream& operator>>(std::istream &in, Automaton& a);

/*
 * sigma(starts from 1)
 * number of edges
 * all edges in format (from, char, to)
 *
 * start Vertex
 * number of finish vertices
 * all finish vertices
 */

#endif //AUTOMATON_H
