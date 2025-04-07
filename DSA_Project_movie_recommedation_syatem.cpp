#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <random>
using namespace std;

// Movie Class
class Movie {
public:
    int id;
    string title;
    string genre;
    double avgRating = 0.0;

    Movie() : id(0), title(""), genre("") {}
    Movie(int id, string title, string genre) 
        : id(id), title(title), genre(genre) {}
};

// Global Data Structures 
unordered_map<int, Movie> movies;
unordered_map<int, unordered_map<int, int>> ratings;

// Data Loading
void loadMovies() {
    ifstream file("movies.txt");
    if (!file.is_open()) {
        cerr << "\033[1;31mError: Could not open movies.txt!\033[0m" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, genre;
        
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, genre);
        
        int movieId = stoi(id);
        movies[movieId] = Movie(movieId, title, genre);
    }
    file.close();
}

void loadRatings() {
    ifstream file("ratings.txt");
    if (!file.is_open()) {
        cerr << "\033[1;31mError: Could not open ratings.txt!\033[0m" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string uid, mid, rating;
        
        getline(ss, uid, ',');
        getline(ss, mid, ',');
        getline(ss, rating);
        
        int userId = stoi(uid);
        int movieId = stoi(mid);
        ratings[userId][movieId] = stoi(rating);
    }
    file.close();
}

// ==================== Get Top Movies ====================
void getTopMovies(int topN, vector<int>& results) {
    unordered_map<int, pair<double, int>> temp;
    
    for (auto& user : ratings) {
        for (auto& mr : user.second) {
            temp[mr.first].first += mr.second;
            temp[mr.first].second++;
        }
    }

    vector<pair<int, double>> avgRatings;
    for (auto& pair : temp) {
        double avg = pair.second.first / pair.second.second;
        movies[pair.first].avgRating = avg;
        avgRatings.emplace_back(pair.first, avg);
    }

    sort(avgRatings.begin(), avgRatings.end(),
        [](auto& a, auto& b) { return a.second > b.second; });

    for (int i = 0; i < min(topN, (int)avgRatings.size()); i++) {
        results.push_back(avgRatings[i].first);
    }
}

//  DFS-Based Reordering 
void dfsReorder(vector<int>& inputMovies, vector<int>& outputMovies) {
    random_device rd;
    mt19937 g(rd());
    shuffle(inputMovies.begin(), inputMovies.end(), g);
    
    unordered_map<int, bool> visited;
    stack<int> st;

    for (int movie : inputMovies) {
        if (!visited[movie]) {
            st.push(movie);
            while (!st.empty()) {
                int current = st.top();
                st.pop();

                if (!visited[current]) {
                    visited[current] = true;
                    outputMovies.push_back(current);
                }
            }
        }
    }
}

// BFS-Based Reordering 
void bfsReorder(vector<int>& inputMovies, vector<int>& outputMovies) {
    random_device rd;
    mt19937 g(rd());
    shuffle(inputMovies.begin(), inputMovies.end(), g);
    
    unordered_map<int, bool> visited;
    queue<int> q;

    for (int movie : inputMovies) {
        if (!visited[movie]) {
            q.push(movie);
            while (!q.empty()) {
                int current = q.front();
                q.pop();

                if (!visited[current]) {
                    visited[current] = true;
                    outputMovies.push_back(current);
                }
            }
        }
    }
}

// Recommendation System
void recommendMovies(int topN, bool useDFS) {
    vector<int> topMovies;
    getTopMovies(topN, topMovies);

    if (topMovies.empty()) {
        cout << "\033[1;31mNo movies available for recommendations.\033[0m\n";
        return;
    }

    vector<int> recommendations;
    
    if (useDFS) {
        dfsReorder(topMovies, recommendations);
    } else {
        bfsReorder(topMovies, recommendations);
    }

    cout << "\n\033[1;34mRecommendations (" << (useDFS ? "DFS" : "BFS") << "):\033[0m\n";
    for (size_t i = 0; i < recommendations.size() && i < topN; i++) {
        if (movies.count(recommendations[i])) {
            Movie& m = movies[recommendations[i]];
            cout << "\033[1;32m" << m.title << "\033[0m (" << m.genre << ") [Rating: " 
                 << m.avgRating << "]\n";
        }
    }
}
//main fun
int main() {
    loadMovies();
    loadRatings();

    int choice;
    do {
        cout << "\n\033[1;36m=========== Movie Recommendation System ===========\033[0m\n";
        cout << "1. \033[1;33mShow All Movies\033[0m\n"
             << "2. \033[1;33mGet DFS Recommendations\033[0m\n"
             << "3. \033[1;33mGet BFS Recommendations\033[0m\n"
             << "4. \033[1;31mExit\033[0m\n"
             << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\n\033[1;34mAll Movies:\033[0m\n";
                for (auto& pair : movies) {
                    cout << pair.first << ". " << pair.second.title << "\n";
                }
                break;
            case 2:
                recommendMovies(5, true);
                break;
            case 3:
                recommendMovies(5, false);
                break;
            case 4:
                cout << "\033[1;31mExiting...\033[0m\n";
                break;
            default:
                cout << "\033[1;31mInvalid choice! Please try again.\033[0m\n";
        }
    } while (choice != 4);

    return 0;
}
