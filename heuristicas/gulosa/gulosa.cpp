#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
using namespace std;

// Define the Movie structure with start, end, and category fields
struct Movie {
    int start, end, category, index;
};

// Function prototypes
bool compareMovies(const Movie &a, const Movie &b);
bool timeOverlap(int start1, int end1, int start2, int end2);
vector<Movie> readMovies(string filename, int &N, int &M, vector<int> &maxMoviesPerCategory);

// Compare function for sorting movies by end time, and start time in case of a tie
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

// Function to read movies from input file and return a vector of Movie structures
vector<Movie> readMovies(string filename, int &N, int &M, vector<int> &maxMoviesPerCategory) {
    ifstream inputFile(filename);

    // Read the number of movies and categories from the input file
    inputFile >> N >> M;

    // Resize the maxMoviesPerCategory vector according to the number of categories
    maxMoviesPerCategory.resize(M);

    // Read the maximum number of movies per category
    for (int i = 0; i < M; i++) {
        inputFile >> maxMoviesPerCategory[i];
    }

    // Initialize the movies vector with the number of movies
    vector<Movie> movies(N);




    // Read the movie information
    for (int i = 0; i < N; i++) {
        inputFile >> movies[i].start >> movies[i].end >> movies[i].category;
        if (movies[i].end < movies[i].start) {
            movies[i].end += 24;
        }
        movies[i].index = i;
    }

    // Close the input file
    inputFile.close();
    return movies;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string input_file = argv[1];
    int N, M;
    vector<int> maxMoviesPerCategory;

     // Initialize the vector of which movies have been selected
    vector<bool> movieSelected(N, false);

    // Passe o nome do arquivo de input para a função readMovies
    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    // Sort the movies by end time, and start time in case of a tie
    sort(movies.begin(), movies.end(), compareMovies);

    // Initialize the chosenMoviesPerCategory vector
    vector<int> chosenMoviesPerCategory(M, 0);
    vector<Movie> chosenMovies;
    int moviesWatched = 0;

    // Start the timer
    auto startTime = chrono::steady_clock::now();

    // Iterate over the sorted movies
    for (const Movie &movie : movies) {


        // Check if the movie doesn't cross the day boundary and there are available slots for the movie category
        if (!movieSelected[movie.index] && movie.end > movie.start && chosenMoviesPerCategory[movie.category - 1] < maxMoviesPerCategory[movie.category - 1]) {
            bool conflict = false;

            // Check for conflicts with other chosen movies
            for (const Movie &chosenMovie : chosenMovies) {
                if (timeOverlap(chosenMovie.start, chosenMovie.end, movie.start, movie.end)) {
                    conflict = true;
                }
            }

            // If there's no conflict, increment the chosen movies count for the category
            if (!conflict) {
                chosenMoviesPerCategory[movie.category - 1]++;
                moviesWatched++;
                chosenMovies.push_back(movie);
                movieSelected[movie.index] = true;
            }

        }
    }

    // Stop the timer
    auto endTime = chrono::steady_clock::now();

    // Calculate the time elapsed during the greedy algorithm execution
    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
    
    // Print the number of movies watched
    cout << "Movies watched: " << moviesWatched << endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : chosenMovies) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    // Print the time elapsed during the greedy algorithm execution
    cout << "Time elapsed during the greedy algorithm: " << duration << " microseconds" << endl;

}

