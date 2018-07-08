
#include <RoutingTable.hpp>
#include <iostream>

using namespace std;

/* ──────────────────────── keys structure functions ──────────────────────── */

/*
 * Equality operator for table's keys
 */
bool operator==(const Key &lhs, const Key &rhs) {
    return lhs.dest == rhs.dest && (* lhs.stack) == (* rhs.stack);
}

/*
 * Comparison operator for table's keys
 */
bool operator<(const Key &lhs, const Key &rhs) {
    return lhs.dest < rhs.dest;
}


/* ──────────────────────────────── hashing ───────────────────────────────── */

/*
 * Implementation of Murmur hash to hash the keys
 */
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


/* ────────────────────── routing table implementation ────────────────────── */
RoutingTable::RoutingTable() = default;


RoutingTable::~RoutingTable() {
    // delete every Key and every row
    for (auto &entry : table) {
        delete entry.second.stack;
    }
}

bool RoutingTable::contains(int dest, ProtocolStack * stack) {
    return table.find(Key {dest, stack}) != table.end();
}

bool RoutingTable::contains(int dest) {
    for (auto &pair : table)
        if (pair.first.dest == dest)
            return true;
    return false;
}

Row RoutingTable::get(int dest, ProtocolStack * stack) {
    auto iter = table.find(Key {dest, stack});
    return iter->second;
}

/*
 * Method: addRoute
 * ------------------
 * Arguments:
 *      - dest: destination of the route
 *      - stack: incoming stack
 *      - newt_hop: next node on the path
 *      - cost: cost of the path
 *      - function: function to apply to follow the route
 *
 * Returns: boolean
 *      - true if the route was either new or better than the precedent
 *      - false if a better route (cost-wise) is already known
 */
bool RoutingTable::addRoute(int dest, int next_hop, int cost,
                            AdaptationFunction function, ProtocolStack * stack) {
    /* ----------------------- case: the route exists ----------------------- */
    if (contains(dest, stack)) {
        Row row = table[Key {dest, stack}];
        if (row.cost > cost) {
            /* - the existing route is less efficient, we update the entry -- */
            row.function = function;
            row.next_hop = next_hop;
            row.cost = cost;
            return true;
        }
        /* ------- the existing route is equal or better, do nothing -------- */
        return false;
    }
    /* ---------- the route does not exist, add it to the entries ----------- */
    ProtocolStack * clonedStack = stack->clone();
    table[Key {dest, clonedStack}] = Row {dest, clonedStack, function, next_hop, cost};
    return true;
}

/*
 * Function: center
 * ----------------
 * Utilitary function to center a word within whitespaces.
 */
string center(const string &str, const int width) {
    unsigned long missing = width - str.length();
    if (missing <= 0) return str;
    string before = string(missing / 2, ' ');
    string after = string(missing / 2 + (missing % 2), ' ');
    return before + str + after;
}

/*
 * Method: toString
 * ----------------
 * Creates a string representing the routing table. This string is meant to be
 * as complete as possible.
 * Formats the output using the following rules:
 *      - Every row is printed, not ommiting any information
 *      - Rows are sorted by destination, then cost
 *      - Entries with different destinations are separated
 */
string RoutingTable::toString() {
    size_t n = table.size();
    string columns[5][n];
    int i = 0;

    
    int maxs[5] = {11, 5, 8, 8, 4};
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
    string seps[5] = {"   ", " | ", "   ", "   ", " "};
    string str;
    str += center("destination", maxs[0]) + seps[0];
    str += center("stack", maxs[1]) + seps[1];
    str += center("next hop", maxs[2]) + seps[2];
    str += center("function", maxs[3]) + seps[3];
    str += center("cost", maxs[4]) + seps[4];
    auto len = str.length();
    str += "\n" + string(len, '-') + "\n";

    // sorting rows
    int indexes[n];
    for (i= 0; i < (int) n; i++)
        indexes[i] = i;

    int a, b;
    for (i= 0; i < (int) n; i++)
        for (int j= (int) n - 1; j > i; j--) {
            a = indexes[j];
            b = indexes[j - 1];
            int l_dest = stoi(columns[0][a]), r_dest = stoi(columns[0][b]);
            if( (l_dest < r_dest) ||
                    ((l_dest == r_dest) && (stoi(columns[4][a]) < stoi(columns[4][b])))) {
                indexes[j - 1] = a;
                indexes[j] = b;
            }
        }

    for (unsigned int j = 0; j < n; j++) {
        for (unsigned int i = 0; i < 5; i++) {
            if (i == 4) str += "  ";
            str += center(columns[i][indexes[j]], maxs[i]) + seps[i];
        }
        str += "\n";
        if (j != n - 1 && stoi(columns[0][indexes[j]]) != stoi(columns[0][indexes[j+1]]))
            str += string(len, '~') + "\n";
    }
    return str;
}

