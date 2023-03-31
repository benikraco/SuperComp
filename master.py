import os
import subprocess
import re
from tabulate import tabulate
from termcolor import colored
import pickle

# Caminhos e configurações
num_inputs = 25
heuristica_gulosa_executavel = os.path.abspath("./heuristicas/gulosa/gulosa")
heuristica_aleatoria_executavel = os.path.abspath("./heuristicas/aleatoria/aleatoria")
pasta_inputs = "heuristicas/inputs"
pasta_resultados_gulosa = "heuristicas/resultados/gulosa"
pasta_resultados_aleatoria = "heuristicas/resultados/aleatoria"

# Inicializar listas para armazenar resultados
gulosa_tempos_execucao = []
aleatoria_tempos_execucao = []
gulosa_assistido = []
aleatoria_assistido = []
total_filmes = []
total_categorias = []
filmes_por_categoria = []
gulosa_tempo_tela = []
aleatoria_tempo_tela = []

tempo_tela_gulosa = 0
tempo_tela_aleatoria = 0

# Expressão regular para extrair informações do output
pattern = re.compile(r"Movies watched: (\d+).*Time elapsed during.*: (\d+\.?\d*)", re.DOTALL)
pattern_tempo_tela = re.compile(r"Movie start: (\d+), end: (\d+)", re.DOTALL)

# Cria as pastas de resultados se ainda não existirem
os.makedirs(pasta_resultados_gulosa, exist_ok=True)
os.makedirs(pasta_resultados_aleatoria, exist_ok=True)

# Executa o código C++ para cada arquivo de input
for i in range(num_inputs):
    tempo_tela_aleatoria = 0
    tempo_tela_gulosa = 0

    input_file = f"input{i}.txt"
    input_path = os.path.join(pasta_inputs, input_file)
    output_gulosa = os.path.join(pasta_resultados_gulosa, f"output{i}.txt")
    output_aleatoria = os.path.join(pasta_resultados_aleatoria, f"output{i}.txt")

    with open(output_gulosa, "w") as output_data:
        subprocess.run([heuristica_gulosa_executavel, input_path], stdout=output_data)

    with open(output_aleatoria, "w") as output_data:
        subprocess.run([heuristica_aleatoria_executavel, input_path], stdout=output_data)

    # Lê a quantidade de filmes e categorias no arquivo de input
    with open(input_path, "r") as input_data:
        first_line = input_data.readline().strip()
        num_filmes, num_categorias = map(int, first_line.split())
        total_filmes.append(num_filmes)
        total_categorias.append(num_categorias)

        # Lê a quantidade de filmes por categoria
        second_line = input_data.readline().strip()
        filmes_categoria = list(map(int, second_line.split()))
        filmes_por_categoria.append(filmes_categoria)

    # Ler e extrair resultados dos outputs
    with open(output_gulosa, "r") as output_data:
        gulosa_output = output_data.read()
        gulosa_matches = pattern.search(gulosa_output)
        gulosa_matches_tempo_tela = pattern_tempo_tela.search(gulosa_output)
        gulosa_assistido.append(int(gulosa_matches.group(1)))
        gulosa_tempos_execucao.append(float(gulosa_matches.group(2)))
        for match in pattern_tempo_tela.finditer(gulosa_output):
            tempo_tela_gulosa += int(match.group(2)) - int(match.group(1))
            if tempo_tela_gulosa > 24:
                tempo_tela_gulosa = 24
        gulosa_tempo_tela.append(tempo_tela_gulosa)



    with open(output_aleatoria, "r") as output_data:
        aleatoria_output = output_data.read()
        aleatoria_matches = pattern.search(aleatoria_output)
        aleatoria_matches_tempo_tela = pattern_tempo_tela.search(aleatoria_output)
        aleatoria_assistido.append(int(aleatoria_matches.group(1)))
        aleatoria_tempos_execucao.append(float(aleatoria_matches.group(2)))

        for match in pattern_tempo_tela.finditer(aleatoria_output):
            tempo_tela_aleatoria += int(match.group(2)) - int(match.group(1))
            if tempo_tela_aleatoria > 24:
                tempo_tela_aleatoria = 24
        aleatoria_tempo_tela.append(tempo_tela_aleatoria)


# Preparar dados para exibir como tabela
table_data = [
    ["Input"] + [f"Input {i}" for i in range(num_inputs)],
    [colored("Total de filmes", "yellow")] + [colored(str(h), "yellow") for h in total_filmes],
    [colored("Total de categorias", "yellow")] + [colored(str(h), "yellow") for h in total_categorias],
    [colored("Gulosa (us)", "cyan")] + [colored(f"{t:.2f}", "cyan") for t in gulosa_tempos_execucao],
    [colored("Aleatória (us)", "green")] + [colored(f"{t:.2f}", "green") for t in aleatoria_tempos_execucao],
    [colored("Filmes assistidos Gulosa", "cyan")] + [colored(str(h), "cyan") for h in gulosa_assistido],
    [colored("Filmes assistidos Aleatória", "green")] + [colored(str(h), "green") for h in aleatoria_assistido],
    [colored("Tempo na tela Gulosa", "cyan")] + [colored(str(h), "cyan") for h in gulosa_tempo_tela],
    [colored("Tempo na tela Aleatória", "green")] + [colored(str(h), "green") for h in aleatoria_tempo_tela]
]

# Transpor a tabela para facilitar a leitura
table_data = list(map(list, zip(*table_data)))

# Importando arquivos pro pickle

data = {
    "total_filmes": total_filmes,
    "gulosa_tempos_execucao": gulosa_tempos_execucao,
    "aleatoria_tempos_execucao": aleatoria_tempos_execucao,
    "gulosa_assistido": gulosa_assistido,
    "aleatoria_assistido": aleatoria_assistido,
    "total_categorias": total_categorias,
    "filmes_por_categoria": filmes_por_categoria,
    "gulosa_tempo_tela": gulosa_tempo_tela,
    "aleatoria_tempo_tela": aleatoria_tempo_tela
}

with open("data.pkl", "wb") as f:
    pickle.dump(data, f)

# Imprimir resultados comparativos
print("Comparação das Heurísticas:")
print(tabulate(table_data, headers="firstrow", tablefmt="fancy_grid"))
