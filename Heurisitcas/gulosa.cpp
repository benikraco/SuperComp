#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

// Define the Movie structure with start, end, and category fields
struct Movie {
    int start, end, category;
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

// Function to adjust the end time of a movie if it crosses the day boundary
int adjustEndTime(int endTime) {
    if (endTime == 0) {
        return 24;
    }
    return endTime;
}

// Function to check if two time intervals overlap
bool timeOverlap(int start1, int end1, int start2, int end2) {
    end1 = adjustEndTime(end1);
    end2 = adjustEndTime(end2);

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
        movies[i].end = adjustEndTime(movies[i].end);
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

    // Start the timer
    auto startTime = std::chrono::steady_clock::now();

    // Iterate over the sorted movies
    for (const Movie &movie : movies) {

        // Check if the movie doesn't cross the day boundary and there are available slots for the movie category
        if (movie.end > movie.start && chosenMoviesPerCategory[movie.category - 1] < maxMoviesPerCategory[movie.category - 1]) {
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
                chosenMoviesPerCategory[movie.category - 1]++;
                moviesWatched++;
                chosenMovies.push_back(movie);
            }

        }
    }

    // Stop the timer
    auto endTime = std::chrono::steady_clock::now();

    // Calculate the time elapsed during the greedy algorithm execution
    double durationInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000000.0;


    // Print the number of movies watched
    std::cout << "Movies watched: " << moviesWatched << std::endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : chosenMovies) {
        std::cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << std::endl;
    }

    // Print the time elapsed during the greedy algorithm execution
    std::cout << "Time elapsed during the greedy algorithm: " << durationInSeconds << " seconds" << std::endl;

}

