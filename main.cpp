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
    if(argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[0] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile(argv[1]);
    if(movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }

    // Use a map (balanced BST) to store movies sorted by name
    map<string, double> movies;
    string line, movieName;
    double movieRating;

    while(getline(movieFile, line) && parseLine(line, movieName, movieRating)){
        movies[movieName] = movieRating;
    }
    movieFile.close();

    if(argc == 2){
        // Part 1: print all movies alphabetically
        for(map<string,double>::iterator it = movies.begin(); it != movies.end(); ++it){
            cout << it->first << ", " << fixed << setprecision(1) << it->second << endl;
        }
        return 0;
    }

    // Part 2: read prefixes and search
    ifstream prefixFile(argv[2]);
    if(prefixFile.fail()){
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while(getline(prefixFile, line)){
        if(!line.empty()){
            prefixes.push_back(line);
        }
    }

    // Store best movie info for each prefix (to print at the end)
    vector<string> bestPrefixes;
    vector<string> bestNames;
    vector<double> bestRatings;

    for(int i = 0; i < (int)prefixes.size(); i++){
        string prefix = prefixes[i];

        // Use lower_bound for efficient O(log n) lookup
        map<string,double>::iterator it = movies.lower_bound(prefix);

        // Collect all movies that start with this prefix
        vector<pair<double, string>> matches;
        while(it != movies.end()){
            // Check if movie name starts with prefix
            if(it->first.substr(0, prefix.size()) == prefix){
                matches.push_back(make_pair(it->second, it->first));
                ++it;
            } else {
                break;
            }
        }

        if(matches.empty()){
            cout << "No movies found with prefix " << prefix << endl;
            continue;
        }

        // Sort by decreasing rating; if tied, alphabetical order of name
        sort(matches.begin(), matches.end(),
            [](const pair<double,string> &a, const pair<double,string> &b){
                if(a.first != b.first) return a.first > b.first;
                return a.second < b.second;
            });

        // Print all matching movies
        for(int j = 0; j < (int)matches.size(); j++){
            cout << matches[j].second << ", " << fixed << setprecision(1)
                 << matches[j].first << endl;
        }
        cout << endl;  // blank line after each prefix group

        // Best movie is first after sorting
        bestPrefixes.push_back(prefix);
        bestNames.push_back(matches[0].second);
        bestRatings.push_back(matches[0].first);
    }

    // Print best movies at the end
    for(int i = 0; i < (int)bestPrefixes.size(); i++){
        cout << "Best movie with prefix " << bestPrefixes[i] << " is: "
             << bestNames[i] << " with rating "
             << fixed << setprecision(1) << bestRatings[i] << endl;
    }

    return 0;
}

/*
 * Part 3a: Time Complexity Analysis
 *
 * Data structure chosen: std::map<string, double> (red-black tree / balanced BST),
 * with movie names as keys. This keeps movies sorted alphabetically at all times.
 *
 * Variables:
 *   n = number of movies in the dataset
 *   m = number of prefixes
 *   k = maximum number of movies that begin with any single prefix
 *   l = maximum length of a movie name
 *
 * For each of the m prefixes:
 *   Step 1 - lower_bound lookup: O(l * log n)
 *     Each comparison in the BST compares strings of length up to l.
 *     The BST has height O(log n), so we do O(log n) comparisons.
 *
 *   Step 2 - Collect k matching movies: O(k * l)
 *     We iterate through k entries, each requiring a prefix comparison of length l.
 *
 *   Step 3 - Sort k matches by rating: O(k * log k)
 *     Sorting k elements. String comparisons for tiebreaking add a factor of l
 *     in the worst case, giving O(k * l * log k).
 *
 * Total for all m prefixes: O(m * (l * log n + k * l * log k))
 * Simplified: O(m * l * (log n + k * log k))
 *
 * Runtime measurements using prefix_large.txt:
 *   input_20_random.csv:    _____ ms
 *   input_100_random.csv:   _____ ms
 *   input_1000_random.csv:  _____ ms
 *   input_76920_random.csv: _____ ms
 *
 * Part 3b: Space Complexity Analysis
 *
 * The map stores n movies, each with a name of up to l characters: O(n * l)
 * The prefix vector stores m prefixes: O(m * l)
 * For each prefix query, we create a temporary vector of up to k matches: O(k * l)
 * The best-movie vectors store at most m results: O(m * l)
 *
 * Total space: O(n * l + m * l + k * l) = O((n + m) * l)
 * Since k <= n, the k*l term is dominated by n*l.
 *
 * Part 3c: Time/Space Tradeoff Discussion
 *
 * I designed my algorithm primarily for low time complexity, targeting
 * O(m * l * (log n + k * log k)) using std::map's balanced BST.
 *
 * I was also able to achieve low space complexity of O((n + m) * l),
 * which is essentially the minimum needed to store all input data.
 * This is because std::map is inherently space-efficient compared to
 * alternatives like a Trie, which would use O(n * l * |alphabet|) space
 * but offer O(l) prefix lookup instead of O(l * log n).
 *
 * Both low time and low space were achieved because std::map naturally
 * provides efficient sorted access with minimal overhead. Neither was
 * particularly hard to achieve for this problem since the BST-based
 * approach handles both concerns well simultaneously.
 */

bool parseLine(string &line, string &movieName, double &movieRating){
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if(movieName[0] == '\"'){
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}
