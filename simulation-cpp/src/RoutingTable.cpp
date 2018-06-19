
#include <RoutingTable.hpp>

using namespace std;

struct Row {
    int dest;
    ProtocolStack * stack;
    AdaptationFunction function;
    int next_hop;
    int cost;
};

struct Key {
    int dest;
    ProtocolStack * stack;
};

bool operator==(const Key &lhs, const Key &rhs) {
    return lhs.dest == rhs.dest && lhs.stack == rhs.stack;
}

bool operator<(const Key &lhs, const Key &rhs) {
    return lhs.dest < rhs.dest;
}

unsigned int MurmurHash2(const void *key, int len, unsigned int seed) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^len;

    // Mix 4 bytes at a time into the hash

    const auto *data = (const unsigned char *) key;

    while (len >= 4) {
        auto k = *(unsigned int *) data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

namespace std {
    template<> struct hash<Key> {
        size_t operator()(const Key &k) const noexcept {
            size_t stackHash = MurmurHash2(k.stack->protocols, k.stack->topIndex + 1, 324157103);
            stackHash &= k.dest;
            return stackHash;
        }
    };
}

RoutingTable::RoutingTable() = default;


RoutingTable::~RoutingTable() {
    // delete every Key and every row
    for (auto &entry : table) {
        delete entry.second.stack;
    }
}

bool RoutingTable::contains(int dest, ProtocolStack * stack) {
    auto iter = table.find(Key {dest, stack});
    return iter != table.end();
}

Row RoutingTable::get(int dest, ProtocolStack * stack) {
    auto iter = table.find(Key {dest, stack});
    return iter->second;
}

bool RoutingTable::addRoute(int dest, int next_hop, int cost,
                            AdaptationFunction function, ProtocolStack * stack) {
    if (contains(dest, stack))
        return false;
    ProtocolStack * clonedStack = stack->clone();
    table[Key {dest, clonedStack}] = Row {dest, clonedStack, function, next_hop, cost};
}

string center(const string &s, const int width) {
    return string((width - s.length()) / 2, ' ') + s
           + string((width - s.length()) / 2 + ((s.length() - width) % 2) == 1 ? 1 : 0 , ' ');
}

string RoutingTable::toString() {
    unsigned long n = table.size();
    string columns[5][n];
    int i = 0;
    int maxs[5] = {11, 0, 0, 0, 0};
    for (auto &entry : table) {
        columns[0][i] = to_string(entry.second.dest);
        columns[1][i] = entry.second.stack->toString();
        columns[2][i] = to_string(entry.second.next_hop);
        columns[3][i] = entry.second.function.toString();
        columns[4][i] = to_string(entry.second.cost);
        for (int k = 0; k < 5; k++)
            maxs[k] = std::max(maxs[k], static_cast<const int &>(columns[k][i].length()));
        i++;
    }
    string seps[5] = {"   ", " | ", "   ", "   ", ""};
    string str;
    str += center("destination", maxs[0]);
    str += seps[0];
    str += center("stack", maxs[1]);
    str += seps[1];
    str += center("next hop", maxs[2]);
    str += seps[2];
    str += center("function", maxs[3]);
    str += seps[3];
    str += center("cost", maxs[4]);
    str += seps[4];
    auto len = str.length();
    str += "\n";
    str += string(len, '-');
    str += "\n";

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < n; j++) {
            str += center(columns[i][j], maxs[i]);
            str += seps[i];
        }
        str += "\n";
    }
}

