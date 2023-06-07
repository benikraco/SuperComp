#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cmath>

using namespace std;

// Define the Movie structure
struct Movie {
    int start, end, category, index;
};

// Compare movies based on their end time and start time
bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

// Check if two time intervals overlap
bool timeOverlap(int start1, int end1, int start2, int end2) {
    if (!((start1 >= end2) || (end1 <= start2))) {
        return true;
    }
    else {
        return false;
    }
}

// Read the movie information from a file
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

// Perform exhaustive search for the optimal movie schedule
void exhaustiveSearch(vector<Movie> &movies, vector<int> &maxMoviesPerCategory, vector<Movie> &bestCombination) {
    int N = movies.size();
    int best = 0;
    vector<Movie> chosenMovies;

    
    for (unsigned int i = 0; i < static_cast<unsigned int>(pow(2, N)); i++) { 
        vector<int> slots(32, 0); // 32 slots for 24 hours (in case there is a movie that starts at 23 and ends at 1)
        vector<int> chosenMoviesPerCategory(maxMoviesPerCategory.size(), 0);

        chosenMovies.clear(); // Clear the chosen movies vector
        int moviesWatched = 0;

        // Check if the current combination of movies is valid
        for (int j = 0; j < N; j++) {
            if (i & (1 << j)) {
                if (moviesWatched >= 24) {
                    break;
                }

                bool conflict = false; // Check if there is a time conflict with the chosen movies
                for (const Movie &chosenMovie : chosenMovies) {
                    if (timeOverlap(chosenMovie.start, chosenMovie.end, movies[j].start, movies[j].end)) {
                        conflict = true;
                        break;
                    }
                }

                // Check if the maximum number of movies per category has been reached
                if (conflict || chosenMoviesPerCategory[movies[j].category - 1] >= maxMoviesPerCategory[movies[j].category - 1]) {
                    continue;
                }

                // Add the movie to the chosen movies vector
                chosenMovies.push_back(movies[j]);
                chosenMoviesPerCategory[movies[j].category - 1]++;
                moviesWatched++;
            }
        }

        // Check if the current combination of movies is better than the previous best combination
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

    // Read the movies from the file
    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    // Sort the movies by their end time and start time
    sort(movies.begin(), movies.end(), compareMovies);

    vector<Movie> bestCombination;

    // Start the timer
    auto startTime = chrono::steady_clock::now();

    // Perform the exhaustive search
    exhaustiveSearch(movies, maxMoviesPerCategory, bestCombination);

    // Stop the timer
    auto endTime = chrono::steady_clock::now();

    // Calculate the duration of the search
    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    cout << "Movies watched: " << bestCombination.size() << endl;

    for (const Movie &movie : bestCombination) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    cout << "Time elapsed during the exhaustive search algorithm: " << duration << " microseconds" << endl;

    return 0;
}