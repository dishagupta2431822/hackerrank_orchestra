#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "text_utils.h"

using namespace std;

struct Ticket {
    string ticket_id;
    string text;
    unordered_map<string, string> raw;
};

inline vector<string> parseCSVRow(const string &line) {
    vector<string> row;
    string cur;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                cur.push_back('"');
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            row.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }

    row.push_back(cur);
    return row;
}

inline vector<Ticket> readTicketsCSV(const string &path) {
    ifstream fin(path);
    if (!fin) {
        throw runtime_error("Unable to open CSV file: " + path);
    }

    string headerLine;
    getline(fin, headerLine);
    vector<string> headers = parseCSVRow(headerLine);

    vector<Ticket> tickets;
    string line;
    int autoId = 1;

    while (getline(fin, line)) {
        if (trim(line).empty()) continue;

        vector<string> cols = parseCSVRow(line);
        Ticket t;

        for (size_t i = 0; i < headers.size() && i < cols.size(); i++) {
            t.raw[trim(headers[i])] = cols[i];
        }

        if (t.raw.count("ticket_id")) t.ticket_id = t.raw["ticket_id"];
        else if (t.raw.count("id")) t.ticket_id = t.raw["id"];
        else t.ticket_id = to_string(autoId);

        if (t.raw.count("text")) t.text = t.raw["text"];
        else if (t.raw.count("issue")) t.text = t.raw["issue"];
        else if (t.raw.count("ticket")) t.text = t.raw["ticket"];
        else if (t.raw.count("description")) t.text = t.raw["description"];
        else {
            string merged;
            for (auto &kv : t.raw) merged += kv.second + " ";
            t.text = trim(merged);
        }

        tickets.push_back(t);
        autoId++;
    }

    return tickets;
}

#endif