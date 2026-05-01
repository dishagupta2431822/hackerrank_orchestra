#ifndef WRITER_H
#define WRITER_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Prediction {
    string ticket_id;
    string request_type;
    string product_area;
    string action;
    string relevant_doc;
    string response;
};

inline string escapeCSV(string s) {
    size_t pos = 0;
    while ((pos = s.find('"', pos)) != string::npos) {
        s.insert(pos, 1, '"');
        pos += 2;
    }
    return "\"" + s + "\"";
}

inline void writeOutputCSV(const string &path, const vector<Prediction> &rows) {
    ofstream out(path);
    out << "ticket_id,request_type,product_area,reply_or_escalate,relevant_doc,response\n";

    for (const auto &r : rows) {
        out << escapeCSV(r.ticket_id) << ","
            << escapeCSV(r.request_type) << ","
            << escapeCSV(r.product_area) << ","
            << escapeCSV(r.action) << ","
            << escapeCSV(r.relevant_doc) << ","
            << escapeCSV(r.response) << "\n";
    }
}

inline void writeLogFile(const string &path, const vector<Prediction> &rows) {
    ofstream out(path);

    for (const auto &r : rows) {
        out << "ticket_id=" << r.ticket_id << "\n";
        out << "request_type=" << r.request_type << "\n";
        out << "product_area=" << r.product_area << "\n";
        out << "action=" << r.action << "\n";
        out << "relevant_doc=" << r.relevant_doc << "\n";
        out << "response=" << r.response << "\n";
        out << "\n";
    }
}

#endif