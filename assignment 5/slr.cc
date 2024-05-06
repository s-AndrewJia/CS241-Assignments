#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
const std::string CFG           = ".CFG";
const std::string INPUT         = ".INPUT";
const std::string TRANSITIONS   = ".TRANSITIONS";
const std::string REDUCTIONS    = ".REDUCTIONS";
const std::string END           = ".END";
const std::string ACCEPT        = ".ACCEPT";
const std::string EMPTY         = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

class illegal_exception {
public:
    illegal_exception(int x) {
        ind = x;
    }
    int loc() {
        return ind;
    }

private:
    int ind;
};

void print_progress(std::vector<std::string> red_s, std::vector<std::string> in_s, int ind) {
  for (int i=0;i<red_s.size();i++) {
    std::cout << red_s[i] << " ";
  }
  std::cout << ". ";
  for (int i=ind;i<in_s.size();i++) {
    std::cout << in_s[i] << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::vector<std::vector<std::string>> rules;
  std::unordered_map<int, std::string> rules_map;
  int curr_i = 0;

  std::vector<std::string> input_sequence;

  std::unordered_map<int, std::unordered_map<std::string, int>> transition_function;
  std::unordered_map<int, std::unordered_map<std::string, int>> reduction_function;

  std::getline(in, s); // CFG section (skip header)
  // Read characters or ranges separated by whitespace
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      break; 
    } else {
      std::istringstream line(s);
      std::vector<std::string> lineVec;

      while (line >> s) {
        lineVec.push_back(s);
      }

      std::string reduction = lineVec[0];
      lineVec.erase(lineVec.begin());

      rules.push_back(lineVec);
      rules_map[curr_i] = reduction;
      curr_i++;
    }
  }

  // Input section
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      input_sequence.push_back(s);
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  while(std::getline(in, s)) {
    if (s == REDUCTIONS) { 
      break; 
    } else {
      std::istringstream line(s);
      std::vector<std::string> lineVec;

      while (line >> s) {
        lineVec.push_back(s);
      }

      int from_state = std::stoi(lineVec[0]);
      int to_state = std::stoi(lineVec[2]);
      std::string sym = lineVec[1];

      transition_function[from_state][sym] = to_state;
    }
  }

  while(std::getline(in, s)) {
    if (s == END) { 
      break; 
    } else {
      std::istringstream line(s);
      std::vector<std::string> lineVec;

      while (line >> s) {
        lineVec.push_back(s);
      }

      int state_num = std::stoi(lineVec[0]);
      int rule_num = std::stoi(lineVec[1]);
      std::string tag = lineVec[2];

      reduction_function[state_num][tag] = rule_num;
    }
  }

  std::stack<int> state_st;
  std::vector<std::string> sym_st;

  state_st.push(0);

  try {
    for (int i=0;i<input_sequence.size();i++) {
      while (reduction_function[state_st.top()].count(input_sequence[i]) > 0) {
        int rule_ind = reduction_function[state_st.top()][input_sequence[i]];
        
        print_progress(sym_st, input_sequence, i);

        for (int j=rules[rule_ind].size()-1;j>=0;j--) {
          if (rules[rule_ind][j] == EMPTY) {
            continue;
          }
          else if (rules[rule_ind][j] == sym_st[sym_st.size()-1]) {
            sym_st.pop_back();
            state_st.pop();
          }
          else {
            throw illegal_exception(i+1);
          }
        }
        
        sym_st.push_back(rules_map[rule_ind]);
        state_st.push(transition_function[state_st.top()][rules_map[rule_ind]]);
      }
      print_progress(sym_st, input_sequence, i);
      sym_st.push_back(input_sequence[i]);
      if (transition_function[state_st.top()].count(input_sequence[i]) < 1 && reduction_function[state_st.top()].count(input_sequence[i]) < 1) {
        throw illegal_exception(i+1);
      }
      state_st.push(transition_function[state_st.top()][input_sequence[i]]);
    }

    print_progress(sym_st, input_sequence, input_sequence.size());

    if (reduction_function[state_st.top()].count(ACCEPT) >= 0) {
      int rule_ind = reduction_function[state_st.top()][ACCEPT];

      for (int j=rules[rule_ind].size()-1;j>=0;j--) {
        if (rules[rule_ind][j] == EMPTY) {
          continue;
        }
        else if (rules[rule_ind][j] == sym_st[sym_st.size()-1]) {
          sym_st.pop_back();
        }
        else {
          throw illegal_exception(input_sequence.size()+1);
        }
      }

      sym_st.push_back(rules_map[rule_ind]);
    }
    else {
      throw illegal_exception(input_sequence.size()+1);
    }

    print_progress(sym_st, input_sequence, input_sequence.size());
  }
  catch (illegal_exception ie) {
    std::cerr << "ERROR at " << ie.loc() << std::endl;
  }

  return 0;
}