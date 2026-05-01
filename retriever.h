#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "csv_utils.h"
#include "classifier.h"
#include "text_utils.h"

using namespace std;

struct Article {
    string source;
    string title;
    string url;
    string content;
    string product_area_hint;
};

inline string readFileContent(const string &path) {
    ifstream fin(path.c_str());
    if (!fin) return "";

    stringstream buffer;
    buffer << fin.rdbuf();
    return buffer.str();
}

inline string inferSource(const string &pathLC) {
    if (pathLC.find("hackerrank") != string::npos) return "hackerrank";
    if (pathLC.find("claude") != string::npos) return "claude";
    if (pathLC.find("visa") != string::npos) return "visa";
    return "unknown";
}

inline vector<Article> loadCorpus(const string &rootDir) {
    vector<Article> docs;

    vector<string> files;
    files.push_back(rootDir + "/hackerrank.txt");
    files.push_back(rootDir + "/claude.txt");
    files.push_back(rootDir + "/visa.txt");

    for (size_t i = 0; i < files.size(); i++) {
        string path = files[i];
        string content = readFileContent(path);
        if (trim(content).empty()) continue;

        string pathLC = toLowerStr(path);

        Article a;
        a.source = inferSource(pathLC);
        a.title = path;
        a.url = path;
        a.content = content;
        a.product_area_hint = classifyProductArea(content);

        docs.push_back(a);
    }

    return docs;
}

inline int relevanceScore(const Ticket &ticket,
                          const Article &article,
                          const string &requestType,
                          const string &productArea) {
    vector<string> ticketWords = tokenize(ticket.text);
    vector<string> docWords = tokenize(article.title + " " + article.content);

    unordered_map<string, int> freq;
    for (size_t i = 0; i < docWords.size(); i++) {
        freq[docWords[i]]++;
    }

    int score = 0;

    for (size_t i = 0; i < ticketWords.size(); i++) {
        string w = ticketWords[i];
        if (freq.count(w)) {
            score += 3 + (freq[w] > 3 ? 3 : freq[w]);
        }
    }

    string ticketLC = toLowerStr(ticket.text);

    if (ticketLC.find(article.source) != string::npos) score += 8;
    if (article.product_area_hint == productArea) score += 10;
    if (requestType == "billing" && article.product_area_hint == "billing") score += 8;
    if (requestType == "fraud" && article.product_area_hint == "fraud") score += 12;
    if (requestType == "account_access" && article.product_area_hint == "account_access") score += 8;

    return score;
}

inline const Article* retrieveBestDoc(const Ticket &ticket,
                                      const vector<Article> &docs,
                                      const string &requestType,
                                      const string &productArea) {
    const Article *best = NULL;
    int bestScore = -1;

    for (size_t i = 0; i < docs.size(); i++) {
        int score = relevanceScore(ticket, docs[i], requestType, productArea);
        if (score > bestScore) {
            bestScore = score;
            best = &docs[i];
        }
    }

    return best;
}

#endif