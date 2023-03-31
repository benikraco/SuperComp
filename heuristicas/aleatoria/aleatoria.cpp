#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <random>
#include <ctime>
using namespace std;

// Define the Movie structure with start, end, category, and index fields
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
        movies[i].index = i; // Set the index of the movie
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

    // Passe o nome do arquivo de input para a função readMovies
    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);
    

    // Sort the movies by end time, and start time in case of a tie
    sort(movies.begin(), movies.end(), compareMovies);

    // Initialize the chosenMoviesPerCategory vector
    vector<int> chosenMoviesPerCategory(M, 0);
    vector<Movie> chosenMovies;
    int moviesWatched = 0;

    // Initialize a vector to track which movies have been selected
    vector<bool> movieSelected(N, false);

    // Initialize the random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> distribution(0.0, 1.0);
    gen.seed(rd() + time(0));

    // Start the timer
    auto startTime = chrono::steady_clock::now();

    // Iterate over the sorted movies
    for (const Movie &movie : movies) {

        double randomValue = distribution(gen);
        if (randomValue <= 0.25) {

            // Make another alearization to find an alternative movie
            uniform_int_distribution<int> distribution2(movie.index, N - 1);
            bool conflict2 = false;
                
            int randomValue2 = distribution2(gen);

            if (movies[randomValue2].start == movies[randomValue2].end) {
                continue;
            }

            for (auto& chosen_movie: chosenMovies){
                // verify if movies is valid to be selected compared to movie
                if (!(!(timeOverlap(movies[randomValue2].start, movies[randomValue2].end, chosen_movie.start, chosen_movie.end)) && (chosenMoviesPerCategory[movies[randomValue2].category - 1] < maxMoviesPerCategory[movies[randomValue2].category - 1]))){
                    conflict2=true; 
                }
            }

            if (!conflict2){
                chosenMoviesPerCategory[movies[randomValue2].category - 1]++;
                moviesWatched++;
                chosenMovies.push_back(movies[randomValue2]);
                movieSelected[randomValue2] = true; // Mark the alternative movie as selected 
            } else{
                // cout << "No alternative movie found" << endl;
            }
        }

        if (movie.start == movie.end) {
            continue;
        }

        // Check if the movie doesn't cross the day boundary and there are available slots for the movie category
        if (!movieSelected[movie.index] && movie.end > movie.start && (chosenMoviesPerCategory[movie.category - 1] < maxMoviesPerCategory[movie.category - 1])) {
            bool conflict = false;

            // Check for conflicts with other chosen movies
            for (const Movie &chosenMovie : chosenMovies) {
                if (timeOverlap(chosenMovie.start, chosenMovie.end, movie.start, movie.end)) {
                    conflict = true;
                }
            }

            // Check for conflicts with other chosen movies

            if (!conflict) {
                chosenMoviesPerCategory[movie.category - 1]++;
                moviesWatched++;
                chosenMovies.push_back(movie);
                movieSelected[movie.index] = true; // Mark the alternative movie as selected
            } else {
                // cout << "No alternative movie found 2" << endl;
            }
    
        }
    }    

    // Stop the timer
    auto endTime = chrono::steady_clock::now();

    // Calculate the time elapsed during algorithm execution
    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    // Sort the chosenMovies vector by end time, and start time in case of a tie
    sort(chosenMovies.begin(), chosenMovies.end(), compareMovies);

    // Print the number of movies watched
    cout << "Movies watched: " << moviesWatched << endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : chosenMovies) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    // Print the time elapsed during the aleatory algorithm execution
    cout << "Time elapsed during the aleatory algorithm: " << duration << " microseconds" << endl;
}