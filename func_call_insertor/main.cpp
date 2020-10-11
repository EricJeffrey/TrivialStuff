
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

/**
使用正则匹配形如: `void return_void() {` 的子串
之后将最后一个花括号之前的内容作为log，插入到文件中

例如，源文件:
```
void return_void(const int &x, const char *argv[]){ return; }

int main(){return 0;}
```
使用`logger()`操作后变为:
```
void return_void(const int &x, const char *argv[]){ logger(“void return_void(const int &x, const
char *argv[])”); return; }

int main(){logger("int main()"); return 0;}
```
*/

using std::string, std::cout, std::endl, std::cerr, std::vector;

static const string pattern("\\w+\\s+\\w+\\s*\\([\\w\\s\\*\\[\\],&]*\\)\\s*(\\w+\\s+)*\\{\\s*");
static const string insertFuncName("logger");

// 寻找匹配并插入logger
string insertLog(const string &line) {
    using std::regex, std::regex_search, std::smatch;
    smatch results;
    static regex r(pattern);
    if (regex_search(line, results, r)) {
        string matchRes = results.str();
        int matchEdPos = line.find(matchRes) + matchRes.size();
        int lastBracePos = matchRes.find('{') - 1;
        // dont want space
        while (matchRes[lastBracePos] == ' ')
            lastBracePos--;
        return line.substr(0, matchEdPos) + insertFuncName + "(\"" +
               matchRes.substr(0, lastBracePos + 1) + "\");" + line.substr(matchEdPos);
    } else {
        return line;
    }
}

string work(const string &filePath) {
    using std::ofstream, std::ifstream;
    const string outFileName = "_tmp_ins_func_call_" + filePath;
    ifstream in(filePath);
    ofstream out(outFileName);
    while (in) {
        string line;
        std::getline(in, line);
        string updateLine = insertLog(line) + "\n";
        out.write(updateLine.c_str(), updateLine.size());
    }
    in.close();
    out.close();
    return outFileName;
}

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        cout << "Insert `logger(\"func signature\")` into every function "
                "call, constructor/decronstructor not supported currently."
             << endl
             << endl
             << "Usage: " << argv[0] << " file" << endl;
        return 0;
    } else {
        try {
            const string res = work(argv[1]);
            cout << "result -> " << res << endl;
        } catch (const std::exception &e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
    return 0;
}
