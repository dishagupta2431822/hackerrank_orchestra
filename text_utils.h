#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <algorithm>
#include <cctype>
#include <set>
#include <string>
#include <vector>

using namespace std;

static const set<string> STOPWORDS = {
    "a","an","and","are","as","at","be","by","for","from","has","have",
    "how","i","in","is","it","me","my","of","on","or","our","that","the",
    "this","to","we","what","when","where","who","why","with","you","your",
    "can","cannot","cant","not"
};

inline string trim(const string &s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;
    return s.substr(start, end - start);
}

inline string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return (char)tolower(c); });
    return s;
}

inline vector<string> tokenize(const string &text) {
    vector<string> tokens;
    string cur;

    for (unsigned char c : text) {
        if (isalnum(c)) {
            cur.push_back((char)tolower(c));
        } else {
            if (!cur.empty()) {
                if (!STOPWORDS.count(cur)) tokens.push_back(cur);
                cur.clear();
            }
        }
    }

    if (!cur.empty() && !STOPWORDS.count(cur)) {
        tokens.push_back(cur);
    }

    return tokens;
}

#endif