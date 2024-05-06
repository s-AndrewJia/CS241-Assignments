#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
const std::string CFG           = ".CFG";
const std::string TRANSITIONS   = ".TRANSITIONS";
const std::string REDUCTIONS    = ".REDUCTIONS";
const std::string END           = ".END";
const std::string ACCEPT        = ".ACCEPT";
const std::string EMPTY         = ".EMPTY";

class Procedure {
public:
  Procedure() {

  }
  Procedure(std::string n, std::vector<std::string> s, std::unordered_map<std::string, std::string> t) {
    name = n;
    signature = s;
    symbol_table = t;
  }
  std::string name;
  std::vector<std::string> signature; 
  std::unordered_map<std::string, std::string> symbol_table;
};

class TreeNode {
public:
  void set_children(std::vector<TreeNode*> v) {
    for (int i=v.size()-1;i>=0;i--) {
      children.push_back(v[i]);
    }
  }
  void add_child(TreeNode* t) {
    children.push_back(t);
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
  ~TreeNode() {

  }
  std::vector<TreeNode*> children;
  std::vector<std::string> data;
  std::string type = "";
};

std::vector<TreeNode*> generate_tree(std::vector<std::vector<std::string>> input_sequence) {
  std::vector<TreeNode*> nodes;
  std::stack<int> curr_node;
  std::stack<std::string> curr_name;

  for (int i=0;i<input_sequence.size();i++) {
    nodes.push_back(new TreeNode(input_sequence[i]));
  }

  curr_node.push(0);

  for (int i=input_sequence[0].size()-1;i>=1;i--) {
    curr_name.push(input_sequence[0][i]);
  }

  for (int i=1;i<input_sequence.size();i++) {
    if (input_sequence[i][0] == curr_name.top()) {
      if (std::isupper(input_sequence[i][0][0]) || input_sequence[i][1] == EMPTY) {
        nodes[curr_node.top()]->add_child(nodes[i]);
        while (nodes[curr_node.top()]->children.size() == input_sequence[curr_node.top()].size() - 1 && i != input_sequence.size()-1) {
          int ind = curr_node.top();
          curr_node.pop();
          curr_name.pop();
          nodes[curr_node.top()]->add_child(nodes[ind]);
        }
        curr_name.pop();
      }
      else {
        curr_node.push(i);
        for (int j=input_sequence[i].size()-1;j>=1;j--) {
          curr_name.push(input_sequence[i][j]);
        }
      }
    }
  }

  return nodes;
}

Procedure gen_procedure(TreeNode* node) {
  std::string name = node->children[1]->data[1];
  std::vector<std::string> sig;
  std::unordered_map<std::string, std::string> tab; 

  TreeNode* params_node = node->children[3];

  if (params_node->data[1] != EMPTY) {
    params_node = params_node->children[0];

    std::string param_id;
    std::string param_type;

    while (params_node->children.size() == 3) {
      param_id = params_node->children[0]->children[1]->data[1];

      if (params_node->children[0]->children[0]->children.size() == 1) {
        param_type = "int";
      }
      else if (params_node->children[0]->children[0]->children.size() == 2) {
        param_type = "int*";
      }
      else {
        throw 1;
      }

      if (tab.count(param_id) > 0) {
        throw 1;
      }

      sig.push_back(param_type);
      tab[param_id] = param_type;

      params_node->children[0]->children[1]->type = param_type;

      params_node = params_node->children[2];
    }

    param_id = params_node->children[0]->children[1]->data[1];

    if (params_node->children[0]->children[0]->children.size() == 1) {
      param_type = "int";
    }
    else if (params_node->children[0]->children[0]->children.size() == 2) {
      param_type = "int*";
    }
    else {
      throw 1;
    }

    sig.push_back(param_type);
    tab[param_id] = param_type;

    params_node->children[0]->children[1]->type = param_type;
  }

  TreeNode* dcls_node = node->children[6];

  while (dcls_node->children.size() != 0) {
    std::string dcl_id;
    std::string dcl_type;
    std::string become_type;

    if (tab.count(dcls_node->children[1]->children[1]->data[1]) > 0) {
      throw 1;
    }
    dcl_id = dcls_node->children[1]->children[1]->data[1];

    if (dcls_node->children[1]->children[0]->children.size() == 1) {
      dcl_type = "int";
    }
    else if (dcls_node->children[1]->children[0]->children.size() == 2) {
      dcl_type = "int*";
    }
    else {
      throw 1;
    }

    if (dcls_node->children[3]->data[0] == "NUM") {
      become_type = "int";
    }
    else if (dcls_node->children[3]->data[0] == "NULL") {
      become_type = "int*";
    }
    else {
      throw 1;
    }
    
    if (dcl_type != become_type) {
      throw 1;
    }

    tab[dcl_id] = dcl_type;
    dcls_node->children[3]->type = become_type;

    dcls_node->children[1]->children[1]->type = dcl_type;

    dcls_node = dcls_node->children[0];
  }
  
  return Procedure(name, sig, tab);
}

Procedure gen_wain(TreeNode* node) {
  std::string name = "wain";
  std::vector<std::string> sig;
  std::unordered_map<std::string, std::string> table;

  // for parameters
  std::string param_1_id = node->children[3]->children[1]->data[1];
  std::string param_2_id = node->children[5]->children[1]->data[1];
  std::string param_1_type;
  std::string param_2_type;

  if (node->children[3]->children[0]->children.size() == 1) {
    param_1_type = "int";
  }
  else if (node->children[3]->children[0]->children.size() == 2) {
    param_1_type = "int*";
  }
  else {
    throw 1;
  }

  if (node->children[5]->children[0]->children.size() == 1) {
    param_2_type = "int";
  }
  else {
    throw 1;
  }

  if (param_1_id == param_2_id) {
    throw 1;
  }

  sig.push_back(param_1_type);
  sig.push_back(param_2_type);
  table[param_1_id] = param_1_type;
  table[param_2_id] = param_2_type;

  node->children[3]->children[1]->type = param_1_type;
  node->children[5]->children[1]->type = param_2_type;

  node = node->children[8];

  while (node->children.size() != 0) {
    std::string dcl_id;
    std::string dcl_type;
    std::string become_type;

    if (table.count(node->children[1]->children[1]->data[1]) > 0) {
      throw 1;
    }
    dcl_id = node->children[1]->children[1]->data[1];

    if (node->children[1]->children[0]->children.size() == 1) {
      dcl_type = "int";
    }
    else if (node->children[1]->children[0]->children.size() == 2) {
      dcl_type = "int*";
    }
    else {
      throw 1;
    }

    if (node->children[3]->data[0] == "NUM") {
      become_type = "int";
    }
    else if (node->children[3]->data[0] == "NULL") {
      become_type = "int*";
    }
    else {
      throw 1;
    }
    
    if (dcl_type != become_type) {
      throw 1;
    }

    table[dcl_id] = dcl_type;
    node->children[3]->type = become_type;

    node->children[1]->children[1]->type = dcl_type;

    node = node->children[0];
  }

  return Procedure(name, sig, table);
}

std::string typecheck(TreeNode* node, std::unordered_map<std::string, Procedure> procedure_map, std::string curr_proc) {
  if (node->data[0] == "statements") {
    // TreeNode* curr = node;
    // while (curr->children.size() == 2) {
    //   typecheck(node->children[1], procedure_map, curr_proc);
    //   curr = curr->children[0];
    // }
    if (node->children.size() == 0) {
      return "";
    }
    typecheck(node->children[0], procedure_map, curr_proc);
    typecheck(node->children[1], procedure_map, curr_proc);
    return "";
  }
  
  if (node->children.size() == 0) {
    if (node->data[0] == "NUM") {
      node->type = "int";
      return "int";
    }
    else if (node->data[0] == "NULL") {
      node->type = "int*";
      return "int*";
    }
    else if (node->data[0] == "ID") {
      if (procedure_map[curr_proc].symbol_table.count(node->data[1]) < 1) {
        throw 1;
      }
      node->type = procedure_map[curr_proc].symbol_table[node->data[1]];
      return procedure_map[curr_proc].symbol_table[node->data[1]];
    }
  }
  else {
    std::string type = "";
    if (node->data[0] == "expr") {
      if (node->children.size() == 1) {
        if (node->data[1] == "term") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "expr" && node->data[2] == "PLUS" && node->data[3] == "term") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (type == "int" && rhs == "int") {
            type = "int";
          }
          else if ((type == "int" && rhs == "int*") || (type == "int*" && rhs == "int")) {
            type = "int*";
          }
          else {
            throw 1;
          }
        }
        else if (node->data[1] == "expr" && node->data[2] == "MINUS" && node->data[3] == "term") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if ((type == "int" && rhs == "int") || (type == "int*" && rhs == "int*")) {
            type = "int";
          }
          else if (type == "int*" && rhs == "int") {
            type = "int*";
          }
          else {
            throw 1;
          }
        }
      }
    }
    else if (node->data[0] == "term") {
      if (node->children.size() == 1) {
        if (node->data[1] == "factor") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "term" && node->data[2] == "STAR" && node->data[3] == "factor") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (type == "int" && rhs == "int") {
            type = "int";
          }
          else {
            throw 1;
          }
        }
        else if (node->data[1] == "term" && node->data[2] == "SLASH" && node->data[3] == "factor") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (type == "int" && rhs == "int") {
            type = "int";
          }
          else {
            throw 1;
          }
        }
        else if (node->data[1] == "term" && node->data[2] == "PCT" && node->data[3] == "factor") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (type == "int" && rhs == "int") {
            type = "int";
          }
          else {
            throw 1;
          }
        }
      }
    }
    else if (node->data[0] == "factor") {
      if (node->children.size() == 1) {
        if (node->data[1] == "ID" || node->data[1] == "NUM" || node->data[1] == "NULL") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
        }
      }
      else if (node->children.size() == 2) {
        if (node->data[1] == "AMP" && node->data[2] == "lvalue") {
          type = typecheck(node->children[1], procedure_map, curr_proc);
          if (type == "int") {
            type = "int*";
          }
          else {
            throw 1;
          }
        }
        if (node->data[1] == "STAR" && node->data[2] == "factor") {
          type = typecheck(node->children[1], procedure_map, curr_proc);
          if (type == "int*") {
            type = "int";
          }
          else {
            throw 1;
          }
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "LPAREN" && node->data[2] == "expr" && node->data[3] == "RPAREN") {
          type = typecheck(node->children[1], procedure_map, curr_proc);
        }
        if (node->data[1] == "ID" && node->data[2] == "LPAREN" && node->data[3] == "RPAREN") {
          if ((node->children[0]->data[1] == curr_proc) && (procedure_map[curr_proc].symbol_table.count(curr_proc) > 0)) {
            throw 1;
          }
          if (procedure_map.count(node->children[0]->data[1]) < 1) {
            throw 1;
          }
          if (procedure_map[node->children[0]->data[1]].signature.size() == 0) {
            type = "int";
          }
          else {
            throw 1;
          }
        }
      }
      else if (node->children.size() == 4) {
        if (node->data[1] == "ID" && node->data[2] == "LPAREN" && node->data[3] == "arglist" && node->data[4] == "RPAREN") {
          if ((node->children[0]->data[1] == curr_proc) && (procedure_map[curr_proc].symbol_table.count(curr_proc) > 0)) {
            throw 1;
          }
          if (procedure_map.count(node->children[0]->data[1]) < 1) {
            throw 1;
          }
          std::vector<std::string> arg_list;
          TreeNode* arg_node = node->children[2];
          while (arg_node->children.size() == 3) {
            arg_list.push_back(typecheck(arg_node->children[0], procedure_map, curr_proc));
            arg_node = arg_node->children[2];
          }
          arg_list.push_back(typecheck(arg_node->children[0], procedure_map, curr_proc));

          if (arg_list.size() != procedure_map[node->children[0]->data[1]].signature.size()) {
            throw 1;
          }
          for (int i=0;i<arg_list.size();i++) {
            if (arg_list[i] != procedure_map[node->children[0]->data[1]].signature[i]) {
              throw 1;
            }
          }

          type = "int";
        }
      }
      else if (node->children.size() == 5) {
        if (node->data[1] == "NEW" && node->data[2] == "INT" && node->data[3] == "LBRACK" && node->data[4] == "expr" && node->data[5] == "RBRACK") {
          type = typecheck(node->children[3], procedure_map, curr_proc);
          if (type == "int") {
            type = "int*";
          }
          else {
            throw 1;
          }
        }
      }
      else {
        throw 1;
      }
    }
    else if (node->data[0] == "lvalue") {
      if (node->children.size() == 1) {
        if (node->data[1] == "ID") {
          type = typecheck(node->children[0], procedure_map, curr_proc);
        }
      }
      else if (node->children.size() == 2) {
        if (node->data[1] == "STAR" && node->data[2] == "factor") {
          type = typecheck(node->children[1], procedure_map, curr_proc);
          if (type == "int*") {
            type = "int";
          }
          else {
            throw 1;
          }
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "LPAREN" && node->data[2] == "lvalue" && node->data[3] == "RPAREN") {
          type = typecheck(node->children[1], procedure_map, curr_proc);
        }
      }
    }
    else if (node->data[0] == "test") {
      if (node->children.size() == 3) {
        if (node->data[1] == "expr" && node->data[3] == "expr" && (node->data[2] == "EQ" || node->data[2] == "NE" || node->data[2] == "LT" || node->data[2] == "LE" || node->data[2] == "GT" || node->data[2] == "GE")) {
          std::string lhs = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (lhs != rhs) {
            throw 1;
          }
        }
      }
    }
    else if (node->data[0] == "statement") {
      if (node->children.size() == 4) {
        if (node->data[1] == "lvalue" && node->data[2] == "BECOMES" && node->data[3] == "expr" && node->data[4] == "SEMI") {
          std::string lhs = typecheck(node->children[0], procedure_map, curr_proc);
          std::string rhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (lhs != rhs) {
            throw 1;
          }
        }
      }
      else if (node->children.size() == 5) {
        if (node->data[1] == "PRINTLN" && node->data[2] == "LPAREN" && node->data[3] == "expr" && node->data[4] == "RPAREN" && node->data[5] == "SEMI") {
          std::string lhs = typecheck(node->children[2], procedure_map, curr_proc);
          if (lhs != "int") {
            throw 1;
          }
        }
        else if (node->data[1] == "DELETE" && node->data[2] == "LBRACK" && node->data[3] == "RBRACK" && node->data[4] == "expr" && node->data[5] == "SEMI") {
          std::string lhs = typecheck(node->children[3], procedure_map, curr_proc);
          if (lhs != "int*") {
            throw 1;
          }
        }
      }
      else if (node->children.size() == 7) {
        if (node->data[1] == "WHILE" && node->data[2] == "LPAREN" && node->data[3] == "test" && node->data[4] == "RPAREN" && node->data[5] == "LBRACE" && node->data[6] == "statements" && node->data[7] == "RBRACE") {
          typecheck(node->children[2], procedure_map, curr_proc);
          typecheck(node->children[5], procedure_map, curr_proc);
        }
      }
      else if (node->children.size() == 11) {
        if (node->data[1] == "IF" && node->data[2] == "LPAREN" && node->data[3] == "test" && node->data[4] == "RPAREN" && node->data[5] == "LBRACE" && node->data[6] == "statements" && node->data[7] == "RBRACE" && node->data[8] == "ELSE" && node->data[9] == "LBRACE" && node->data[10] == "statements" && node->data[11] == "RBRACE") {
          typecheck(node->children[2], procedure_map, curr_proc);
          typecheck(node->children[5], procedure_map, curr_proc);
          typecheck(node->children[9], procedure_map, curr_proc);
        }
      }
    }
    else {
      throw 1;
    }

    node->type = type;
    return type;
  }
}

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::vector<std::vector<std::string>> input_sequence;

  while (std::getline(in, s)) {
    std::istringstream line(s);
    std::vector<std::string> lineVec;

    while (line >> s) {
      lineVec.push_back(s);
    }

    input_sequence.push_back(lineVec);
  }

  std::vector<TreeNode*> tree_vector = generate_tree(input_sequence);
  TreeNode* root = tree_vector[0];

  std::unordered_map<std::string, Procedure> procedure_map;

  try {
    TreeNode* curr_node = root->children[1];

    while (curr_node->data[1] != "main") {
      Procedure P = gen_procedure(curr_node->children[0]);
      if (procedure_map.count(P.name) > 0) {
        throw 1;
      }
      procedure_map[P.name] = P;

      typecheck(curr_node->children[0]->children[7], procedure_map, procedure_map[curr_node->children[0]->children[1]->data[1]].name);

      if (typecheck(curr_node->children[0]->children[9], procedure_map, procedure_map[curr_node->children[0]->children[1]->data[1]].name) != "int") {
        throw 1;
      }

      curr_node = curr_node->children[1];
    }

    std::unordered_map<std::string, std::string> main_table;
    procedure_map["wain"] = gen_wain(curr_node->children[0]);

    curr_node = curr_node->children[0];

    // std::cout << curr_node->children[9]->children[1]->data[1] << " ";
    typecheck(curr_node->children[9], procedure_map, "wain");

    if (typecheck(curr_node->children[11], procedure_map, "wain") != "int") {
      throw 1;
    }

    // printing
    std::stack<TreeNode*> node_stack;
    node_stack.push(root);
    std::string curr_proc;

    while (!node_stack.empty()) {
      TreeNode* node_top = node_stack.top();
      node_stack.pop();

      for (int i=0;i<node_top->data.size();i++) {
        std::cout << node_top->data[i] << " ";
      }
      if (node_top->data[0] == "procedures") {
        curr_proc = node_top->children[0]->children[1]->data[1];
      }
      if (node_top->type != "") {
        std::cout << ": " << node_top->type;
      }
      std::cout << std::endl;

      for (int i=node_top->children.size()-1;i>=0;i--) {
        node_stack.push(node_top->children[i]);
      }
    }
  }
  catch (...) {
    std::cerr << "ERROR" << std::endl;
    for (int i=0;i<tree_vector.size();i++) {
      delete tree_vector[i];
    }
    return 1;
  }

  for (int i=0;i<tree_vector.size();i++) {
    delete tree_vector[i];
  }

  return 0;
}