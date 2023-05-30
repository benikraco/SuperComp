#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <omp.h> 

using namespace std;

// Define a structure for Movie which holds its start time, end time, category, and index
struct Movie {
    int start, end, category, index;
};

// Define function prototypes for comparing movies, checking time overlap, reading movies from a file, and the main function for exhaustive search

// Function to compare two movies based on their end time and start time (if end times are equal)
bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

// Function to check if two time periods overlap
bool timeOverlap(int start1, int end1, int start2, int end2) {
    if (!((start1 >= end2) || (end1 <= start2))) {
        return true;
    }
    else {
        return false;
    }
}

// Function to read movies from an input file and store them into a vector of Movie objects
vector<Movie> readMovies(string filename, int &N, int &M, vector<int> &maxMoviesPerCategory) {
    ifstream inputFile(filename);

    // Reading the number of movies and categories from the input file
    inputFile >> N >> M;

    // Resizing the maxMoviesPerCategory vector as per the number of categories
    maxMoviesPerCategory.resize(M);

    // Reading the maximum number of movies per category
    for (int i = 0; i < M; i++) {
        inputFile >> maxMoviesPerCategory[i];
    }

    // Initializing the movies vector with N number of movies
    vector<Movie> movies(N);

    // Reading the movie information from the file
    for (int i = 0; i < N; i++) {
        inputFile >> movies[i].start >> movies[i].end >> movies[i].category;
        if (movies[i].end < movies[i].start) {
            movies[i].end += 24;
        }
        movies[i].index = i;
    }

    // Closing the input file
    inputFile.close();
    return movies;
}

// Implementation of exhaustive search using OpenMP to parallelize the task
void exhaustiveSearch(vector<Movie> &movies, vector<int> &maxMoviesPerCategory, vector<Movie> &bestCombination) {
    int maxMoviesWatched = 0;
    vector<Movie> chosenMovies;
    vector<int> chosenMoviesPerCategory(maxMoviesPerCategory.size(), 0);
    vector<bool> movieSelected(movies.size(), false);
    int N = movies.size();
    int M = maxMoviesPerCategory.size();

    // OpenMP directive to parallelize the outer loop
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            // Inside the loop, we verify each movie and, if it fits within the time and category constraints, increment a shared variable count.
            // As count is a shared variable, we need to protect this critical region among threads.
            
            chosenMovies.clear();
            chosenMoviesPerCategory.assign(M, 0);
            movieSelected.assign(N, false);
            int moviesWatched = 0;
            
            for (int k = i; k < N; k++) {
                const Movie &movie = movies[k];
                if (!movieSelected[movie.index] && movie.end > movie.start && chosenMoviesPerCategory[movie.category - 1] < maxMoviesPerCategory[movie.category - 1]) {
                    bool conflict = false;
                    for (const Movie &chosenMovie : chosenMovies) {
                        if (timeOverlap(chosenMovie.start, chosenMovie.end, movie.start, movie.end)) {
                            conflict = true;
                            break;
                        }
                    }
                    if (!conflict) {
                        chosenMoviesPerCategory[movie.category - 1]++;
                        moviesWatched++;
                        chosenMovies.push_back(movie);
                        movieSelected[movie.index] = true;
                    }
                }
            }
            
            // In the critical section, we check if the current count of movies watched is higher than the maximum found so far. If it is, we update the maximum count and the best combination of movies.
            #pragma omp critical
            {
                if (moviesWatched > maxMoviesWatched) {
                    maxMoviesWatched = moviesWatched;
                    bestCombination = chosenMovies;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // The main function checks the input arguments, reads the movies from the input file, sorts the movies, applies the exhaustive search algorithm, and finally prints the results along with the time taken to execute the algorithm.

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string input_file = argv[1];
    int N, M;
    vector<int> maxMoviesPerCategory;

    // Pass the input file name to the readMovies function
    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    // Sort the movies by end time, and start time in case of a tie
    sort(movies.begin(), movies.end(), compareMovies);

    vector<Movie> bestCombination;
    // Start the timer
    auto startTime = chrono::steady_clock::now();

    exhaustiveSearch(movies, maxMoviesPerCategory, bestCombination);

    // Stop the timer
    auto endTime = chrono::steady_clock::now();

    // Calculate the time elapsed during the exhaustive search execution
    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1e6;

    // Print the number of movies watched
    cout << "Movies watched: " << bestCombination.size() << endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : bestCombination) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    // Print the time elapsed during the exhaustive search execution
    cout << "Time elapsed during the exhaustive search algorithm: " << duration << " microseconds" << endl;

    return 0;
}
