#include <iostream>
#include <fstream>
#include <thrust/device_vector.h>
#include <thrust/transform_reduce.h>
#include <thrust/execution_policy.h>

struct max_filmes_functor {
    int M;
    int *start_times;
    int *end_times;
    int *categories;
    int *L;
    int c;

    max_filmes_functor(int M, int *start_times, int *end_times, int *categories, int *L)
        : M(M), start_times(start_times), end_times(end_times), categories(categories), L(L) {}

    __host__ __device__
    int operator()(const thrust::tuple<int,int>& t) const {
        int peso = thrust::get<0>(t);
        int filmes = thrust::get<1>(t);
        if (peso <= c) {
            return filmes;
        } else {
            return 0;
        }
    }

};

int main() {
    // Abrindo o arquivo de entrada
    std::ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de entrada!" << std::endl;
        return -1;
    }

    // Lendo o número de filmes e categorias
    int N, M;
    input_file >> N >> M;

    // Lendo as horas de início, término e categorias dos filmes
    thrust::host_vector<int> h_start_times(N);
    thrust::host_vector<int> h_end_times(N);
    thrust::host_vector<int> h_categories(N);
    for (int i = 0; i < N; i++) {
        input_file >> h_start_times[i] >> h_end_times[i] >> h_categories[i];
    }

    // Lendo o número máximo de filmes por categoria
    thrust::host_vector<int> h_L(M);
    for (int j = 0; j < M; j++) {
        input_file >> h_L[j];
    }

    // Fechando o arquivo de entrada
    input_file.close();

   // Copiar os dados para a GPU
    thrust::device_vector<int> d_start_times = h_start_times;
    thrust::device_vector<int> d_end_times = h_end_times;
    thrust::device_vector<int> d_categories = h_categories;
    thrust::device_vector<int> d_L = h_L;

    max_filmes_functor functor(M, d_start_times.data().get(), d_end_times.data().get(),
                               d_categories.data().get(), d_L.data().get());

    // Calculando o número máximo de filmes que podem ser assistidos
    int max_count = thrust::transform_reduce(thrust::device, 
                                             thrust::make_counting_iterator(0), 
                                             thrust::make_counting_iterator(N*M), 
                                             functor, 
                                             0, 
                                             thrust::maximum<int>());

    // Abrindo o arquivo de saída
    std::ofstream output_file("output.txt");
    if (!output_file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de saída!" << std::endl;
        return -1;
    }

    // Escrevendo o resultado no arquivo de saída
    output_file << max_count << std::endl;

    // Fechando o arquivo de saída
    output_file.close();

    return 0;
}