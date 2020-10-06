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
#include <string>

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

    void delete_letter(char c);
    void add_edge(char c, int to);
    void delete_edge(char c, int to);
    std::vector<int> get_edge(char c) const;
    int get_id() const;
    int get_next_count(char c) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    friend std::ostream& operator<<(std::ostream &out, const Vertex& v);
};

class Automaton {
    std::string _sigma;
    int _start;
    size_t _edges;
    std::set<int> _terminals;
    std::map<int, Vertex> _graph;

    void _calc_buckets(std::vector<std::vector<int>>&);
    bool _dfs(int v, const std::string &s, int pos) const;
    void _reformat_dfs(int v, std::map<int, bool> &used) const;
    void _reformat();
    void _eps_dfs(int v, std::vector<int> &result, std::map<int, bool> &used);
public:
    typedef typename std::map<int, Vertex>::iterator iterator;
    typedef typename std::map<int, Vertex>::const_iterator const_iterator;

    Automaton();
    Automaton(const std::string &sig, int start);
    Automaton(const Automaton& other);

    Automaton& operator=(const Automaton& other);
    void add_vertex(int v);
    void add_edge(int from, char c, int to);
    void add_terminal(int s);
    void set_start(int start);

    std::set<int> get_terminals() const;
    int get_start() const;
    std::string get_sigma() const;
    std::vector<int> get_next_vertex(int from, char c) const;

    size_t v_size() const;
    size_t ed_size() const;
    bool is_terminal(int f) const;
    void clear_terminals();

    void delete_edge(int from, char c, int to);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    void delete_eps();
    void make_deterministic();
    void make_complete();
    void make_complement();
    void make_minimal();
    bool has_word(const std::string &s) const;
    bool is_same(Automaton other) const;

    friend std::ostream& operator<<(std::ostream &out, const Automaton &a);
    friend std::istream& operator>>(std::istream &in, Automaton& a);
};

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
