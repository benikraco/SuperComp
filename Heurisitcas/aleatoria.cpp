#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <random>
#include <ctime>

// Define the Movie structure with start, end, category, and index fields
struct Movie {
    int start, end, category, index;
};

// Function prototypes
bool compareMovies(const Movie &a, const Movie &b);
bool timeOverlap(int start1, int end1, int start2, int end2);
std::vector<Movie> readMovies(std::string filename, int &N, int &M, std::vector<int> &maxMoviesPerCategory);

// Compare function for sorting movies by end time, and start time in case of a tie
bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

// Function to check if two time intervals overlap
bool timeOverlap(int start1, int end1, int start2, int end2) {
    return start1 < end2 && start2 < end1;
}

// Function to read movies from input file and return a vector of Movie structures
std::vector<Movie> readMovies(std::string filename, int &N, int &M, std::vector<int> &maxMoviesPerCategory) {
    std::ifstream inputFile(filename);

    // Read the number of movies and categories from the input file
    inputFile >> N >> M;

    // Resize the maxMoviesPerCategory vector according to the number of categories
    maxMoviesPerCategory.resize(M);

    // Read the maximum number of movies per category
    for (int i = 0; i < M; i++) {
        inputFile >> maxMoviesPerCategory[i];
    }

    // Initialize the movies vector with the number of movies
    std::vector<Movie> movies(N);

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

int main() {
    int N, M;
    std::vector<int> maxMoviesPerCategory;

    // Read movies from input file
    std::vector<Movie> movies = readMovies("input.txt", N, M, maxMoviesPerCategory);

    // Sort the movies by end time, and start time in case of a tie
    std::sort(movies.begin(), movies.end(), compareMovies);

    // Initialize the chosenMoviesPerCategory vector
    std::vector<int> chosenMoviesPerCategory(M, 0);
    std::vector<Movie> chosenMovies;
    int moviesWatched = 0;

    // Initialize a vector to track which movies have been selected
    std::vector<bool> movieSelected(N, false);

    // Initialize the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    gen.seed(rd() + time(0));

    // Start the timer
    auto startTime = std::chrono::steady_clock::now();

    // Iterate over the sorted movies
    for (const Movie &movie : movies) {

        // Check if the movie doesn't cross the day boundary and there are available slots for the movie category
        if (!movieSelected[movie.index] && movie.end > movie.start && chosenMoviesPerCategory[movie.category - 1] < maxMoviesPerCategory[movie.category - 1]) {
            bool conflict = false;

            // Check for conflicts with other chosen movies
            for (const Movie &chosenMovie : chosenMovies) {
                if (timeOverlap(chosenMovie.start, chosenMovie.end, movie.start, movie.end)) {
                    conflict = true;
                    break;
                }
            }

            // If there's no conflict, increment the chosen movies count for the category
            if (!conflict) {
                double randomValue = distribution(gen);
                if (randomValue <= 0.25) {

                    // Find an alternative movie respecting the time constraints (25% chance)
                    for (const Movie &alternativeMovie : movies) {

                        // Skip if the alternative movie has already been selected
                        if (movieSelected[alternativeMovie.index]) {
                            continue;
                        }

                        bool altConflict = false;

                        // Check for conflicts with other chosen movies
                        for (const Movie &chosenMovie : chosenMovies) {
                            if (timeOverlap(chosenMovie.start, chosenMovie.end, alternativeMovie.start, alternativeMovie.end)) {
                                altConflict = true;
                                break;
                            }
                        }

                        // Check if the alternative movie doesn't cross the day boundary and there are available slots for the movie category
                        if (!altConflict && chosenMoviesPerCategory[alternativeMovie.category - 1] < maxMoviesPerCategory[alternativeMovie.category - 1] && alternativeMovie.start != alternativeMovie.end) {
                            chosenMoviesPerCategory[alternativeMovie.category - 1]++;
                            moviesWatched++;
                            chosenMovies.push_back(alternativeMovie);
                            movieSelected[alternativeMovie.index] = true; // Mark the alternative movie as selected
                            break;
                        }
                    }

                // Otherwise, select the current movie
                } else {
                    chosenMoviesPerCategory[movie.category - 1]++;
                    moviesWatched++;
                    chosenMovies.push_back(movie);
                    movieSelected[movie.index] = true; // Mark the movie as selected
                }
            }
        }
    }

    // Stop the timer
    auto endTime = std::chrono::steady_clock::now();

    // Calculate the time elapsed during algorithm execution
    double durationInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000000.0;

    // Sort the chosenMovies vector by end time, and start time in case of a tie
    std::sort(chosenMovies.begin(), chosenMovies.end(), compareMovies);

    // Print the number of movies watched
    std::cout << "Movies watched: " << moviesWatched << std::endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : chosenMovies) {
        std::cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << std::endl;
    }

    // Print the time elapsed during the aleatory algorithm execution
    std::cout << "Time elapsed during the aleatory algorithm: " << durationInSeconds << " seconds" << std::endl;
}