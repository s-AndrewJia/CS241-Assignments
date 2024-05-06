#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
const std::string CFG       = ".CFG";
const std::string INPUT     = ".INPUT";
const std::string ACTIONS   = ".ACTIONS";
const std::string END       = ".END";
const std::string EMPTY     = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

void print_progress(std::vector<std::string> red_s, std::vector<std::string> in_s) {
  for (int i=0;i<red_s.size();i++) {
    std::cout << red_s[i] << " ";
  }
  std::cout << ". ";
  for (int i=0;i<in_s.size();i++) {
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

  std::vector<std::string> reduction_sequence;

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

  for (int i=0;i<rules.size();i++) {
    for (int j=0;j<rules[i].size();j++) {
        std::cout << rules[j][i] << " ";
    }
    std::cout << std::endl;
  }


  // Input section
  while(in >> s) {
    if (s == ACTIONS) { 
      break; 
    } else {
      input_sequence.push_back(s);
    }
  }

  std::getline(in, s); // Actions section (skip header)
  while(std::getline(in, s)) {
    if (s == END) { 
      break; 
    } else {
      std::istringstream line(s);
      std::vector<std::string> lineVec;

      while (line >> s) {
        lineVec.push_back(s);
      }

      if (lineVec[0] == "print") {
        print_progress(reduction_sequence, input_sequence);
      }
      if (lineVec[0] == "shift") {
        std::string temp = input_sequence[0];
        reduction_sequence.push_back(temp);
        input_sequence.erase(input_sequence.begin());
      }
      if (lineVec[0] == "reduce") {
        int rule_ind = std::stoi(lineVec[1]);

        for (int i=rules[rule_ind].size()-1;i>=0;i--) {
          if (rules[rule_ind][i] == EMPTY) {
            continue;
          }
          if (rules[rule_ind][i] == reduction_sequence[reduction_sequence.size()-1]) {
            reduction_sequence.pop_back();
          }
        }

        reduction_sequence.push_back(rules_map[rule_ind]);
      }
    }
  }
}