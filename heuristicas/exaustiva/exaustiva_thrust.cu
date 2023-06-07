#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/functional.h>
#include <thrust/transform.h>
#include <thrust/extrema.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <math.h>
using namespace std;

// Structure for movies
struct Movie {
    int start, end, category, index;
};

bool compareMovies(const Movie &a, const Movie &b) {
    if (a.end == b.end) {
        return a.start < b.start;
    }
    return a.end < b.end;
}

// Transformer to perform the calculation on the GPU
struct Transformer {
    int total_movies;
    int total_categories;
    int *max_movies_per_category;
    Movie *movie_list;

    Transformer(int _total_movies, int _total_categories, int* _max_movies_per_category, Movie *_movie_list) : 
        total_movies(_total_movies), total_categories(_total_categories), max_movies_per_category(_max_movies_per_category), movie_list(_movie_list) {};

    __device__ __host__
    thrust::tuple<int, unsigned long long> operator() (unsigned long long combination) {
        int movie_counter = 0;
        int count_per_category[10];
        bool time_slots[24];
        bool valid, finished;

        for (int i = 0; i < total_categories; i++) count_per_category[i] = 0;
        for (int i = 0; i < 24; i++) time_slots[i] = false;

        for (int i = 0; i < total_movies; i++) {
            valid = true;
            finished = true;
            unsigned long long comparison = pow(2, i);
            if ((combination & comparison) && (count_per_category[movie_list[i].category-1] < max_movies_per_category[movie_list[i].category-1])) {
                for (int j = movie_list[i].start; j < (movie_list[i].end > movie_list[i].start ? movie_list[i].end : 24); j++) {
                    if (time_slots[j]) {
                        valid = false;
                        break;
                    }
                    time_slots[j] = true;
                }
                if (valid) {
                    count_per_category[movie_list[i].category-1]++;
                    movie_counter++;
                    for (int j = 0; j < 10; j++) {
                        if (count_per_category[j] != max_movies_per_category[j]) {
                            finished = false;
                            break;
                        }
                    }
                    if (movie_counter >= 24 || finished) break;
                }
            }
        }
        return thrust::make_tuple(movie_counter, combination);
    }
};

// Custom comparison function to find the optimal combination
struct Comparator {
    __device__ __host__
    bool operator() (const thrust::tuple<int, unsigned long long>& a, const thrust::tuple<int, unsigned long long>& b) const {
        return thrust::get<0>(a) < thrust::get<0>(b);
    }
};


// Read movie information from a file
vector<Movie> readMovies(string file_name, int &N, int &M, vector<int> &max_movies_per_category) {
    ifstream input_file(file_name);

    input_file >> N >> M;
    max_movies_per_category.resize(M);
    for (int i = 0; i < M; i++) input_file >> max_movies_per_category[i];

    vector<Movie> movies(N);
    for (int i = 0; i < N; i++) {
        input_file >> movies[i].start >> movies[i].end >> movies[i].category;
        movies[i].index = i;
        if (movies[i].end < movies[i].start) movies[i].end += 24;
    }

    input_file.close();
    return movies;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string input_file = argv[1];
    int number_of_movies, number_of_categories;
    vector<int> max_movies_per_category;

    // Reading movies from the file
    vector<Movie> movies = readMovies(input_file, number_of_movies, number_of_categories, max_movies_per_category);

    // Sorting the movies based on their end and start times
    sort(movies.begin(), movies.end(), compareMovies);

    thrust::device_vector<unsigned long long> possibilities(pow(2, number_of_movies));
    thrust::sequence(possibilities.begin(), possibilities.end());

    thrust::device_vector<int> max_categories_dev = max_movies_per_category;
    thrust::device_vector<Movie> movies_dev = movies;

    thrust::device_vector<thrust::tuple<int, unsigned long long>> final_output(pow(2, number_of_movies));

    auto start_calculation = chrono::high_resolution_clock::now();
    thrust::transform(
        possibilities.begin(),
        possibilities.end(),
        final_output.begin(),
        Transformer(number_of_movies, number_of_categories, thrust::raw_pointer_cast(max_categories_dev.data()), thrust::raw_pointer_cast(movies_dev.data()))
    );

    auto max_iter = thrust::max_element(final_output.begin(), final_output.end(), Comparator());
    auto end_calculation = chrono::high_resolution_clock::now();
    auto calculation_duration = chrono::duration_cast<chrono::microseconds>(end_calculation - start_calculation);

    thrust::host_vector<thrust::tuple<int, unsigned long long>> final_output_host = final_output;

    int max_movies = thrust::get<0>(final_output_host[max_iter - final_output.begin()]);
    unsigned long long max_combination = thrust::get<1>(final_output_host[max_iter - final_output.begin()]);

    cout << "Movies watched: " << max_movies << endl;
    unsigned long long comparison;
    for (int i = 0; i < number_of_movies; i++) {
        comparison = pow(2, i);
        if (max_combination & comparison) {
            cout << "Movie start: " << movies[i].start << ", end: " << (movies[i].end > movies[i].start ? movies[i].end : movies[i].end - 24) << ", category: " << movies[i].category << endl;
        }
    }

    cout << "Time elapsed during the exhaustive search algorithm: " << calculation_duration.count() << " microseconds" << endl;

    return 0;
}
