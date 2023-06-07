#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <omp.h> 
#include <cmath>

using namespace std;

// Define a structure to hold the details of a movie
struct Movie {
    int start, end, category, index;
};

// Compare two movies based on their end time. If end times are equal, compare start times.
bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

// Function to check if two time intervals overlap
bool timeOverlap(int start1, int end1, int start2, int end2) {
    if (!((start1 >= end2) || (end1 <= start2))) {
        return true;
    }
    else {
        return false;
    }
}

// Read movie details from an input file
vector<Movie> readMovies(string filename, int &N, int &M, vector<int> &maxMoviesPerCategory) {
    ifstream inputFile(filename);

    // Read number of movies and categories
    inputFile >> N >> M;

    maxMoviesPerCategory.resize(M);

    // Read maximum number of movies allowed per category
    for (int i = 0; i < M; i++) {
        inputFile >> maxMoviesPerCategory[i];
    }

    vector<Movie> movies(N);

    // Read movie details
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

// Function to execute an exhaustive search for the best combination of movies to watch
void exhaustiveSearch(vector<Movie> &movies, vector<int> &maxMoviesPerCategory, vector<Movie> &bestCombination) {
    int N = movies.size();
    int n_threads = omp_get_max_threads();
    vector<int> bests(n_threads, 0);
    vector<vector<Movie>> bestCombinations(n_threads);

    // Start parallel execution
    #pragma omp parallel
    {
        int best = 0;
        vector<Movie> chosenMovies;

        // Iterate through all possible combinations of movies
        #pragma omp for
        for (unsigned int i = 0; i < static_cast<unsigned int>(pow(2, N)); i++) { 
            vector<int> slots(32, 0);
            vector<int> chosenMoviesPerCategory(maxMoviesPerCategory.size(), 0);

            chosenMovies.clear();
            int moviesWatched = 0;

            // Check each movie to see if it can be added to the current combination
            for (int j = 0; j < N; j++) {
                if (i & (1 << j)) {
                    if (moviesWatched >= 24) {
                        break;
                    }

                    // Check for time conflict with already chosen movies
                    bool conflict = false;
                    for (const Movie &chosenMovie : chosenMovies) {
                        if (timeOverlap(chosenMovie.start, chosenMovie.end, movies[j].start, movies[j].end)) {
                            conflict = true;
                            break;
                        }
                    }

                    // Skip current movie if there is a time conflict or if the maximum number of movies for its category has been reached
                    if (conflict || chosenMoviesPerCategory[movies[j].category - 1] >= maxMoviesPerCategory[movies[j].category - 1]) {
                        continue;
                    }

                    // Add current movie to chosen movies
                    chosenMovies.push_back(movies[j]);
                    chosenMoviesPerCategory[movies[j].category - 1]++;
                    moviesWatched++;
                }
            }

            // Update best combination for current thread if current combination is better
            if (moviesWatched > 0 && moviesWatched <= 24 && moviesWatched > best) {
                best = moviesWatched;
                bestCombinations[omp_get_thread_num()] = chosenMovies;
                bests[omp_get_thread_num()] = best;
            }
        }
    }

    // Find the best combination among all threads
    int bestMoviesWatched = 0;
    for (int i = 0; i < n_threads; i++) {
        if (bests[i] > bestMoviesWatched) {
            bestMoviesWatched = bests[i];
            bestCombination = bestCombinations[i];
        }
    }
}


// Main function
int main(int argc, char* argv[]) {

    // Check if correct number of arguments is provided
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    // Read movies from input file
    string input_file = argv[1];
    int N, M;
    vector<int> maxMoviesPerCategory;

    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    // Sort movies based on their end time
    sort(movies.begin(), movies.end(), compareMovies);

    vector<Movie> bestCombination;

    // Start timer
    auto startTime = chrono::steady_clock::now();

    // Execute exhaustive search for the best combination of movies
    exhaustiveSearch(movies, maxMoviesPerCategory, bestCombination);

    // End timer and calculate duration
    auto endTime = chrono::steady_clock::now();

    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    // Print results
    cout << "Movies watched: " << bestCombination.size() << endl;

    for (const Movie &movie : bestCombination) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    cout << "Time elapsed during the exhaustive search algorithm: " << duration << " microseconds" << endl;

    return 0;
}
