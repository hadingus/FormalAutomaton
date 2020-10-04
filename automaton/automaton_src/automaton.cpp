//
// Created by Евгений Тищенко on 04.10.2020.
//

#include "automaton.h"

Vertex::Vertex() = default;
Vertex::Vertex(int val): _id(val) {}
Vertex::Vertex(const Vertex& other): _connected_vertices(other._connected_vertices),
                             _id(other._id) {}

Vertex& Vertex::operator=(const Vertex& other) {
    _connected_vertices.clear();
    _id = other._id;
    _connected_vertices = other._connected_vertices;
    return *this;
}

void Vertex::add_edge(char c, int to) {
    _connected_vertices[c].insert(to);
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
Automaton::Automaton(const std::string &sig, int _start): _edges(0), _sigma(sig), _start(_start) {
    add_Vertex(_start);
}

Automaton::Automaton(const Automaton& other): _start(other._start), _finishes(other._finishes),
                                   _graph(other._graph), _edges(other._edges), _sigma(other._sigma) {}

Automaton& Automaton::operator=(const Automaton& other) {
    if (this != &other) {
        _edges  = other._edges;
        _finishes = other._finishes;
        _graph = other._graph;
        _edges = other._edges;
        _sigma = other._sigma;
    }
    return *this;
}

void Automaton::add_Vertex(int v) {
    if (_graph.find(v) == _graph.end()) {
        _graph[v] = Vertex(v);
    }
}

void Automaton::add_edge(int from, char c, int to) {
    add_Vertex(from);
    add_Vertex(to);
    _graph[from].add_edge(c, to);
    ++_edges;
}

void Automaton::set_sigma(const std::string &str) {
    _sigma = str;
}

std::string Automaton::get_sigma() const {
    return _sigma;
}

size_t Automaton::v_size() const {
    return _graph.size();
}

size_t Automaton::ed_size() const {
    return _edges;
}

void Automaton::set_start(int start) {
    _start = start;
    add_Vertex(_start);
}

int Automaton::get_start() const {
    return _start;
}

void Automaton::clear_finishes() {
    _finishes.clear();
}

void Automaton::add_finish(int s) {
    add_Vertex(s);
    _finishes.insert(s);
}

std::vector<int> Automaton::get_next_Vertex(int from, char c) const {
    return _graph.at(from).get_edge(c);
}

bool Automaton::is_finish(int f) const {
    return _finishes.find(f) != _finishes.end();
}

std::set<int> Automaton::get_finishes() const {
    return _finishes;
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
        return is_finish(v);
    }
    auto next_verts = get_next_Vertex(v, s[pos]);
    bool good;
    for (auto u : next_verts) {
        good = _dfs(u, s, pos + 1);
        if (good) {
            return true;
        }
    }
    return false;
}

bool Automaton::has_word(const std::string &s) const {
    return _dfs(_start, s, 0);
}

void Automaton::make_deterministic() {
    std::queue<std::vector<int>> q;
    std::map<std::vector<int>, int> new_id;
    int id = 0;
    std::map<std::vector<int>, bool> used;

    Automaton result(get_sigma(), 0);

    q.push({_start});
    new_id[{_start}] = id++;
    used[{_start}] = true;

    while (!q.empty()) {
        std::vector<int> vert_cort = q.front();
        q.pop();
        int cur_id = new_id[vert_cort];


        for (auto u : vert_cort) {
            if (is_finish(u)) {
                result.add_finish(cur_id);
                break;
            }
        }

        for (auto c : get_sigma()) {
            std::set<int> nxt_set;
            for (auto v : vert_cort) {
                auto next_verts = get_next_Vertex(v, c);
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
                    q.push(next_vert);
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

void Automaton::make_full() {
    int sz = v_size();
    int new_edges = 0;
    bool connection_got = false;
    add_Vertex(sz);
    for (const auto& it : _graph) {
        int v = it.first;
        for (int i = 1; i < _sigma.size(); ++i) {
            char c = _sigma[i];
            auto connected_vertices = get_next_Vertex(v, c);
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

void Automaton::make_additional() {
    make_deterministic();
    make_full();

    std::vector<bool> new_finish(v_size(), 1);
    for (int v: _finishes) {
        new_finish[v] = 0;
    }
    clear_finishes();

    for (int v = 0; v < v_size(); ++v) {
        if (new_finish[v]) {
            add_finish(v);
        }
    }
}

void Automaton::_calc_buckets(std::vector<std::vector<int>> &t) {
    for (int i = 1; i < _sigma.size(); ++i) {
        char c = _sigma[i];
        for (int v = 0; v < v_size(); ++v) {
            t[v][i] = t[get_next_Vertex(v, c)[0]][0];
        }
    }
}

void Automaton::make_minimal() {
    std::vector<std::vector<int>> prev_bucket(v_size(), std::vector<int>(_sigma.size(), 1));
    for (int v : _finishes) {
        prev_bucket[v][0] = 0;
    }
    _calc_buckets(prev_bucket);
    int id;
    for (int iter = 0; iter < v_size(); ++iter) {
        id = 0;
        std::vector<std::vector<int>> next_bucket(v_size(), std::vector<int>(_sigma.size(), -1));
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

    Automaton min_auto(_sigma, prev_bucket[0][0]);
    std::vector<bool> used(id, false);
    for (int v = 0; v < v_size(); ++v) {
        if (!(used[prev_bucket[v][0]])) {
            used[prev_bucket[v][0]] = true;
            for (int i = 1; i < _sigma.size(); ++i) {
                char c = _sigma[i];
                min_auto.add_edge(prev_bucket[v][0], c, prev_bucket[v][i]);
            }
            if (is_finish(v)) {
                min_auto.add_finish(prev_bucket[v][0]);
            }
        }
    }
    (*this) = min_auto;
}

//input and output

std::ostream& operator<<(std::ostream &out, const Automaton &a) {
    out << "vertices: " << a.v_size() << ", edges: "<< a.ed_size() << std::endl;
    bool change_str = false;
    for (const auto &it : a) {
        if (change_str) {
            out << "\n";
        }
        change_str = true;
        out << it.second;
    }

    out << "\nstart: " << a.get_start() << "\n";
    auto f = a.get_finishes();
    out << "finishes: ";
    for (auto i : f) {
        out << i << " ";
    }
    return out;
}

std::istream& operator>>(std::istream &in, Automaton& a) {
    std::string S;
    in >> S;
    a.set_sigma(S);
    size_t n;
    in >> n;
    int from, to;
    char c;
    while (n--) {
        in >> from >> c >> to;
        a.add_edge(from, c, to);
    }
    in >> to;
    a.set_start(to);
    in >> n;
    a.clear_finishes();
    while (n--) {
        in >> to;
        a.add_finish(to);
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
    Automaton a = (*this);
    a.make_deterministic();
    a.make_full();
    a.make_minimal();

    other.make_deterministic();
    other.make_full();
    other.make_minimal();
    other.make_additional();

    std::map<std::pair<int, int>, bool> used;
    std::queue<std::pair<int, int>> q;
    q.push({a.get_start(), other.get_start()});
    used[{a.get_start(), other.get_start()}] = true;

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();

        int u = cur.first, v = cur.second;
        if (a.is_finish(u) && other.is_finish(v)) {
            return false;
        }
        for (int i = 1; i < _sigma.size(); ++i) {
            char c = _sigma[i];
            int u1 = a.get_next_Vertex(u, c)[0];
            int v1 = other.get_next_Vertex(v, c)[0];
            if (used.find({u1, v1}) == used.end()) {
                used[{u1, v1}] = true;
                q.push({u1, v1});
            }
        }
    }
    return true;
}
