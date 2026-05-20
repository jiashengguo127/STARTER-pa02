// Spring'22
// Instructor: Diba Mirza
// Student name:
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <map>
#include "movies.h"
#include "utilities.h"
using namespace std;

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[0] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile(argv[1]);
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }

    map<string, double> movies;
    string line, movieName;
    double movieRating;

    while (getline(movieFile, line) && parseLine(line, movieName, movieRating)){
        movies[movieName] = movieRating;
    }
    movieFile.close();

    if (argc == 2){
        for (const auto& p : movies){
            cout << p.first << ", " << fixed << setprecision(1) << p.second << endl;
        }
        return 0;
    }

    ifstream prefixFile(argv[2]);
    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline(prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }

    vector<tuple<string, string, double>> bestResults;

    for (const auto& prefix : prefixes) {
        auto it = movies.lower_bound(prefix);
        vector<pair<double, string>> matches;

        while (it != movies.end() &&
               it->first.size() >= prefix.size() &&
               it->first.compare(0, prefix.size(), prefix) == 0) {
            matches.push_back({it->second, it->first});
            ++it;
        }

        if (matches.empty()) {
            cout << "No movies found with prefix " << prefix << endl;
            continue;
        }

        sort(matches.begin(), matches.end(),
            [](const pair<double,string>& a, const pair<double,string>& b){
                if (a.first != b.first) return a.first > b.first;
                return a.second < b.second;
            });

        for (const auto& m : matches){
            cout << m.second << ", " << fixed << setprecision(1) << m.first << endl;
        }
        cout << endl;

        bestResults.emplace_back(prefix, matches[0].second, matches[0].first);
    }

    for (size_t i = 0; i < bestResults.size(); i++) {
        cout << "Best movie with prefix " << get<0>(bestResults[i]) << " is: "
             << get<1>(bestResults[i]) << " with rating "
             << fixed << setprecision(1) << get<2>(bestResults[i]) << endl;
    }

    return 0;
}

/*
 * Part 3a: Time Complexity
 * Data structure: std::map (red-black tree) keyed by movie name.
 * n=movies, m=prefixes, k=max movies per prefix, l=max name length
 * Per prefix: lower_bound O(l*log n), iterate O(k*l), sort O(k*l*log k)
 * Total: O(m * l * (log n + k * log k))
 *
 * Runtimes (prefix_large.txt):
 *   input_20_random.csv:    _____ ms
 *   input_100_random.csv:   _____ ms
 *   input_1000_random.csv:  _____ ms
 *   input_76920_random.csv: _____ ms
 *
 * Part 3b: Space Complexity
 * O((n+m)*l)
 *
 * Part 3c: Designed for low time using std::map balanced BST.
 * Achieved both low time and space naturally.
 */

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}
