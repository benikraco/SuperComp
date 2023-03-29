import os
import subprocess
import re
from termcolor import colored

# Caminhos e configurações
num_inputs = 5
heuristica_gulosa_executavel = os.path.abspath("./heuristicas/gulosa/gulosa")
heuristica_aleatoria_executavel = os.path.abspath("./heuristicas/aleatoria/aleatoria")
pasta_inputs = "heuristicas/inputs"

# Inicializar listas para armazenar resultados
gulosa_tempos_execucao = []
aleatoria_tempos_execucao = []
gulosa_tempos_tela = []
aleatoria_tempos_tela = []

# Expressão regular para extrair informações do output
pattern = re.compile(r"Movies watched: (\d+).*Time elapsed during.*: (\d+\.?\d*)", re.DOTALL)

# Executa o código C++ para cada arquivo de input
for i in range(num_inputs):
    input_file = f"input{i}.txt"
    input_path = os.path.join(pasta_inputs, input_file)

    with subprocess.Popen([heuristica_gulosa_executavel, input_path], stdout=subprocess.PIPE, text=True) as proc:
        output_gulosa = proc.stdout.read()
        match = pattern.search(output_gulosa)
        gulosa_tempos_tela.append(int(match.group(1)))
        gulosa_tempos_execucao.append(float(match.group(2)))

    with subprocess.Popen([heuristica_aleatoria_executavel, input_path], stdout=subprocess.PIPE, text=True) as proc:
        output_aleatoria = proc.stdout.read()
        match = pattern.search(output_aleatoria)
        aleatoria_tempos_tela.append(int(match.group(1)))
        aleatoria_tempos_execucao.append(float(match.group(2)))

# Imprimir resultados comparativos
print(colored("\nComparação de tempo de execução (microssegundos):", 'green'))
print(f"{'Input':<10}        {'Gulosa':<10}{'Aleatória':<10}")
for i in range(num_inputs):
    print(f"{colored(f'input{i}.txt', 'cyan')}        {gulosa_tempos_execucao[i]:<10.2f}{aleatoria_tempos_execucao[i]:<10.2f}")

print("\n" + colored("Comparação de tempo de tela (horas assistidas):", 'green'))
print(f"{'Input':<10}        {'Gulosa':<10}{'Aleatória':<10}")
for i in range(num_inputs):
    print(f"{colored(f'input{i}.txt', 'cyan')}        {gulosa_tempos_tela[i]:<10}{aleatoria_tempos_tela[i]:<10}")
