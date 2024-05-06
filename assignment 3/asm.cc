#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "scanner.h"

void gen_output(int64_t i) {
  unsigned char c = i >> 24;
  std::cout << c;
  c = i >> 16;
  std::cout << c;
  c = i >> 8;
  std::cout << c;
  c = i;
  std::cout << c;
}

int main() {
  std::string line;
  try {
    std::vector<std::vector<Token>> tokenLines;
    std::unordered_map<std::string, int64_t> symbolTable;
    int lineNum = 0;

    // first pass
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      if (tokenLine.size() != 0) {
        int st = 0;
        while ((st < tokenLine.size()) && (tokenLine[st].getKind() == 1)) {
          if (symbolTable.count(tokenLine[st].getLexeme().substr(0, tokenLine[st].getLexeme().size()-1)) < 1) {
            symbolTable.insert(std::pair<std::string, int64_t>(tokenLine[st].getLexeme().substr(0, tokenLine[st].getLexeme().size()-1), lineNum*4));
          }
          else {
            throw ScanningFailure("ERROR");
          }
          st++;
        }
        if (st >= tokenLine.size()) {
          continue;
        }

        int tokKind = tokenLine[st].getKind();
        std::string tokLexme = tokenLine[st].getLexeme();

        if (tokKind == 2) {
          if ((tokenLine.size() - st) != 2) {
            throw ScanningFailure("ERROR");
          }
          if (!(tokenLine[st+1].getKind() == 6 || tokenLine[st+1].getKind() == 7 || tokenLine[st+1].getKind() == 0)) {
            throw ScanningFailure("ERROR");
          }
        }
        else if (tokKind == 0) {
          if (tokLexme == "add" || tokLexme == "sub" || tokLexme == "slt" || tokLexme == "sltu") {
            if ((tokenLine.size() - st) != 6) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8 && tokenLine[st+2].getKind() == 3 && tokenLine[st+3].getKind() == 8 && tokenLine[st+4].getKind() == 3 && tokenLine[st+5].getKind() == 8)) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tokLexme == "mult" || tokLexme == "multu" || tokLexme == "div" || tokLexme == "divu") {
            if ((tokenLine.size() - st) != 4) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8 && tokenLine[st+2].getKind() == 3 && tokenLine[st+3].getKind() == 8)) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tokLexme == "mfhi" || tokLexme == "mflo" || tokLexme == "lis") {
            if ((tokenLine.size() - st) != 2) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8)) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tokLexme == "lw" || tokLexme == "sw") {
            if ((tokenLine.size() - st) != 7) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8 && tokenLine[st+2].getKind() == 3 && (tokenLine[st+3].getKind() == 6 || tokenLine[st+3].getKind() == 7) && tokenLine[st+4].getKind() == 4 && tokenLine[st+5].getKind() == 8 && tokenLine[st+6].getKind() == 5)) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tokLexme == "beq" || tokLexme == "bne") {
            if ((tokenLine.size() - st) != 6) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8 && tokenLine[st+2].getKind() == 3 && tokenLine[st+3].getKind() == 8 && tokenLine[st+4].getKind() == 3 && (tokenLine[st+5].getKind() == 0 || tokenLine[st+5].getKind() == 6 || tokenLine[st+5].getKind() == 7))) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tokLexme == "jr" || tokLexme == "jalr") {
            if ((tokenLine.size() - st) != 2) {
              throw ScanningFailure("ERROR");
            }
            if (!(tokenLine[st+1].getKind() == 8)) {
              throw ScanningFailure("ERROR");
            }
          }
          else {
            throw ScanningFailure("ERROR");
          }
        }
        else {
          throw ScanningFailure("ERROR");
        }

        tokenLines.push_back(tokenLine);
        lineNum++;
      }
    }


    // second pass
    for (int i=0;i<tokenLines.size();i++) {
      std::vector<Token> tline = tokenLines[i];
      int st = 0;

      while ((st < tline.size()) && (tline[st].getKind() == 1)) {
        st++;
      }
      if (st >= tline.size()) {
        continue;
      }

      int tokKind = tline[st].getKind();
      std::string tokLexme = tline[st].getLexeme();

      // WORD
      if (tokKind == 2) {
        if (tline[st+1].getKind() == 6) {
          if (tline[st+1].toNumber() < -2147483648 || tline[st+1].toNumber() > 4294967295) {
            throw ScanningFailure("ERROR");
          }
          int64_t tokout = tline[st+1].toNumber();
          gen_output(tokout);
        }
        else if (tline[st+1].getKind() == 7) {
          if (tline[st+1].toNumber() > 4294967295) {
            throw ScanningFailure("ERROR");
          }
          int64_t tokout = tline[st+1].toNumber();
          gen_output(tokout);
        }
        else if (tline[st+1].getKind() == 0) {
          if (symbolTable.count(tline[st+1].getLexeme()) < 1) {
            throw ScanningFailure("ERROR");
          }
          int64_t tokout = symbolTable[tline[st+1].getLexeme()];
          gen_output(tokout);
        }
      }
      else if (tokKind == 0) {
        if (tokLexme == "add" || tokLexme == "sub" || tokLexme == "slt" || tokLexme == "sltu") {
          int64_t d = tline[st+1].toNumber();
          int64_t s = tline[st+3].toNumber();
          int64_t t = tline[st+5].toNumber();

          if (d < 0 || d > 31) {
            throw ScanningFailure("ERROR");
          }
          if (s < 0 || s > 31) {
            throw ScanningFailure("ERROR");
          }
          if (t < 0 || t > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t func = 0;
          if (tokLexme == "add") {
            func = 32;
          }
          else if (tokLexme == "sub") {
            func = 34;
          }
          else if (tokLexme == "slt") {
            func = 42;
          }
          else if (tokLexme == "sltu") {
            func = 43;
          }

          int64_t tokout = (0 << 26) | (s << 21) | (t << 16) | (d << 11) | (func & 0x7FF);
          gen_output(tokout);
        }
        else if (tokLexme == "mult" || tokLexme == "multu" || tokLexme == "div" || tokLexme == "divu") {
          int64_t s = tline[st+1].toNumber();
          int64_t t = tline[st+3].toNumber();

          if (s < 0 || s > 31) {
            throw ScanningFailure("ERROR");
          }
          if (t < 0 || t > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t func = 0;
          if (tokLexme == "mult") {
            func = 24;
          }
          else if (tokLexme == "multu") {
            func = 25;
          }
          else if (tokLexme == "div") {
            func = 26;
          }
          else if (tokLexme == "divu") {
            func = 27;
          }

          int64_t tokout = (0 << 26) | (s << 21) | (t << 16) | (func & 0xFFFF);
          gen_output(tokout);
        }
        else if (tokLexme == "mfhi" || tokLexme == "mflo" || tokLexme == "lis") {
          int64_t d = tline[st+1].toNumber();

          if (d < 0 || d > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t func = 0;
          if (tokLexme == "mfhi") {
            func = 16;
          }
          else if (tokLexme == "mflo") {
            func = 18;
          }
          else if (tokLexme == "lis") {
            func = 20;
          }

          int64_t tokout = (0 << 16) | (d << 11) | (func & 0x7FF);
          gen_output(tokout);
        }
        else if (tokLexme == "lw" || tokLexme == "sw") {
          int64_t t = tline[st+1].toNumber();
          int64_t s = tline[st+5].toNumber();

          if (s < 0 || s > 31) {
            throw ScanningFailure("ERROR");
          }
          if (t < 0 || t > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t im = 0;
          if (tline[st+3].getKind() == 6) {
            im = tline[st+3].toNumber();
            if (im < -32768 || im > 32767) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tline[st+3].getKind() == 7) {
            im = tline[st+3].toNumber();
            if (im > 65535) {
              throw ScanningFailure("ERROR");
            }
          }

          int64_t opcode = 0;
          if (tokLexme == "lw") {
            opcode = 35;
          }
          else if (tokLexme == "sw") {
            opcode = 43;
          }

          int64_t tokout = (opcode << 26) | (s << 21) | (t << 16) | (im & 0xFFFF);
          gen_output(tokout);
        }
        else if (tokLexme == "beq" || tokLexme == "bne") {
          int64_t s = tline[st+1].toNumber();
          int64_t t = tline[st+3].toNumber();

          if (s < 0 || s > 31) {
            throw ScanningFailure("ERROR");
          }
          if (t < 0 || t > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t im = 0;
          if (tline[st+5].getKind() == 6) {
            im = tline[st+5].toNumber();
            if (im < -32768 || im > 32767) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tline[st+5].getKind() == 7) {
            im = tline[st+5].toNumber();
            if (im > 65535) {
              throw ScanningFailure("ERROR");
            }
          }
          else if (tline[st+5].getKind() == 0) {
            if (symbolTable.count(tline[st+5].getLexeme()) < 1) {
              throw ScanningFailure("ERROR");
            }
            im = (symbolTable[tline[st+5].getLexeme()]/4) - (i+1);
            if (im < -32768 || im > 32767) {
              throw ScanningFailure("ERROR");
            }
          }
          int64_t opcode = 0;
          if (tokLexme == "beq") {
            opcode = 4;
          }
          else if (tokLexme == "bne") {
            opcode = 5;
          }

          int64_t tokout = (opcode << 26) | (s << 21) | (t << 16) | (im & 0xFFFF);
          gen_output(tokout);
        }
        else if (tokLexme == "jr" || tokLexme == "jalr") {
          int64_t d = tline[st+1].toNumber();

          if (d < 0 || d > 31) {
            throw ScanningFailure("ERROR");
          }

          int64_t func = 0;
          if (tokLexme == "jr") {
            func = 8;
          }
          else if (tokLexme == "jalr") {
            func = 9;
          }

          int64_t tokout = (0 << 26) | (d << 21) | (func & 0x1FFFFF);
          gen_output(tokout);
        }
        else {
          throw ScanningFailure("ERROR");
        }
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  
  return 0;
}
