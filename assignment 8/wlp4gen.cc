#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
const std::string CFG           = ".CFG";
const std::string TRANSITIONS   = ".TRANSITIONS";
const std::string REDUCTIONS    = ".REDUCTIONS";
const std::string END           = ".END";
const std::string ACCEPT        = ".ACCEPT";
const std::string EMPTY         = ".EMPTY";

int label_counter = 0;

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
    TreeNode* temp;
    if (input_sequence[i].size() >= 2 && input_sequence[i][input_sequence[i].size()-2] == ":") {
      std::string temp_type = input_sequence[i][input_sequence[i].size()-1];
      input_sequence[i].resize(input_sequence[i].size()-2);
      temp = new TreeNode(input_sequence[i]);
      temp->type = temp_type;
    }
    else {
      temp = new TreeNode(input_sequence[i]);
    }
    nodes.push_back(temp);
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

void push(std::string reg) {
  std::cout << "sw " << reg << ", -4($30)" << std::endl;
  std::cout << "sub $30, $30, $4" << std::endl;
}

void pop(std::string reg) {
  std::cout << "add $30, $30, $4" << std::endl;
  std::cout << "lw " << reg << ", -4($30)" << std::endl;
}

std::unordered_map<std::string, int> gen_wain(TreeNode* node, int &offset) {
  std::unordered_map<std::string, int> table;

  std::string param_1_id = node->children[3]->children[1]->data[1];
  std::string param_2_id = node->children[5]->children[1]->data[1];
  
  table[param_1_id] = offset;
  offset -= 4;
  table[param_2_id] = offset;
  offset -= 4;

  push("$1");
  push("$2");

  node = node->children[8];

  while (node->children.size() != 0) {
    std::string dcl_id = node->children[1]->children[1]->data[1];

    table[dcl_id] = offset;
    offset -= 4;

    if (node->children[3]->data[0] == "NUM") {
      std::cout << "lis $5" << std::endl;
      std::cout << ".word " << node->children[3]->data[1] << std::endl;
    }
    if (node->children[3]->data[0] == "NULL") {
      std::cout << "add $5, $11, $0" << std::endl;
    }

    push("$5");

    node = node->children[0];
  }

  return table;
}

std::unordered_map<std::string, int> gen_proc(TreeNode* node, int &offset) {
  std::unordered_map<std::string, int> table;

  TreeNode* params_node = node->children[3];

  if (params_node->data[1] != EMPTY) {
    params_node = params_node->children[0];

    std::queue<std::string> param_ids;
    int n = 1;
    while (params_node->children.size() == 3) {
      param_ids.push(params_node->children[0]->children[1]->data[1]);
      params_node = params_node->children[2];
      n++;
    }
    param_ids.push(params_node->children[0]->children[1]->data[1]);

    int i = 1;
    while (!param_ids.empty()) {
      std::string param_id = param_ids.front();
      param_ids.pop();

      table[param_id] = 4*(n - i + 1);

      i++;
    }
  }

  TreeNode* dcls_node = node->children[6];

  while (dcls_node->children.size() != 0) {
    std::string dcl_id = dcls_node->children[1]->children[1]->data[1];
    std::string dcl_val = dcls_node->children[3]->data[1];

    table[dcl_id] = offset;
    offset -= 4;

    if (dcls_node->children[3]->data[0] == "NUM") {
      std::cout << "lis $5" << std::endl;
      std::cout << ".word " << dcl_val << std::endl;
    }
    if (dcls_node->children[3]->data[0] == "NULL") {
      std::cout << "add $5, $11, $0" << std::endl;
    }

    push("$5");

    dcls_node = dcls_node->children[0];
  }

  return table;
}

void code(TreeNode* node, std::unordered_map<std::string, int> offset_map) {
  if (node->data[0] == "statements") {
    if (node->children.size() == 0) {
      return;
    }
    code(node->children[0], offset_map);
    code(node->children[1], offset_map);
    return;
  }

  if (node->children.size() == 0) {
    if (node->data[0] == "NUM") {
      std::cout << "lis $3" << std::endl;
      std::cout << ".word " << node->data[1] << std::endl;
      return;
    }
    else if (node->data[0] == "NULL") {
      std::cout << "add $3, $0, $11" << std::endl;
    }
    else if (node->data[0] == "ID") {
      std::cout << "lw $3, " << offset_map[node->data[1]] << "($29)" << std::endl;
      return;
    }
  }
  else {
    if (node->data[0] == "expr") {
      if (node->children.size() == 1) {
        if (node->data[1] == "term") {
          code(node->children[0], offset_map);
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "expr" && node->data[2] == "PLUS" && node->data[3] == "term") {
          if (node->children[0]->type == "int" && node->children[2]->type == "int") {
            code(node->children[0], offset_map);
            push("$3");
            code(node->children[2], offset_map);
            pop("$5");
            std::cout << "add $3, $5, $3" << std::endl;
          }
          if (node->children[0]->type == "int*" && node->children[2]->type == "int") {
            code(node->children[0], offset_map);
            push("$3");
            code(node->children[2], offset_map);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            pop("$5");
            std::cout << "add $3, $5, $3" << std::endl;
          }
          if (node->children[0]->type == "int" && node->children[2]->type == "int*") {
            code(node->children[2], offset_map);
            push("$3");
            code(node->children[0], offset_map);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            pop("$5");
            std::cout << "add $3, $5, $3" << std::endl;
          }
        }
        else if (node->data[1] == "expr" && node->data[2] == "MINUS" && node->data[3] == "term") {
          if (node->children[0]->type == "int" && node->children[2]->type == "int") {
            code(node->children[0], offset_map);
            push("$3");
            code(node->children[2], offset_map);
            pop("$5");
            std::cout << "sub $3, $5, $3" << std::endl;
          }
          if (node->children[0]->type == "int*" && node->children[2]->type == "int") {
            code(node->children[0], offset_map);
            push("$3");
            code(node->children[2], offset_map);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            pop("$5");
            std::cout << "sub $3, $5, $3" << std::endl;
          }
          if (node->children[0]->type == "int*" && node->children[2]->type == "int*") {
            code(node->children[0], offset_map);
            push("$3");
            code(node->children[2], offset_map);
            pop("$5");
            std::cout << "sub $3, $5, $3" << std::endl;
            std::cout << "div $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
          }
        }
      }
    }
    else if (node->data[0] == "term") {
      if (node->children.size() == 1) {
        if (node->data[1] == "factor") {
          code(node->children[0], offset_map);
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "term" && node->data[2] == "STAR" && node->data[3] == "factor") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          std::cout << "mult $3, $5" << std::endl;
          std::cout << "mflo $3" << std::endl;
        }
        else if (node->data[1] == "term" && node->data[2] == "SLASH" && node->data[3] == "factor") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          std::cout << "div $5, $3" << std::endl;
          std::cout << "mflo $3" << std::endl;
        }
        else if (node->data[1] == "term" && node->data[2] == "PCT" && node->data[3] == "factor") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          std::cout << "div $5, $3" << std::endl;
          std::cout << "mfhi $3" << std::endl;
        }
      }
    }
    else if (node->data[0] == "factor") {
      if (node->children.size() == 1) {
        if (node->data[1] == "NUM" || node->data[1] == "NULL" || node->data[1] == "ID") {
          code(node->children[0], offset_map);
        }
      }
      else if (node->children.size() == 2) {
        if (node->data[1] == "AMP" && node->data[2] == "lvalue") {
          TreeNode* lvalue_node = node->children[1];
          while (lvalue_node->children.size() == 3) {
            lvalue_node = lvalue_node->children[1];
          }

          if (lvalue_node->data[1] == "ID") {
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << offset_map[lvalue_node->children[0]->data[1]] << std::endl;
            std::cout << "add $3, $3, $29" << std::endl;
          }
          if (lvalue_node->data[1] == "STAR" && lvalue_node->data[2] == "factor") {
            code(lvalue_node->children[1], offset_map);
          }
        }
        if (node->data[1] == "STAR" && node->data[2] == "factor") {
          code(node->children[1], offset_map);
          std::cout << "lw $3, 0($3)" << std::endl;
        }
      }
      else if (node->children.size() == 3) {
        if (node->data[1] == "LPAREN" && node->data[2] == "expr" && node->data[3] == "RPAREN") {
          code(node->children[1], offset_map);
        }
        if (node->data[1] == "ID" && node->data[2] == "LPAREN" && node->data[3] == "RPAREN") {
          std::string name = node->children[0]->data[1];
          name = "F" + name;

          push("$29");
          push("$31");
          std::cout << "lis $5" << std::endl;
          std::cout << ".word " << name << std::endl;
          std::cout << "jalr $5" << std::endl;
          pop("$31");
          pop("$29");
        }
      }
      else if (node->children.size() == 4) {
        if (node->data[1] == "ID" && node->data[2] == "LPAREN" && node->data[3] == "arglist" && node->data[4] == "RPAREN") {
          std::string name = node->children[0]->data[1];
          name = "F" + name;
          
          push("$29");
          push("$31");

          int size = 1;
          TreeNode* arg_node = node->children[2];
          while (arg_node->children.size() == 3) {
            code(arg_node->children[0], offset_map);
            push("$3");
            arg_node = arg_node->children[2];
            size++;
          }
          code(arg_node->children[0], offset_map);
          push("$3");

          std::cout << "lis $5" << std::endl;
          std::cout << ".word " << name << std::endl;
          std::cout << "jalr $5" << std::endl;

          for (int i=0;i<size;i++) {
            pop("$31");
          }

          pop("$31");
          pop("$29");
        }
      }
      else if (node->children.size() == 5) {
        if (node->data[1] == "NEW" && node->data[2] == "INT" && node->data[3] == "LBRACK" && node->data[4] == "expr" && node->data[5] == "RBRACK") {
          code(node->children[3], offset_map);
          std::cout << "add $1, $3, $0" << std::endl;
          push("$31");
          std::cout << "lis $5" << std::endl;
          std::cout << ".word new" << std::endl;
          std::cout << "jalr $5" << std::endl;
          pop("$31");
          std::cout << "bne $3, $0, 1" << std::endl;
          std::cout << "add $3, $11, $0" << std::endl;
        }
      }
    }
    else if (node->data[0] == "test") {
      if (node->children.size() == 3) {
        if (node->data[1] == "expr" && node->data[2] == "EQ" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $6, $3, $5" << std::endl;
            std::cout << "slt $7, $5, $3" << std::endl;
          }
          else {
            std::cout << "sltu $6, $3, $5" << std::endl;
            std::cout << "sltu $7, $5, $3" << std::endl;
          }
          std::cout << "add $3, $6, $7" << std::endl;
          std::cout << "sub $3, $11, $3" << std::endl;
        }
        else if (node->data[1] == "expr" && node->data[2] == "NE" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $6, $3, $5" << std::endl;
            std::cout << "slt $7, $5, $3" << std::endl;
          }
          else {
            std::cout << "sltu $6, $3, $5" << std::endl;
            std::cout << "sltu $7, $5, $3" << std::endl;
          }
          std::cout << "add $3, $6, $7" << std::endl;
        }
        else if (node->data[1] == "expr" && node->data[2] == "LT" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $3, $5, $3" << std::endl;
          }
          else {
            std::cout << "sltu $3, $5, $3" << std::endl;
          }
        }
        else if (node->data[1] == "expr" && node->data[2] == "GT" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $3, $3, $5" << std::endl;
          }
          else {
            std::cout << "sltu $3, $3, $5" << std::endl;
          }
        }
        else if (node->data[1] == "expr" && node->data[2] == "LE" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $3, $3, $5" << std::endl;
          }
          else {
            std::cout << "sltu $3, $3, $5" << std::endl;
          }
          std::cout << "sub $3, $11, $3" << std::endl;
        }
        else if (node->data[1] == "expr" && node->data[2] == "GE" && node->data[3] == "expr") {
          code(node->children[0], offset_map);
          push("$3");
          code(node->children[2], offset_map);
          pop("$5");
          if (node->children[0]->type == "int") {
            std::cout << "slt $3, $5, $3" << std::endl;
          }
          else {
            std::cout << "sltu $3, $5, $3" << std::endl;
          }
          std::cout << "sub $3, $11, $3" << std::endl;
        }
      }
    }
    else if (node->data[0] == "statement") {
      if (node->children.size() == 4) {
        if (node->data[1] == "lvalue" && node->data[2] == "BECOMES" && node->data[3] == "expr" && node->data[4] == "SEMI") {
          TreeNode* lvalue_node = node->children[0];
          while (lvalue_node->children.size() == 3) {
            lvalue_node = lvalue_node->children[1];
          }

          if (lvalue_node->data[1] == "ID") {
            code(node->children[2], offset_map);
            std::cout << "sw $3, " << offset_map[lvalue_node->children[0]->data[1]] << "($29)" << std::endl;
          }
          if (lvalue_node->data[1] == "STAR" && lvalue_node->data[2] == "factor") {
            code(node->children[2], offset_map);
            push("$3");
            code(lvalue_node->children[1], offset_map);
            pop("$5");
            std::cout << "sw $5, 0($3)" << std::endl;
          }
        }
      }
      else if (node->children.size() == 5) {
        if (node->data[1] == "PRINTLN" && node->data[2] == "LPAREN" && node->data[3] == "expr" && node->data[4] == "RPAREN" && node->data[5] == "SEMI") {
          push("$1");
          code(node->children[2], offset_map);
          std::cout << "add $1, $3, $0" << std::endl;
          push("$31");
          std::cout << "jalr $10" << std::endl;
          pop("$31");
          pop("$1");
        }
        else if (node->data[1] == "DELETE" && node->data[2] == "LBRACK" && node->data[3] == "RBRACK" && node->data[4] == "expr" && node->data[5] == "SEMI") {
          int ln1 = label_counter;
          label_counter++;
          
          code(node->children[3], offset_map);
          std::cout << "beq $3, $11, skipDelete" << ln1 << std::endl;
          std::cout << "add $1, $3, $0" << std::endl;
          push("$31");
          std::cout << "lis $5" << std::endl;
          std::cout << ".word delete" << std::endl;
          std::cout << "jalr $5" << std::endl;
          pop("$31");
          std::cout << "skipDelete" << ln1 << ":" << std::endl;
        }
      }
      else if (node->children.size() == 7) {
        if (node->data[1] == "WHILE" && node->data[2] == "LPAREN" && node->data[3] == "test" && node->data[4] == "RPAREN" && node->data[5] == "LBRACE" && node->data[6] == "statements" && node->data[7] == "RBRACE") {
          int ln1 = label_counter;
          label_counter++;
          int ln2 = label_counter;
          label_counter++;

          std::cout << "loop" << ln1 << ":" << std::endl;
          code(node->children[2], offset_map);
          std::cout << "beq $3, $0, endWhile" << ln2 << std::endl;
          code(node->children[5], offset_map);
          std::cout << "beq $0, $0, loop" << ln1 << std::endl;
          std::cout << "endWhile" << ln2 << ":" << std::endl;
        }
      }
      else if (node->children.size() == 11) {
        if (node->data[1] == "IF" && node->data[2] == "LPAREN" && node->data[3] == "test" && node->data[4] == "RPAREN" && node->data[5] == "LBRACE" && node->data[6] == "statements" && node->data[7] == "RBRACE" && node->data[8] == "ELSE" && node->data[9] == "LBRACE" && node->data[10] == "statements" && node->data[11] == "RBRACE") {
          int ln1 = label_counter;
          label_counter++;
          int ln2 = label_counter;
          label_counter++;

          code(node->children[2], offset_map);
          std::cout << "beq $3, $0, else" << ln1 << std::endl;
          code(node->children[5], offset_map);
          std::cout << "beq $0, $0, endif" << ln2 << std::endl;
          std::cout << "else" << ln1 << ":" << std::endl;
          code(node->children[9], offset_map);
          std::cout << "endif" << ln2 << ":" << std::endl;
        }
      }
    }
  }
}

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::vector<std::vector<std::string>> input_sequence;

  std::unordered_map<std::string, int> symbol_table;

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

  std::unordered_map<std::string, std::unordered_map<std::string, int>> offset_map;      // can replace with procedure later

  int offset = 0;

  TreeNode* curr_node = root->children[1];

  std::stack<TreeNode*> proc_st;
  while (curr_node->data[1] != "main") {
    proc_st.push(curr_node->children[0]);
    curr_node = curr_node->children[1];
  }

  // wain function

  std::cout << ".import print" << std::endl;
  std::cout << ".import init" << std::endl;
  std::cout << ".import new" << std::endl;
  std::cout << ".import delete" << std::endl << std::endl;
  std::cout << "; begin prologue" << std::endl;
  std::cout << "lis $4" << std::endl;
  std::cout << ".word 4" << std::endl;
  std::cout << "lis $10" << std::endl;
  std::cout << ".word print" << std::endl;
  std::cout << "lis $11" << std::endl;
  std::cout << ".word 1" << std::endl;
  std::cout << "sub $29, $30, $4" << std::endl;

  curr_node = curr_node->children[0];

  if (curr_node->children[3]->children[1]->type == "int*") {
    push("$31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word init" << std::endl;
    std::cout << "jalr $5" << std::endl;
    pop("$31");
  }
  else {
    push("$2");
    std::cout << "add $2, $0, $0" << std::endl;
    push("$31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word init" << std::endl;
    std::cout << "jalr $5" << std::endl;
    pop("$31");
    pop("$2");
  }

  std::unordered_map<std::string, int> wain_table = gen_wain(curr_node, offset);
  offset_map["wain"] = wain_table;
  
  std::cout << "; end prologue" << std::endl << std::endl;

  code(curr_node->children[9], offset_map["wain"]);

  code(curr_node->children[11], offset_map["wain"]);

  std::cout << std::endl << "; begin epilogue" << std::endl;
  int count = offset;
  while (count != 0) {
    std::cout << "add $30, $30, $4" << std::endl;
    count += 4;
  }
  std::cout << "jr $31" << std::endl;

  // other procedures

  while (!proc_st.empty()) {
    TreeNode* proc_node = proc_st.top();
    proc_st.pop();

    std::string name = proc_node->children[1]->data[1];
    name = "F" + name;
    std::cout << name << ":" << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;

    int proc_offset = 0;

    std::unordered_map<std::string, int> tab = gen_proc(proc_node, proc_offset);
    offset_map[name] = tab;

    push("$1");
    push("$2");
    push("$5");
    push("$6");
    push("$7");

    std::cout << "; end prologue" << std::endl << std::endl;

    code(proc_node->children[7], offset_map[name]);

    code(proc_node->children[9], offset_map[name]);

    std::cout << std::endl << "; begin epilogue" << std::endl;

    pop("$7");
    pop("$6");
    pop("$5");
    pop("$2");
    pop("$1");

    int proc_count = proc_offset;
    while (proc_count != 0) {
      std::cout << "add $30, $30, $4" << std::endl;
      proc_count += 4;
    }
    std::cout << "jr $31" << std::endl;
  }

  for (int i=0;i<tree_vector.size();i++) {
    delete tree_vector[i];
  }

  return 0;
}