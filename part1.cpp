#include <iostream>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

// DFA structure
struct DFA {
    set<string> states;
    set<char> alphabet;
    string start_state;
    set<string> accept_states;
    map<pair<string, char>, string> transitions;

    DFA(set<string> st, set<char> alph, string start, set<string> acc, map<pair<string, char>, string> trans)
        : states(st), alphabet(alph), start_state(start), accept_states(acc), transitions(trans) {}
};

// Function to parse a DFA from a string representation
DFA parse_dfa(const string& dfa_str) {
    istringstream ss(dfa_str);
    string line;
    set<string> states;
    set<char> alphabet;
    string start_state;
    set<string> accept_states;
    map<pair<string, char>, string> transitions;

    // Read states
    getline(ss, line);
    istringstream state_stream(line);
    string state;
    while (state_stream >> state) states.insert(state);

    // Read alphabet
    getline(ss, line);
    for (char c : line) if (c != ' ') alphabet.insert(c);

    // Read start state
    getline(ss, start_state);

    // Read accept states
    getline(ss, line);
    istringstream accept_stream(line);
    while (accept_stream >> state) accept_states.insert(state);

    // Read transitions
    while (getline(ss, line)) {
        istringstream trans_stream(line);
        string from, to;
        char symbol;
        trans_stream >> from >> symbol >> to;
        transitions[{from, symbol}] = to;
    }

    return DFA(states, alphabet, start_state, accept_states, transitions);
}

// Function to build the intersection DFA of two DFAs
DFA intersection_dfa(const DFA& dfa1, const DFA& dfa2) {
    set<string> new_states;
    set<string> accepting_states;
    map<pair<string, char>, string> new_transitions;

    // Create the Cartesian product of the states of the two DFAs
    for (const string& state1 : dfa1.states) {
        for (const string& state2 : dfa2.states) {
            string new_state = state1 + "," + state2;
            new_states.insert(new_state);

            // If both states are accepting, the new state is accepting
            if (dfa1.accept_states.count(state1) && dfa2.accept_states.count(state2)) {
                accepting_states.insert(new_state);
            }

            // Define transitions for the new state
            for (const char& symbol : dfa1.alphabet) {
                string next_state1 = dfa1.transitions.at({state1, symbol});
                string next_state2 = dfa2.transitions.at({state2, symbol});
                new_transitions[{new_state, symbol}] = next_state1 + "," + next_state2;
            }
        }
    }

    return DFA(new_states, dfa1.alphabet, dfa1.start_state + "," + dfa2.start_state, accepting_states, new_transitions);
}

// Function to check if a DFA accepts the empty language
bool is_empty_dfa(const DFA& dfa) {
    queue<string> q;
    set<string> visited;

    q.push(dfa.start_state);
    visited.insert(dfa.start_state);

    while (!q.empty()) {
        string current_state = q.front();
        q.pop();

        // If it's an accepting state, the DFA is not empty
        if (dfa.accept_states.count(current_state)) {
            return false;
        }

        // Add all unvisited neighboring states to the queue
        for (const char& symbol : dfa.alphabet) {
            string next_state = dfa.transitions.at({current_state, symbol});
            if (visited.count(next_state) == 0) {
                visited.insert(next_state);
                q.push(next_state);
            }
        }
    }

    return true; // No accepting state is reachable, so the DFA accepts the empty language
}

// Function to check if the two DFAs are consistent (i.e., their intersection is non-empty)
bool check_consistency(const string& w0, const string& w1) {
    DFA dfa1 = parse_dfa(w0);
    DFA dfa2 = parse_dfa(w1);

    DFA intersection = intersection_dfa(dfa1, dfa2);

    // If the intersection DFA accepts no string, then the properties are inconsistent (false)
    return !is_empty_dfa(intersection);
}

int main() {
    // Test input w0 and w1 representing two DFAs
    // string w0 = "qstart q0 q1\n0 1\nqstart\nq0\nqstart 0 q0\nqstart 1 q1\nq0 0 q0\nq0 1 q0\nq1 0 q1\nq1 1 q1";
    // string w1 = "rstart r0 r1\n0 1\nrstart\nr1\nrstart 0 r0\nrstart 1 r1\nr0 0 r0\nr0 1 r0\nr1 0 r1\nr1 1 r1";
    // bool result1 = check_consistency(w0, w1);
    // cout << (result1 ? "True" : "False") << endl;

    string x0 = "A B\n0 1\nA\nB\nA 0 B\nA 1 B\nB 0 A\nB 1 B";
    string x1 = "X Y\n0 1\nX\nY\nX 0 Y\nX 1 Y\nY 0 Y\nY 1 Y";
    bool result2 = check_consistency(x0, x1);
    cout << (result2 ? "True" : "False") << endl;

    return 0;
}