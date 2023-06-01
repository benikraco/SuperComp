#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cmath>

using namespace std;

struct Movie {
    int start, end, category, index;
};

bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

bool timeOverlap(int start1, int end1, int start2, int end2) {
    if (!((start1 >= end2) || (end1 <= start2))) {
        return true;
    }
    else {
        return false;
    }
}

vector<Movie> readMovies(string filename, int &N, int &M, vector<int> &maxMoviesPerCategory) {
    ifstream inputFile(filename);

    inputFile >> N >> M;

    maxMoviesPerCategory.resize(M);

    for (int i = 0; i < M; i++) {
        inputFile >> maxMoviesPerCategory[i];
    }

    vector<Movie> movies(N);

    for (int i = 0; i < N; i++) {
        inputFile >> movies[i].start >> movies[i].end >> movies[i].category;
        if (movies[i].end < movies[i].start) {
            movies[i].end += 24;
        }
        movies[i].index = i;
    }

    inputFile.close();
    return movies;
}

void exhaustiveSearch(vector<Movie> &movies, vector<int> &maxMoviesPerCategory, vector<Movie> &bestCombination) {
    int N = movies.size();
    int best = 0;
    vector<Movie> chosenMovies;

    for (unsigned int i = 0; i < static_cast<unsigned int>(pow(2, N)); i++) { 
        vector<int> slots(32, 0);
        vector<int> chosenMoviesPerCategory(maxMoviesPerCategory.size(), 0);

        chosenMovies.clear();
        int moviesWatched = 0;

        for (int j = 0; j < N; j++) {
            if (i & (1 << j)) {
                if (moviesWatched >= 24) {
                    break;
                }

                bool conflict = false;
                for (const Movie &chosenMovie : chosenMovies) {
                    if (timeOverlap(chosenMovie.start, chosenMovie.end, movies[j].start, movies[j].end)) {
                        conflict = true;
                        break;
                    }
                }

                if (conflict || chosenMoviesPerCategory[movies[j].category - 1] >= maxMoviesPerCategory[movies[j].category - 1]) {
                    continue;
                }

                chosenMovies.push_back(movies[j]);
                chosenMoviesPerCategory[movies[j].category - 1]++;
                moviesWatched++;
            }
        }

        if (moviesWatched > 0 && moviesWatched <= 24 && moviesWatched > best) {
            best = moviesWatched;
            bestCombination = chosenMovies;
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string input_file = argv[1];
    int N, M;
    vector<int> maxMoviesPerCategory;

    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    sort(movies.begin(), movies.end(), compareMovies);

    vector<Movie> bestCombination;

    auto startTime = chrono::steady_clock::now();

    exhaustiveSearch(movies, maxMoviesPerCategory, bestCombination);

    auto endTime = chrono::steady_clock::now();

    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    cout << "Movies watched: " << bestCombination.size() << endl;

    for (const Movie &movie : bestCombination) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    cout << "Time elapsed during the exhaustive search algorithm: " << duration << " seconds" << endl;

    return 0;
}
