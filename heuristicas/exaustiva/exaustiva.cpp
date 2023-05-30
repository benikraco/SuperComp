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
void busca_exaustiva(int index, vector<Movie>& movies, vector<bool>& movieSelected, vector<int>& chosenMoviesPerCategory, vector<int>& maxMoviesPerCategory, vector<Movie>& chosenMovies, vector<Movie>& bestCombination, int& maxMoviesWatched, int moviesWatched);

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

void busca_exaustiva(int index, vector<Movie>& movies, vector<bool>& movieSelected, vector<int>& chosenMoviesPerCategory, vector<int>& maxMoviesPerCategory, vector<Movie>& chosenMovies, vector<Movie>& bestCombination, int& maxMoviesWatched, int moviesWatched) {
    // Base case: if the index is equal to the number of movies, check if the current combination is better than the best found so far
    if (index == movies.size()) {
        if (moviesWatched > maxMoviesWatched) {
            maxMoviesWatched = moviesWatched;
            bestCombination = chosenMovies;
        }
    }
    else {
        // Recursion case 1: do not include the current movie in the combination
        busca_exaustiva(index + 1, movies, movieSelected, chosenMoviesPerCategory, maxMoviesPerCategory, chosenMovies, bestCombination, maxMoviesWatched, moviesWatched);

        // Check if the movie can be included in the combination
        if (!movieSelected[index] && movies[index].end > movies[index].start && chosenMoviesPerCategory[movies[index].category - 1] < maxMoviesPerCategory[movies[index].category - 1]) {
            bool conflict = false;

            // Check for conflicts with other chosen movies
            for (const Movie &chosenMovie : chosenMovies) {
                if (timeOverlap(chosenMovie.start, chosenMovie.end, movies[index].start, movies[index].end)) {
                    conflict = true;
                    break;
                }
            }

            // If there's no conflict, add the movie to the combination
            if (!conflict) {
                chosenMoviesPerCategory[movies[index].category - 1]++;
                moviesWatched++;
                chosenMovies.push_back(movies[index]);

                // Recursion case 2: include the current movie in the combination
                busca_exaustiva(index + 1, movies, movieSelected, chosenMoviesPerCategory, maxMoviesPerCategory, chosenMovies, bestCombination, maxMoviesWatched, moviesWatched);

                // Undo the choice for the next iterations
                chosenMoviesPerCategory[movies[index].category - 1]--;
                moviesWatched--;
                chosenMovies.pop_back();
            }
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

    // Passe o nome do arquivo de input para a função readMovies
    vector<Movie> movies = readMovies(input_file, N, M, maxMoviesPerCategory);

    // Sort the movies by end time, and start time in case of a tie
    sort(movies.begin(), movies.end(), compareMovies);

    // Initialize the chosenMoviesPerCategory vector
    vector<Movie> chosenMovies, bestCombination;
    vector<int> chosenMoviesPerCategory(M, 0);
    vector<bool> movieSelected(N, false);
    int moviesWatched = 0, maxMoviesWatched = 0;

    // Start the timer
    auto startTime = chrono::steady_clock::now();

    // Start the exhaustive search
    busca_exaustiva(0, movies, movieSelected, chosenMoviesPerCategory, maxMoviesPerCategory, chosenMovies, bestCombination, maxMoviesWatched, moviesWatched);

    // Stop the timer
    auto endTime = chrono::steady_clock::now();

    // Calculate the time elapsed during the exhaustive search algorithm execution
    double duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

    // Print the number of movies watched
    cout << "Movies watched: " << maxMoviesWatched << endl;

    // Print the start and end times, and category of the selected movies
    for (const Movie &movie : bestCombination) {
        cout << "Movie start: " << movie.start << ", end: " << movie.end << ", category: " << movie.category << endl;
    }

    // Print the time elapsed during the exhaustive search algorithm execution
    cout << "Time elapsed during the exhaustive search algorithm: " << duration << " microseconds" << endl;
}
