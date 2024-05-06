#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include "wlp4data.h"
const std::string CFG           = ".CFG";
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

class TreeNode {
public:
    void set_children(std::vector<TreeNode> v) {
        for (int i=v.size()-1;i>=0;i--) {
            children.push_back(v[i]);
        }
    }
    TreeNode() {

    }
    TreeNode(std::vector<std::string> d) {
        for (int i=0;i<d.size();i++) {
            data.push_back(d[i]);
        }
    }
    TreeNode(std::vector<std::string> d, std::vector<std::string> d2) {
        for (int i=0;i<d.size();i++) {
            data.push_back(d[i]);
        }
        for (int i=0;i<d2.size();i++) {
            data.push_back(d2[i]);
        }
    }
    std::vector<TreeNode> children;
    std::vector<std::string> data;
};

void print_tree(TreeNode root) {
    for (int i=0;i<root.data.size();i++) {
        std::cout << root.data[i] << " ";
    }
    std::cout << std::endl;

    for (int i=0;i<root.children.size();i++) {
        print_tree(root.children[i]);
    }
}

void print_progress(std::vector<TreeNode> red_s, std::vector<std::vector<std::string>> in_s, int ind) {
  for (int i=0;i<red_s.size();i++) {
    std::cout << red_s[i].data[0] << " ";
  }
  std::cout << ". ";
  for (int i=ind;i<in_s.size();i++) {
    std::cout << in_s[i][0] << " ";
  }
  std::cout << std::endl;
}

void initialize_slr1(std::vector<std::vector<std::string>> &rules, std::unordered_map<int, std::string> &rules_map, std::unordered_map<int, std::unordered_map<std::string, int>> &transition_function, std::unordered_map<int, std::unordered_map<std::string, int>> &reduction_function) {
  int curr_i = 0;
  std::stringstream in(WLP4_COMBINED);
  std::string s;

  std::getline(in, s); // CFG section (skip header)
  // Read characters or ranges separated by whitespace
  while(std::getline(in, s)) {
    if (s == TRANSITIONS) { 
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

  // Transitions section
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
}

int main() {
  std::vector<std::vector<std::string>> rules;
  std::unordered_map<int, std::string> rules_map;

  std::unordered_map<int, std::unordered_map<std::string, int>> transition_function;
  std::unordered_map<int, std::unordered_map<std::string, int>> reduction_function;

  initialize_slr1(rules, rules_map, transition_function, reduction_function);

  std::istream& in = std::cin;
  std::string s;

  std::vector<std::vector<std::string>> input_sequence;

  input_sequence.push_back({"BOF", "BOF"});
  while (std::getline(in, s)) {
    std::istringstream line(s);
    std::vector<std::string> lineVec;

    while (line >> s) {
      lineVec.push_back(s);
    }

    input_sequence.push_back(lineVec);
  }
  input_sequence.push_back({"EOF", "EOF"});

  std::stack<int> state_st;
  std::vector<TreeNode> sym_st;

  state_st.push(0);

  try {
    for (int i=0;i<input_sequence.size();i++) {
      while (reduction_function[state_st.top()].count(input_sequence[i][0]) > 0) {
        int rule_ind = reduction_function[state_st.top()][input_sequence[i][0]];
        std::vector<TreeNode> temp;

        for (int j=rules[rule_ind].size()-1;j>=0;j--) {
          if (rules[rule_ind][j] == EMPTY) {
            continue;
          }
          else if (rules[rule_ind][j] == sym_st[sym_st.size()-1].data[0]) {
            temp.push_back(sym_st[sym_st.size()-1]);
            sym_st.pop_back();
            state_st.pop();
          }
          else {
            throw illegal_exception(i);
          }
        }
        TreeNode new_node = TreeNode({rules_map[rule_ind]}, rules[rule_ind]);
        new_node.set_children(temp);
        sym_st.push_back(new_node);
        state_st.push(transition_function[state_st.top()][rules_map[rule_ind]]);
      }
      sym_st.push_back(TreeNode(input_sequence[i]));
      if (transition_function[state_st.top()].count(input_sequence[i][0]) < 1 && reduction_function[state_st.top()].count(input_sequence[i][0]) < 1) {
        throw illegal_exception(i);
      }
      state_st.push(transition_function[state_st.top()][input_sequence[i][0]]);
    }

    if (reduction_function[state_st.top()].count(ACCEPT) >= 0) {
      int rule_ind = reduction_function[state_st.top()][ACCEPT];
      std::vector<TreeNode> temp;

      for (int j=rules[rule_ind].size()-1;j>=0;j--) {
        if (rules[rule_ind][j] == EMPTY) {
          continue;
        }
        else if (rules[rule_ind][j] == sym_st[sym_st.size()-1].data[0]) {
          temp.push_back(sym_st[sym_st.size()-1]);
          sym_st.pop_back();
        }
        else {
          throw illegal_exception(input_sequence.size());
        }
      }
      TreeNode new_node = TreeNode({rules_map[rule_ind]}, rules[rule_ind]);
      new_node.set_children(temp);
      sym_st.push_back(new_node);
    }
    else {
      throw illegal_exception(input_sequence.size());
    }
  }
  catch (illegal_exception ie) {
    std::cerr << "ERROR at " << ie.loc() << std::endl;
    return 1;
  }

  print_tree(sym_st[0]);

  return 0;
}