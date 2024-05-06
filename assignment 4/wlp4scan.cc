#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

void initialize_DFA(std::unordered_set<char> &alphabet_set, std::unordered_set<std::string> &states_set, std::string &start_state, std::unordered_set<std::string> &accepting_set, std::unordered_map<std::string, std::unordered_map<char, std::string>> &transition_function) {
  std::ifstream in("wlp4scan.dfa");
  std::string s;

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
}

std::string kindify(std::string str) {
  if (str == "0" || str == "num") {
    return "NUM";
  }
  else if (str == "id" || str == "r" || str == "re" || str == "ret" || str == "retu" || str == "retur" || str == "i" || str == "in" || str == "i" || str == "in" || str == "e" || str == "el" || str == "els" || str == "w" || str == "wh" || str == "whi" || str == "whil" || str == "wa" || str == "wai" || str == "p" || str == "pr" || str == "pri" || str == "prin" || str == "print" || str == "printl" || str == "n" || str == "ne" || str == "d" || str == "de" || str == "del" || str == "dele" || str == "delet" || str == "N" || str == "NU" || str == "NUL") {
    return "ID";
  }
  else if (str == "(") {
    return "LPAREN";
  }
  else if (str == ")") {
    return "RPAREN";
  }
  else if (str == "{") {
    return "LBRACE";
  }
  else if (str == "}") {
    return "RBRACE";
  }
  else if (str == "[") {
    return "LBRACK";
  }
  else if (str == "]") {
    return "RBRACK";
  }
  else if (str == "=") {
    return "BECOMES";
  }
  else if (str == "==") {
    return "EQ";
  }
  else if (str == "!=") {
    return "NE";
  }
  else if (str == "<") {
    return "LT";
  }
  else if (str == ">") {
    return "GT";
  }
  else if (str == "<=") {
    return "LE";
  }
  else if (str == ">=") {
    return "GE";
  }
  else if (str == "+") {
    return "PLUS";
  }
  else if (str == "-") {
    return "MINUS";
  }
  else if (str == "*") {
    return "STAR";
  }
  else if (str == "/") {
    return "SLASH";
  }
  else if (str == "%") {
    return "PCT";
  }
  else if (str == ",") {
    return "COMMA";
  }
  else if (str == ";") {
    return "SEMI";
  }
  else if (str == "&") {
    return "AMP";
  }
  else if (str == "return") {
    return "RETURN";
  }
  else if (str == "if") {
    return "IF";
  }
  else if (str == "else") {
    return "ELSE";
  }
  else if (str == "while") {
    return "WHILE";
  }
  else if (str == "println") {
    return "PRINTLN";
  }
  else if (str == "wain") {
    return "WAIN";
  }
  else if (str == "int") {
    return "INT";
  }
  else if (str == "new") {
    return "NEW";
  }
  else if (str == "delete") {
    return "DELETE";
  }
  else if (str == "NULL") {
    return "NULL";
  }
  else {
    return "NOT DEFINED";
  }
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string str;

  std::unordered_set<char> alphabet_set;
  std::unordered_set<std::string> states_set;
  std::string start_state;
  std::unordered_set<std::string> accepting_set;
  std::unordered_map<std::string, std::unordered_map<char, std::string>> transition_function;

  initialize_DFA(alphabet_set, states_set, start_state, accepting_set, transition_function);

  try {
    while (std::getline(in, str)) {
      std::istringstream line(str);
      std::vector<std::string> lines;
      while (line >> str) {
        lines.push_back(str);
      }

      for (int i=0;i<lines.size();i++) {
        std::string s = lines[i];

        if (s.length() >= 2) {
          if (s.substr(0, 2) == "//") {
            break;
          }
        }

        //// Variable 's' contains an input string for the DFA
        std::string curr_str = "";
        std::string curr_state = start_state;
        bool first = true;    // for when start state is accepting
        
        while (s.length() > 0) {
          if (transition_function[curr_state].count(s[0]) < 1) {
            if (!first && accepting_set.count(curr_state) > 0) {
              if (kindify(curr_state) == "NUM") {
                if (std::stoi(curr_str) > 2147483647) {
                  throw 1;
                }
              }
              std::cout << kindify(curr_state) << " " << curr_str << std::endl;
              curr_str = "";
              curr_state = start_state;
              first = true;
            }
            else {
              throw 1;
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
          if (kindify(curr_state) == "NUM") {
            if (std::stoi(curr_str) > 2147483647) {
              throw 1;
            }
          }
          std::cout << kindify(curr_state) << " " << curr_str << std::endl;
        }
        else {
          throw 1;
        }
      }

    }
  }
  catch (...) {
    std::cerr << "ERROR" << std::endl;
  }
}