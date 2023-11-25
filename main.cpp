/*

Name :- Siddharth Nahar
Entry No :- 2016csb1043
Date :- 16/2/19
Purpose :- 

        1. Produce FIRST and FOLLOW Sets of grammer.
        2. Check if Grammer can be parsed by LL(1)
        3. Can be extended in RD
*/


/*

        Check for This Grammer :- 

			S -> ACB | Cbb | Ba
			A -> da | BC
			B -> g | ~
			C -> h | ~

		FIRST-SET :- 
		
			FIRST(S) = { d, g, h, Є, b, a}
			FIRST(A) = { d, g , h, ~ }
			FIRST(B) = { g , ~ }
			FIRST(C) = { h , ~ }
		
		FOLLOW-SET :- 
		
			FOLLOW(S) = { $ }
			FOLLOW(A)  = { h, g, $ }
			FOLLOW(B) = { a, $, h, g }
			FOLLOW(C) = { b, g, $, h }

*/

#include <bits/stdc++.h>

using namespace std;

// K for LL(K) Grammar
const int K = 1;

void MakeUnique(vector<string> &a){
    sort(a.begin(), a.end());
    a.resize(unique(a.begin(), a.end()) - a.begin());
}

bool CheckEmptyIntersection(vector<string> a, vector<string> b){
    for (const string& s : a){
        for (const string& t : b){
            if (s == t){
                return false;
            }
        }
    }
    return true;
}

vector<string>UnionSets(vector<string> a, vector<string> b){
    vector<string> ans;
    for (const string& s : a){
        ans.push_back(s);
    }
    for (const string& s : b){
        ans.push_back(s);
    }
    MakeUnique(ans);
    return ans;
}

vector<string> UnionSetsUnderK(vector<string> a, vector<string> b){
    map<string, int> mp;
    vector<string> ans;
    for (const string& s : a){
        for (const string& t : b){
            string temp = s+t;
            while(temp.size() > K) {
                temp.pop_back();
            }
            if (!mp[temp]){
                mp[temp] = 1;
                ans.push_back(temp);
            }
        }
    }
    return ans;
}

struct NonTerminal
{
    char name;
    bool DerivesEmpty;
    vector<string> productions;
    vector<string> first;
    vector<string> follow;
};

NonTerminal nonTerminals[26];
int NumberOfNonTerminals;
char InitialState;

NonTerminal getNonTerminal(char c)
{
    return nonTerminals[c - 'A'];
}

bool isNonTerminal(char c)
{
    return (c >= 'A' && c <= 'Z');
}

void ReadGrammar()
{
    freopen("input.txt", "r", stdin);
    cin >> NumberOfNonTerminals >> InitialState;
    cin.ignore();
    for (int i = 0; i < NumberOfNonTerminals; i++)
    {
        string line = "";
        getline(cin, line);
        NonTerminal Current;
        Current.name = line[0];
        for (int j = 3; j < line.size(); j++)
        {
            string production = "";
            while (j < line.size() && line[j] != '|')
            {
                production += line[j];
                j++;
            }
            if (production=="^"){
                production = "";
            }
            Current.productions.push_back(production);
        }
        if (Current.name== InitialState)
        {
         //   Current.productions.push_back("");
        }
        MakeUnique(Current.productions);
        nonTerminals[Current.name - 'A'] = Current;
    }
}

bool CheckWordDerivesEmpty(string s)
{
    for (int i = 0; i < s.size(); i++)
    {
        if (!isNonTerminal(s[i]))
        {
            return false;
        }
        else if (!nonTerminals[s[i] - 'A'].DerivesEmpty)
        {
            return false;
        }
    }
    return true;
}

void FindEmptyNonTerminals()
{
    for (int i = 0; i < 26; i++)
    {
        nonTerminals[i].DerivesEmpty = false;
    }
    while(true) {
        bool changed = false;
        for (int i = 0; i < 26; i++) {
            if (nonTerminals[i].DerivesEmpty) {
                continue;
            }
            for (const string &production: nonTerminals[i].productions) {
                if (CheckWordDerivesEmpty(production)) {
                    nonTerminals[i].DerivesEmpty = true;
                    changed = true;
                    break;
                }
            }
        }
        if (!changed) {
            break;
        }
    }
}

void CalcFirstK()
{
    while(true){
        vector<string>PrevFirst[26];
        for (int i = 0; i < 26; i++)
        {
            PrevFirst[i] = nonTerminals[i].first;
        }
        for (int i = 0; i < 26; i++)
        {
            NonTerminal Current = nonTerminals[i];
            for (const string& production : Current.productions)
            {
                vector<string>result= {""};
                for (int j=0;j<production.size();j++){
                    if (isNonTerminal(production[j])){
                        result = UnionSetsUnderK(result, PrevFirst[production[j] - 'A']);
                    }
                    else{
                        result = UnionSetsUnderK(result, vector<string>{string(1, production[j])});
                    }
                }
                nonTerminals[i].first = UnionSets(nonTerminals[i].first, result);
            }
            MakeUnique(nonTerminals[i].first);
        }
        for (int i = 0; i < 26; i++)
        {
            if (PrevFirst[i] != nonTerminals[i].first){
                break;
            }
            if (i == 25){
                return;
            }
        }
    }
}

void CalcFollowK()
{
    nonTerminals[InitialState - 'A'].follow.push_back("");
    while(true) {
        vector<string> PrevFollow[26];
        for (int i = 0; i < 26; i++) {
            PrevFollow[i] = nonTerminals[i].follow;
        }
        for (int i = 0; i < 26; i++) {
            NonTerminal Current = nonTerminals[i];
            for (const string &production: Current.productions) {
                for (int j = 0; j < production.size(); j++) {
                    if (isNonTerminal(production[j])) {
                        vector<string> result = {""};
                        for (int k = j + 1; k < production.size(); k++) {
                            if (isNonTerminal(production[k])) {
                                result = UnionSetsUnderK(result, nonTerminals[production[k] - 'A'].first);
                            } else {
                                result = UnionSetsUnderK(result, vector<string>{string(1, production[k])});
                            }
                        }
                        result = UnionSetsUnderK(result, PrevFollow[i]);
                        nonTerminals[production[j] - 'A'].follow = UnionSets(nonTerminals[production[j] - 'A'].follow,result);
                    }
                }
            }
        }
        for (int i = 0; i < 26; i++) {
            MakeUnique(nonTerminals[i].follow);
            if (PrevFollow[i] != nonTerminals[i].follow) {
                break;
            }
            if (i == 25) {
                return;
            }
        }
    }
}

vector<string> GetFirstKWord(string s){
    vector<string> result = {""};
    for (int i = 0; i < s.size(); i++){
        if (isNonTerminal(s[i])){
            result = UnionSetsUnderK(result, nonTerminals[s[i] - 'A'].first);
        }
        else{
            result = UnionSetsUnderK(result, vector<string>{string(1, s[i])});
        }
    }
    return result;
}

bool CheckLL1Grammar()
{
    for (int i = 0; i < 26; i++)
    {
        NonTerminal Current = nonTerminals[i];
        for (int j = 0; j < Current.productions.size(); j++)
        {
            for (int k = j + 1; k < Current.productions.size(); k++)
            {
                if (!CheckEmptyIntersection(GetFirstKWord(Current.productions[j]), GetFirstKWord(Current.productions[k])))
                {
                    return false;
                }
            }
        }
        for (int j = 0; j < Current.productions.size(); j++)
        {
            if (CheckWordDerivesEmpty(Current.productions[j]))
            {
                for (int k = 0; k < Current.productions.size(); k++)
                {
                    if (k != j && !CheckEmptyIntersection(GetFirstKWord(Current.productions[k]), Current.follow))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

map<char,int>ControlTable[26];


/// Make LL Table
void MakeLLTable()
{
    for (int i = 0; i < 26; i++)
    {
        for (int j=0;j<nonTerminals[i].productions.size();j++){
            vector<string> Cur= GetFirstKWord(nonTerminals[i].productions[j]);
            Cur= UnionSetsUnderK(Cur, nonTerminals[i].follow);
            for (const string& s : Cur){
                if (s == ""){
                    ControlTable[i]['\0'] = j;
                }
                else{
                    ControlTable[i][s[0]] = j;
                }
            }
        }
    }
}

struct ASTNode {
    string value;
    vector<ASTNode*> children;
};

void ReverseAST(ASTNode* root, int depth){
    reverse(root->children.begin(), root->children.end());
    for (ASTNode* child : root->children){
        ReverseAST(child, depth+1);
    }
}

pair<pair<bool,string>,ASTNode*> CheckWord(string Word)
{
    ASTNode* root = new ASTNode();
    root->value = InitialState;
    vector<ASTNode*> NodeStack;
    NodeStack.push_back(root);
    deque<char> Stack;
    Stack.push_back(InitialState);
    for (int i=0;i<Word.size();i++){
        char c= Word[i];
        if (Stack.empty()){
            return {{false, "Stack is empty"}, root};
        }
        while(Stack.size()>0 && isNonTerminal(Stack.back())){
            char top = Stack.back();
            Stack.pop_back();
            if (ControlTable[top - 'A'].find(c) == ControlTable[top - 'A'].end()){
                return {{false, "No entry in table"}, root};
            }
            int index = ControlTable[top - 'A'][c];
            ASTNode* Current = NodeStack.back();
            NodeStack.pop_back();
            if (nonTerminals[top - 'A'].productions[index] == ""){
                ASTNode* NewNode = new ASTNode();
                NewNode->value = '^';
                Current->children.push_back(NewNode);
                continue;
            }
            for (int j=nonTerminals[top - 'A'].productions[index].size()-1;j>=0;j--){
                ASTNode* NewNode = new ASTNode();
                NewNode->value = nonTerminals[top - 'A'].productions[index][j];
                Current->children.push_back(NewNode);
                NodeStack.push_back(Current->children.back());
                Stack.push_back(nonTerminals[top - 'A'].productions[index][j]);
            }
        }
        if (Stack.empty()) {
            return {{false, "Stack is empty"}, root};
        }
        if (Stack.back() == c){
            Stack.pop_back();
            NodeStack.pop_back();
        }
        else{
            return {{false, "Top of stack does not match"}, root};
        }
    }
    while(Stack.size()>0 && isNonTerminal(Stack.back())){
        char top = Stack.back();
        Stack.pop_back();
        if (ControlTable[top - 'A'].find('\0') == ControlTable[top - 'A'].end()){
            return {{false, "No entry in table"}, root};
        }
        int index = ControlTable[top - 'A']['\0'];
        ASTNode* Current = NodeStack.back();
        NodeStack.pop_back();
        ASTNode* NewNode = new ASTNode();
        NewNode->value = '^';
        Current->children.push_back(NewNode);
    }
    if (Stack.empty()){
        return {{true, "Success"}, root};
    }
    else{
        return {{false, "Stack is not empty"}, root};
    }
}

pair<string,ASTNode*> GetAst(string Word){
    pair<pair<bool,string>,ASTNode*> result = CheckWord(Word);
    ReverseAST(result.second, 0);
    return {result.first.second, result.second};
}

#include "libs/json-develop/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

json ConvertNodeToJson(ASTNode* node) {
    json jsonNode;
    jsonNode["value"] = node->value;

    if (!node->children.empty()) {
        jsonNode["children"] = json::array();
        for (ASTNode* child : node->children) {
            jsonNode["children"].push_back(ConvertNodeToJson(child));
        }
    }

    return jsonNode;
}

std::string ConvertAstToJson(ASTNode* root) {
    json astJson = ConvertNodeToJson(root);
    return astJson.dump();
}

#include "libs/cpp-httplib/httplib.h"

int main(){
    ReadGrammar();
    FindEmptyNonTerminals();
    CalcFirstK();
    CalcFollowK();
    MakeLLTable();
//    string word="abb";
//    pair<string,ASTNode*> RES = GetAst(word);
//    cout<<RES.first<<endl;
//    cout<<ConvertAstToJson(RES.second)<<endl;
//    exit(0);
    httplib::Server svr;
    // Add this code to handle word submission
    svr.Post("/check_word", [&](const httplib::Request& req, httplib::Response& res) {
        // Extract the word from the request JSON
        std::string word = req.body;

        // Process the word and obtain AST data
        pair<string,ASTNode*> RES = GetAst(word);

        // Convert AST data to JSON format
        // You need to implement a function for this conversion
        std::string astJson = ConvertAstToJson(RES.second);

        json jsonNode;
        jsonNode["ast"]=json::parse(astJson);
        jsonNode["message"]=RES.first;
        string jsonRes=jsonNode.dump();

        // Set the response content type and send the AST data
        res.set_content(jsonRes, "application/json");
    });

    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        // Перевірка на LL(1)
        if (CheckLL1Grammar()) {
            MakeLLTable();
            // Завантаження вмісту HTML-файлу "index.html"
            std::ifstream t("website/index.html");
            if (t.good()) {
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                // Встановлення HTML-вмісту відповіді
                res.set_content(str, "text/html");
            } else {
                // Якщо файл не знайдено, надсилаємо відповідь з кодом помилки
                res.status = 404;
                res.set_content("File not found", "text/plain");
            }
        } else {
            // Граматика не є LL(1), встановлення текстового вмісту відповіді
            res.set_content("Not LL(1) Grammar", "text/plain");
        }
    });
    svr.Get("/script.js", [&](const httplib::Request&, httplib::Response& res) {
        // Відкрийте файл script.js та відправте вміст як відповідь
        std::ifstream t("website/script.js");
        if (t.good()) {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            res.set_content(str, "application/javascript");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    svr.Get("/style.css", [&](const httplib::Request&, httplib::Response& res) {
        // Відкрийте файл style.css та відправте вміст як відповідь
        std::ifstream t("website/style.css");
        if (t.good()) {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            res.set_content(str, "text/css");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });
    svr.listen("localhost", 12345);
}