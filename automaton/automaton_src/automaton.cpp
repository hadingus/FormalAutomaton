//
// Created by Евгений Тищенко on 04.10.2020.
//


#include "automaton.h"

Vertex::Vertex() = default;
Vertex::Vertex(int val): _id(val) {}
Vertex::Vertex(const Vertex& other): _connected_vertices(other._connected_vertices),
                             _id(other._id) {}

Vertex& Vertex::operator=(const Vertex& other) {
    if (&other != this) {
        _connected_vertices.clear();
        _id = other._id;
        _connected_vertices = other._connected_vertices;
    }
    return *this;
}

void Vertex::delete_letter(char c) {
    if (_connected_vertices.find(c) != _connected_vertices.end()) {

        _connected_vertices.erase(c);
    }
}

void Vertex::add_edge(char c, int to) {
    _connected_vertices[c].insert(to);
}

void Vertex::delete_edge(char c, int to) {
    if (_connected_vertices[c].find(to) != _connected_vertices[c].end()) {
        _connected_vertices[c].erase(to);
    }
}

std::vector<int> Vertex::get_edge(char c) const {
    if (_connected_vertices.find(c) != _connected_vertices.end()) {
        std::vector<int> result;
        for (auto v : _connected_vertices.at(c)) {
            result.push_back(v);
        }
        return result;
    } else {
        return {};
    }
}

int Vertex::get_id() const {
    return _id;
}

Vertex::iterator Vertex::begin() {
    return _connected_vertices.begin();
}

Vertex::iterator Vertex::end() {
    return _connected_vertices.end();
}

Vertex::const_iterator Vertex::begin() const {
    return _connected_vertices.cbegin();
}

Vertex::const_iterator Vertex::end() const {
    return _connected_vertices.cend();
}

Automaton::Automaton(): _edges(0), _start(-1) {}
Automaton::Automaton(const std::string &sig, int _start): _edges(0), _alphabet(sig), _start(_start) {
    add_vertex(_start);
}

Automaton::Automaton(const Automaton& other): _start(other._start), _terminals(other._terminals),
                                   _graph(other._graph), _edges(other._edges), _alphabet(other._alphabet) {}

Automaton& Automaton::operator=(const Automaton& other) {
    if (this != &other) {
        _start = other._start;
        _terminals = other._terminals;
        _graph = other._graph;
        _edges = other._edges;
        _alphabet = other._alphabet;
    }
    return *this;
}

void Automaton::add_vertex(int v) {
    if (_graph.find(v) == _graph.end()) {
        _graph.emplace(v, Vertex(v));
    }
}

void Automaton::add_edge(int from, char c, int to) {
    add_vertex(from);
    add_vertex(to);
    _graph[from].add_edge(c, to);
    ++_edges;
}

void Automaton::delete_edge(int from, char c, int to) {
    add_vertex(from);
    _graph[from].delete_edge(c, to);
}

std::string Automaton::get_alphabet() const {
    return _alphabet;
}

size_t Automaton::v_size() const {
    return _graph.size();
}

size_t Automaton::ed_size() const {
    return _edges;
}

void Automaton::set_start(int start) {
    _start = start;
    add_vertex(_start);
}

int Vertex::get_next_count(char c) const {
    if (_connected_vertices.find(c) == _connected_vertices.end()) {
        return 0;
    }
    return _connected_vertices.at(c).size();
}

int Automaton::get_start() const {
    return _start;
}

void Automaton::clear_terminals() {
    _terminals.clear();
}

void Automaton::add_terminal(int s) {
    add_vertex(s);
    _terminals.insert(s);
}

std::vector<int> Automaton::get_next_vertex(int from, char c) const {
    if (_graph.find(from) == _graph.end()) {
        return {};
    }
    return _graph.at(from).get_edge(c);
}

bool Automaton::is_terminal(int f) const {
    return _terminals.find(f) != _terminals.end();
}

std::set<int> Automaton::get_terminals() const {
    return _terminals;
}

Automaton::iterator Automaton::begin() {
    return _graph.begin();
}

Automaton::iterator Automaton::end() {
    return _graph.end();
}

Automaton::const_iterator Automaton::begin() const {
    return _graph.cbegin();
}

Automaton::const_iterator Automaton::end() const {
    return _graph.cend();
}

bool Automaton::_dfs(int v, const std::string &s, int pos) const {
    if (pos == s.size()) {
        return is_terminal(v);
    }
    auto next_verts = get_next_vertex(v, s[pos]);
    bool good;
    for (auto u : next_verts) {
        good = _dfs(u, s, pos + 1);
        if (good) {
            return true;
        }
    }
    return false;
}

void Automaton::_eps_dfs(int v, std::vector<int> &result, std::map<int, bool> &used) {
    used[v] = true;

    auto cur = get_next_vertex(v, '1');
    for (int u : cur) {
        if (used.find(u) == used.end()) {
            result.push_back(u);
            _eps_dfs(u, result, used);
        }
    }
}

void Automaton::_reformat_dfs(int v, std::map<int, bool> &used) const {
    used[v] = true;
    for (auto symbol: _alphabet) {
        for (int next_vertex : get_next_vertex(v, symbol)) {
            if (used.find(next_vertex) == used.end()) {
                _reformat_dfs(next_vertex, used);
            }
        }
    }
}

void Automaton::_reformat() {
    std::map<int, bool> used;
    _reformat_dfs(_start, used);
    for (int v = 0; v < v_size(); ++v) {
        if (!used[v] && _graph.find(v) != _graph.end()) {
            int deleted_cnt = 0;
            for (char c : _alphabet) {
                deleted_cnt += _graph[v].get_next_count(c);
            }
            _edges -= deleted_cnt;
            _graph.erase(v);
        }
    }
}

bool Automaton::has_word(const std::string &s) const {
    return _dfs(_start, s, 0);
}

void Automaton::make_deterministic() {
    std::queue<std::vector<int>> vertex_queue;
    std::map<std::vector<int>, int> new_id;
    int id = 0;
    std::map<std::vector<int>, bool> used;

    Automaton result(get_alphabet(), 0);

    std::vector<int> base_vertex = {_start};
    vertex_queue.push(base_vertex);
    new_id[base_vertex] = id;
    used[base_vertex] = true;
    id++;

    while (!vertex_queue.empty()) {
        std::vector<int> current_vertex = vertex_queue.front(); //all these vertices will be the one
        vertex_queue.pop();
        int cur_id = new_id[current_vertex];

        for (auto u : current_vertex) {
            if (is_terminal(u)) {
                result.add_terminal(cur_id);
                break;
            }
        }

        for (auto c : get_alphabet()) {
            std::set<int> nxt_set;
            for (auto v : current_vertex) {
                auto next_verts = get_next_vertex(v, c);
                for (auto it : next_verts) {
                    nxt_set.insert(it);
                }
            }
            std::vector<int> next_vert;

            next_vert.reserve(nxt_set.size());
            for (auto it : nxt_set) {
                next_vert.push_back(it);
            }

            if (!next_vert.empty()) {
                if (used.find(next_vert) == used.end()) {
                    vertex_queue.push(next_vert);
                    used[next_vert] = true;
                    new_id[next_vert] = id++;
                }

                int next_id = new_id[next_vert];
                result.add_edge(cur_id, c, next_id);
            }
        }
    }
    *this = result;
}

void Automaton::make_complete() {
    make_deterministic();
    int sz = v_size();
    int new_edges = 0;
    bool connection_got = false;
    add_vertex(sz);
    for (const auto& it : _graph) {
        int v = it.first;
        for (int i = 1; i < _alphabet.size(); ++i) {
            char c = _alphabet[i];
            auto connected_vertices = get_next_vertex(v, c);
            if (connected_vertices.empty()) {
                add_edge(v, c, sz);
                ++new_edges;
                if (v != sz) {
                    connection_got = true;
                }
            }
        }
    }
    if (!connection_got) {
        _graph.erase(sz);
        _edges -= new_edges;
    }
}

void Automaton::make_complement() {
    make_complete();

    std::vector<bool> new_terminal(v_size(), true);
    for (int v: _terminals) {
        new_terminal[v] = false;
    }
    clear_terminals();

    for (int v = 0; v < v_size(); ++v) {
        if (new_terminal[v]) {
            add_terminal(v);
        }
    }
}

void Automaton::_calc_buckets(std::vector<std::vector<int>> &t) {
    for (int i = 1; i < _alphabet.size(); ++i) {
        char c = _alphabet[i];
        for (int v = 0; v < v_size(); ++v) {
            t[v][i] = t[get_next_vertex(v, c)[0]][0];
        }
    }
}

void Automaton::delete_eps() {
    std::vector<std::vector<int>> new_connections(v_size());
    for (int v = 0; v < v_size(); ++v) {
        std::map<int, bool> used;
        _eps_dfs(v, new_connections[v], used);
    }

    for (int v = 0; v < v_size(); ++v) {

        for (int u : new_connections[v]) {
            for (int i = 1; i < _alphabet.size(); ++i) {
                char c = _alphabet[i];
                auto new_vrts = get_next_vertex(u, c);
                for (int w : new_vrts) {
                    add_edge(v, c, w);
                }
            }
            if (is_terminal(u)) {
                add_terminal(v);
            }
        }
        int deleted_edges = _graph[v].get_next_count('1');
        _graph[v].delete_letter('1');
        _edges -= deleted_edges;
    }

    _reformat();
}

void Automaton::make_minimal() {
    make_complete();
    std::vector<std::vector<int>> prev_bucket(v_size(), std::vector<int>(_alphabet.size(), 1));
    for (int v : _terminals) {
        prev_bucket[v][0] = 0;
    }
    _calc_buckets(prev_bucket);
    int id;
    for (int iter = 0; iter < v_size(); ++iter) {
        id = 0;
        std::vector<std::vector<int>> next_bucket(v_size(), std::vector<int>(_alphabet.size(), -1));
        for (int v = 0; v < v_size(); ++v) {
            if (next_bucket[v][0] == -1) {
                auto temp = prev_bucket[v];
                next_bucket[v][0] = id;
                for (int u = v + 1; u < v_size(); ++u) {
                    if (prev_bucket[u] == temp) {
                        next_bucket[u][0] = id;
                    }
                }
                ++id;
            }
        }
        std::swap(prev_bucket, next_bucket);
        _calc_buckets(prev_bucket);
    }

    Automaton min_auto(_alphabet, prev_bucket[0][0]);
    std::vector<bool> used(id, false);
    for (int v = 0; v < v_size(); ++v) {
        if (!(used[prev_bucket[v][0]])) {
            used[prev_bucket[v][0]] = true;
            for (int i = 1; i < _alphabet.size(); ++i) {
                char c = _alphabet[i];
                min_auto.add_edge(prev_bucket[v][0], c, prev_bucket[v][i]);
            }
            if (is_terminal(v)) {
                min_auto.add_terminal(prev_bucket[v][0]);
            }
        }
    }
    (*this) = min_auto;
}

//input and output

std::ostream& operator<<(std::ostream &out, const Automaton &a) {
    out << "vertices: " << a.v_size() << "\nedges: "<< a.ed_size() << std::endl;
    bool change_str = false;
    for (const auto &it : a) {
        if (change_str) {
            out << "\n";
        }
        change_str = true;
        out << it.second;
    }

    out << "\nstart: " << a.get_start() << "\n";
    auto f = a.get_terminals();
    out << "terminals: ";
    for (auto i : f) {
        out << i << " ";
    }
    return out;
}

std::istream& operator>>(std::istream &in, Automaton& a) {
    in >> a._alphabet;
    int edges_count, terminals_count;
    in >> edges_count;
    int from, to;
    char c;
    while (edges_count--) {
        in >> from >> c >> to;
        a.add_edge(from, c, to);
    }
    in >> a._start;
    in >> terminals_count;
    a.clear_terminals();
    while (terminals_count--) {
        in >> to;
        a.add_terminal(to);
    }

    return in;
}

std::ostream& operator<<(std::ostream &out, const Vertex& v) {
    bool change_str = false;
    for (const auto& it : v) {
        if (change_str) {
            out << "\n";
        }
        change_str = true;
        int pos = 0;
        for (auto u : it.second) {
            out << v.get_id() << "----" << it.first << "---->" << u;
            if (pos + 1 < it.second.size()) {
                out << "\n";
            }
            ++pos;
        }
    }
    return out;
}

bool Automaton::is_same(Automaton other) const {
    struct vertex_pair {
        int vertex1, vertex2;
        vertex_pair(int v1, int v2): vertex1(v1), vertex2(v2) {}
        vertex_pair() = default;

        bool operator<(const vertex_pair& other) const {
            if (vertex1 != other.vertex1) {
                return vertex1 < other.vertex1;
            }
            return vertex2 < other.vertex2;
        }
    };

    Automaton a = (*this);
    a.make_minimal();

    other.make_minimal();

    std::map<vertex_pair, bool> used;
    std::queue<vertex_pair> q;
    q.push({a.get_start(), other.get_start()});
    used[{a.get_start(), other.get_start()}] = true;

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();

        int u = cur.vertex1, v = cur.vertex2;
        if (a.is_terminal(u) ^ other.is_terminal(v)) {
            return false;
        }
        for (int i = 1; i < _alphabet.size(); ++i) {
            char c = _alphabet[i];
            int u1 = a.get_next_vertex(u, c)[0];
            int v1 = other.get_next_vertex(v, c)[0];
            if (used.find({u1, v1}) == used.end()) {
                used[{u1, v1}] = true;
                q.push(vertex_pair(u1, v1));
            }
        }
    }
    return true;
}
