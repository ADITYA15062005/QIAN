#include <iostream>
#include "json.hpp"
#include <filesystem>
using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace std;

// Decode string in given base → long long
long long decodeValue(const string &s, int base) {
    long long num = 0;
    for (char c : s) {
        int digit;
        if (isdigit(c)) digit = c - '0';
        else if (isalpha(c)) digit = tolower(c) - 'a' + 10;
        else throw runtime_error("Invalid digit in value");
        if (digit >= base) throw runtime_error("Digit not valid for base");
        num = num * base + digit;
    }
    return num;
}

// Perform Lagrange interpolation at x=0 (secret c)
long long lagrangeInterpolation(const vector<pair<long long, long long>> &points, int k) {
    long long result = 0;
    int n = points.size();
    for (int i = 0; i < k; i++) {
        long double term = points[i].second;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                term *= (0.0 - points[j].first) / (long double)(points[i].first - points[j].first);
            }
        }
        result += llround(term);
    }
    return result;
}

void processTestcase(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    json j;
    file >> j;

    int n = j["keys"]["n"].get<int>();
    int k = j["keys"]["k"].get<int>();

    vector<pair<long long, long long>> points;
    for (auto &el : j.items()) {
        if (el.key() == "keys") continue;

        long long x = stoll(el.key());
        string y_str = el.value()["value"].get<string>();
        int base = stoi(el.value()["base"].get<string>());

        long long y = decodeValue(y_str, base);
        points.push_back({x, y});
    }

    sort(points.begin(), points.end()); // ensure order

    cout << "========== " << filename << " ==========\n";
    cout << "Decoded Points: ";
    for (int i = 0; i < k; i++) {
        cout << "(" << points[i].first << "," << points[i].second << ") ";
    }
    cout << "\n";

    long long secret = lagrangeInterpolation(points, k);
    cout << "Secret (c) = " << secret << "\n\n";
}

int main() {
    string folder = "."; // current directory
    for (const auto &entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".json") {
            processTestcase(entry.path().string());
        }
    }
    return 0;
}
