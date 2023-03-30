import os
import subprocess
import re
from tabulate import tabulate
from termcolor import colored

# Caminhos e configurações
num_inputs = 15
heuristica_gulosa_executavel = os.path.abspath("./heuristicas/gulosa/gulosa")
heuristica_aleatoria_executavel = os.path.abspath("./heuristicas/aleatoria/aleatoria")
pasta_inputs = "heuristicas/inputs"
pasta_resultados_gulosa = "heuristicas/resultados/gulosa"
pasta_resultados_aleatoria = "heuristicas/resultados/aleatoria"

# Inicializar listas para armazenar resultados
gulosa_tempos_execucao = []
aleatoria_tempos_execucao = []
gulosa_tempos_tela = []
aleatoria_tempos_tela = []
total_filmes = []

# Expressão regular para extrair informações do output
pattern = re.compile(r"Movies watched: (\d+).*Time elapsed during.*: (\d+\.?\d*)", re.DOTALL)

# Cria as pastas de resultados se ainda não existirem
os.makedirs(pasta_resultados_gulosa, exist_ok=True)
os.makedirs(pasta_resultados_aleatoria, exist_ok=True)

# Executa o código C++ para cada arquivo de input
for i in range(num_inputs):
    input_file = f"input{i}.txt"
    input_path = os.path.join(pasta_inputs, input_file)
    output_gulosa = os.path.join(pasta_resultados_gulosa, f"output{i}.txt")
    output_aleatoria = os.path.join(pasta_resultados_aleatoria, f"output{i}.txt")

    with open(output_gulosa, "w") as output_data:
        subprocess.run([heuristica_gulosa_executavel, input_path], stdout=output_data)

    with open(output_aleatoria, "w") as output_data:
        subprocess.run([heuristica_aleatoria_executavel, input_path], stdout=output_data)

    # Lê a quantidade de filmes no arquivo de input
    with open(input_path, "r") as input_data:
        first_line = input_data.readline().strip()
        num_filmes = int(first_line.split()[0])
        total_filmes.append(num_filmes)

    # Ler e extrair resultados dos outputs
    with open(output_gulosa, "r") as output_data:
        gulosa_output = output_data.read()
        gulosa_matches = pattern.search(gulosa_output)
        gulosa_tempos_tela.append(int(gulosa_matches.group(1)))
        gulosa_tempos_execucao.append(float(gulosa_matches.group(2)))

    with open(output_aleatoria, "r") as output_data:
        aleatoria_output = output_data.read()
        aleatoria_matches = pattern.search(aleatoria_output)
        aleatoria_tempos_tela.append(int(aleatoria_matches.group(1)))
        aleatoria_tempos_execucao.append(float(aleatoria_matches.group(2)))

# Preparar dados para exibir como tabela
table_data = [
    ["Input"] + [f"Input {i}" for i in range(num_inputs)],
    [colored("Gulosa (us)", "cyan")] + [colored(f"{t:.2f}", "cyan") for t in gulosa_tempos_execucao],
    [colored("Aleatória (us)", "green")] + [colored(f"{t:.2f}", "green") for t in aleatoria_tempos_execucao],
    [colored("Horas de tela Gulosa", "cyan")] + [colored(str(h), "cyan") for h in gulosa_tempos_tela],
    [colored("Horas de tela Aleatória", "green")] + [colored(str(h), "green") for h in aleatoria_tempos_tela],
]

# Adicionar coluna com o total de filmes disponíveis por input na tabela
table_data.insert(1, ["Total de filmes"] + total_filmes)

# Transpor a tabela para facilitar a leitura
table_data = list(map(list, zip(*table_data)))

# Imprimir resultados comparativos
print("Comparação das Heurísticas:")
print(tabulate(table_data, headers="firstrow", tablefmt="fancy_grid"))
