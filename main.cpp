#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "csv_utils.h"
#include "classifier.h"
#include "retriever.h"
#include "writer.h"

using namespace std;

inline string safeSnippet(const string &content) {
    string s = trim(content);

    for (char &c : s) {
        if (c == '\n' || c == '\r') c = ' ';
    }

    if (s.size() > 220) {
        s = s.substr(0, 220) + "...";
    }

    return s;
}

inline string generateResponse(const Ticket &ticket,
                               const Article *doc,
                               bool escalate) {
    if (escalate) {
        if (doc) {
            return "This issue should be escalated to a human support team. "
                   "The closest matching support document is '" + doc->title +
                   "' and it can be used as reference while routing the case.";
        }

        return "This issue should be escalated to a human support team because "
               "it is sensitive, high-risk, or not safely answerable from the available corpus.";
    }

    if (!doc) {
        return "I could not find enough grounded support documentation to answer safely, so this case should be escalated.";
    }

    return "Based on the closest support documentation, please review '" +
           doc->title + "'. Grounded guidance: " + safeSnippet(doc->content);
}

int main(int argc, char *argv[]) {
    string ticketsCSV = (argc > 1) ? argv[1] : "support_issues/support_issues.csv";
    string corpusDir  = (argc > 2) ? argv[2] : "data";
    string outputCSV  = (argc > 3) ? argv[3] : "output.csv";
    string logPath    = (argc > 4) ? argv[4] : "log.txt";

    try {
        vector<Ticket> tickets = readTicketsCSV(ticketsCSV);
        vector<Article> docs = loadCorpus(corpusDir);
        vector<Prediction> predictions;

        for (const auto &ticket : tickets) {
            string requestType = classifyRequestType(ticket.text);
            string productArea = classifyProductArea(ticket.text);
            bool escalate = shouldEscalate(requestType, productArea, ticket.text);

            const Article *bestDoc = retrieveBestDoc(ticket, docs, requestType, productArea);

            bool finalEscalate = escalate || (bestDoc == nullptr);
            string response = generateResponse(ticket, bestDoc, finalEscalate);

            Prediction p;
            p.ticket_id = ticket.ticket_id;
            p.request_type = requestType;
            p.product_area = productArea;
            p.action = finalEscalate ? "escalate" : "reply";
            p.relevant_doc = bestDoc ? bestDoc->url : "";
            p.response = response;

            predictions.push_back(p);
        }

        writeOutputCSV(outputCSV, predictions);
        writeLogFile(logPath, predictions);

        cout << "Generated " << outputCSV
             << " and " << logPath
             << " for " << predictions.size()
             << " tickets." << endl;
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}