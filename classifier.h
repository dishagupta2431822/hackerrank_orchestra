#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <string>
#include "text_utils.h"

using namespace std;

inline string classifyRequestType(const string &text) {
    string t = toLowerStr(text);

    if (t.find("fraud") != string::npos ||
        t.find("unauthorized") != string::npos ||
        t.find("stolen") != string::npos ||
        t.find("scam") != string::npos) {
        return "fraud";
    }

    if (t.find("refund") != string::npos ||
        t.find("charged") != string::npos ||
        t.find("billing") != string::npos ||
        t.find("invoice") != string::npos ||
        t.find("payment") != string::npos ||
        t.find("subscription") != string::npos) {
        return "billing";
    }

    if (t.find("bug") != string::npos ||
        t.find("error") != string::npos ||
        t.find("crash") != string::npos ||
        t.find("not working") != string::npos ||
        t.find("failed") != string::npos) {
        return "bug";
    }

    if (t.find("permission") != string::npos ||
        t.find("access") != string::npos ||
        t.find("login") != string::npos ||
        t.find("password") != string::npos ||
        t.find("locked") != string::npos) {
        return "account_access";
    }

    if (t.find("how do i") != string::npos ||
        t.find("how to") != string::npos ||
        t.find("where") != string::npos ||
        t.find("can i") != string::npos ||
        t.find("what is") != string::npos) {
        return "faq";
    }

    return "general";
}

inline string classifyProductArea(const string &text) {
    string t = toLowerStr(text);

    if (t.find("assessment") != string::npos ||
        t.find("test") != string::npos ||
        t.find("question") != string::npos ||
        t.find("exam") != string::npos ||
        t.find("candidate") != string::npos) {
        return "assessments";
    }

    if (t.find("billing") != string::npos ||
        t.find("refund") != string::npos ||
        t.find("charge") != string::npos ||
        t.find("invoice") != string::npos ||
        t.find("subscription") != string::npos) {
        return "billing";
    }

    if (t.find("fraud") != string::npos ||
        t.find("unauthorized") != string::npos ||
        t.find("scam") != string::npos ||
        t.find("dispute") != string::npos) {
        return "fraud";
    }

    if (t.find("login") != string::npos ||
        t.find("password") != string::npos ||
        t.find("account") != string::npos ||
        t.find("access") != string::npos ||
        t.find("permission") != string::npos) {
        return "account_access";
    }

    if (t.find("card") != string::npos ||
        t.find("payment") != string::npos ||
        t.find("transaction") != string::npos ||
        t.find("visa") != string::npos) {
        return "payments";
    }

    return "general";
}

inline bool shouldEscalate(const string &requestType,
                           const string &productArea,
                           const string &text) {
    string t = toLowerStr(text);

    if (requestType == "fraud") return true;

    if (t.find("human") != string::npos ||
        t.find("agent") != string::npos ||
        t.find("representative") != string::npos) {
        return true;
    }

    if (t.find("legal") != string::npos ||
        t.find("lawsuit") != string::npos ||
        t.find("police") != string::npos) {
        return true;
    }

    if (t.find("cannot access") != string::npos ||
        t.find("can't access") != string::npos ||
        t.find("locked out") != string::npos ||
        t.find("2fa") != string::npos ||
        t.find("otp") != string::npos) {
        return true;
    }

    if (productArea == "billing" &&
        (t.find("dispute") != string::npos ||
         t.find("charged twice") != string::npos ||
         t.find("double charge") != string::npos)) {
        return true;
    }

    return false;
}

#endif