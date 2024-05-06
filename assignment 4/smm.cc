#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

class illegal_exception {
public:
    illegal_exception(std::string msg) {
        message = msg;
    }
    std::string what() {
        return message;
    }

private:
    std::string message;
};

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;

  std::unordered_set<char> alphabet_set;
  std::unordered_set<std::string> states_set;
  std::string start_state;
  std::unordered_set<std::string> accepting_set;
  std::unordered_map<std::string, std::unordered_map<char, std::string>> transition_function;

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        //// Variable 's[0]' is an alphabet symbol
        alphabet_set.insert(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          //// Variable 'c' is an alphabet symbol
          alphabet_set.insert(c);
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      if (initial) {
        //// The state is initial
        start_state = s;
        initial = false;
      }
      if (accepting) {
        //// The state is accepting
        accepting_set.insert(s);
      }
      states_set.insert(s);
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      std::vector<std::string> lineVec;
      while(line >> s) {
        lineVec.push_back(s);
      }
      fromState = lineVec.front();
      toState = lineVec.back();
      for(int i = 1; i < lineVec.size()-1; ++i) {
        std::string s = lineVec[i];
        if (isChar(s)) {
          symbols += s;
        } else if (isRange(s)) {
          for(char c = s[0]; c <= s[2]; ++c) {
            symbols += c;
          }
        }
      }
      for ( char c : symbols ) {
        //// There is a transition from 'fromState' to 'toState' on 'c'
        transition_function[fromState][c] = toState;
      }
    }
  }


  // Input section (already skipped header)
  try {
    while(in >> s) {
      //// Variable 's' contains an input string for the DFA
      std::string curr_str = "";
      std::string curr_state = start_state;
      bool first = true;    // for when start state is accepting
      
      while (s.length() > 0) {
        if (s.length() >= 6) {
          if (s.substr(0, 6) == EMPTY) {
            if (accepting_set.count(curr_state) > 0) {
              std::cout << curr_str << std::endl;
              curr_str = "";
              curr_state = start_state;
              first = true;
            }
            else {
              throw illegal_exception("ERROR");
            }

            if (accepting_set.count(start_state) > 0) {
              std::cout << EMPTY << std::endl;
              s = s.substr(6, s.length()-6);
              continue;
            }
            else {
              throw illegal_exception("ERROR");
            }
          }
        }

        if (transition_function[curr_state].count(s[0]) < 1) {
          if (!first && accepting_set.count(curr_state) > 0) {
            std::cout << curr_str << std::endl;
            curr_str = "";
            curr_state = start_state;
            first = true;
          }
          else {
            throw illegal_exception("ERROR");
          }
        }
        else {
          curr_state = transition_function[curr_state][s[0]];
          curr_str += s[0];
          s = s.substr(1, s.length()-1);
          first = false;
        }
      }

      if (accepting_set.count(curr_state) > 0) {
        std::cout << curr_str << std::endl;
      }
      else {
        throw illegal_exception("ERROR");
      }
    }
  }
  catch (illegal_exception ie) {
    std::cerr << ie.what() << std::endl;
    return 1;
  }

  return 0;
}