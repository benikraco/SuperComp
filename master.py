import os
import subprocess
import re
from prettytable import PrettyTable

# Caminhos e configurações
num_inputs = 6
heuristica_gulosa_executavel = os.path.abspath("./heuristicas/gulosa/gulosa")
heuristica_aleatoria_executavel = os.path.abspath("./heuristicas/aleatoria/aleatoria")
heuristica_exaustiva_executavel = os.path.abspath("./heuristicas/exaustiva/exaustiva")
pasta_inputs = "heuristicas/inputs"
pasta_resultados_gulosa = "heuristicas/resultados/gulosa"
pasta_resultados_aleatoria = "heuristicas/resultados/aleatoria"
pasta_resultados_exaustiva = "heuristicas/resultados/exaustiva"

# Inicializar listas para armazenar resultados
gulosa_tempos_execucao = []
aleatoria_tempos_execucao = []
exaustiva_tempos_execucao = []
gulosa_assistido = []
aleatoria_assistido = []
exaustiva_assistido = []
total_filmes = []
total_categorias = []
gulosa_tempo_tela = []
aleatoria_tempo_tela = []
exaustiva_tempo_tela = []

tempo_tela_gulosa = 0
tempo_tela_aleatoria = 0
tempo_tela_exaustiva = 0

# Expressão regular para extrair informações do output
pattern = re.compile(r"Movies watched: (\d+).*Time elapsed during.*: (\d+\.?\d*)", re.DOTALL)
pattern_tempo_tela = re.compile(r"Movie start: (\d+), end: (\d+)", re.DOTALL)

# Cria as pastas de resultados se ainda não existirem
os.makedirs(pasta_resultados_gulosa, exist_ok=True)
os.makedirs(pasta_resultados_aleatoria, exist_ok=True)
os.makedirs(pasta_resultados_exaustiva, exist_ok=True)

# Executa o código C++ para cada arquivo de input
for i in range(num_inputs):
    tempo_tela_aleatoria = 0
    tempo_tela_gulosa = 0
    tempo_tela_exaustiva = 0

    input_file = f"input{i}.txt"
    input_path = os.path.join(pasta_inputs, input_file)
    output_gulosa = os.path.join(pasta_resultados_gulosa, f"output{i}.txt")
    output_aleatoria = os.path.join(pasta_resultados_aleatoria, f"output{i}.txt")
    output_exaustiva = os.path.join(pasta_resultados_exaustiva, f"output{i}.txt")

    with open(output_gulosa, "w") as output_data:
        subprocess.run([heuristica_gulosa_executavel, input_path], stdout=output_data)

    with open(output_aleatoria, "w") as output_data:
        subprocess.run([heuristica_aleatoria_executavel, input_path], stdout=output_data)
    
    with open(output_exaustiva, "w") as output_data:
        subprocess.run([heuristica_exaustiva_executavel, input_path], stdout=output_data)

    # Lê a quantidade de filmes e categorias no arquivo de input
    with open(input_path, "r") as input_data:
        first_line = input_data.readline().strip()
        num_filmes, num_categorias = map(int, first_line.split(" "))
        total_filmes.append(num_filmes)
        total_categorias.append(num_categorias)

    # Extrai os resultados para heurística gulosa
    with open(output_gulosa, "r") as output_data:
        output_text = output_data.read()
        match = pattern.search(output_text)
        if match:
            gulosa_assistido.append(int(match.group(1)))
            gulosa_tempos_execucao.append(float(match.group(2)))
        match_tempo_tela = pattern_tempo_tela.findall(output_text)
        if match_tempo_tela:
            for item in match_tempo_tela:
                tempo_tela_gulosa += (int(item[1]) - int(item[0]))
        gulosa_tempo_tela.append(tempo_tela_gulosa)

    # Extrai os resultados para heurística aleatória
    with open(output_aleatoria, "r") as output_data:
        output_text = output_data.read()
        match = pattern.search(output_text)
        if match:
            aleatoria_assistido.append(int(match.group(1)))
            aleatoria_tempos_execucao.append(float(match.group(2)))
        match_tempo_tela = pattern_tempo_tela.findall(output_text)
        if match_tempo_tela:
            for item in match_tempo_tela:
                tempo_tela_aleatoria += (int(item[1]) - int(item[0]))
        aleatoria_tempo_tela.append(tempo_tela_aleatoria)

    # Extrai os resultados para heurística exaustiva
    with open(output_exaustiva, "r") as output_data:
        output_text = output_data.read()
        match = pattern.search(output_text)
        if match:
            exaustiva_assistido.append(int(match.group(1)))
            exaustiva_tempos_execucao.append(float(match.group(2)))
        match_tempo_tela = pattern_tempo_tela.findall(output_text)
        if match_tempo_tela:
            for item in match_tempo_tela:
                tempo_tela_exaustiva += (int(item[1]) - int(item[0]))
        exaustiva_tempo_tela.append(tempo_tela_exaustiva)

# Cria a tabela para mostrar os resultados
table_data = [["Input", "Heurística", "Filmes assistidos", "Tempo de execução", "Tempo na tela"]]

for i in range(num_inputs):
    table_data.append([f"input{i}.txt", "Gulosa", gulosa_assistido[i], gulosa_tempos_execucao[i], gulosa_tempo_tela[i]])
    table_data.append([f"input{i}.txt", "Aleatória", aleatoria_assistido[i], aleatoria_tempos_execucao[i], aleatoria_tempo_tela[i]])
    table_data.append([f"input{i}.txt", "Exaustiva", exaustiva_assistido[i], exaustiva_tempos_execucao[i], exaustiva_tempo_tela[i]])

# Criar uma nova tabela usando PrettyTable
tabela = PrettyTable()

# Adicionar o cabeçalho da tabela
tabela.field_names = table_data[0]

# Adicionar as linhas restantes
for row in table_data[1:]:
    tabela.add_row(row)

# Configurar o alinhamento das colunas
for field_name in tabela.field_names:
    tabela.align[field_name] = "l"

# Imprimir a tabela
print("Comparação das Heurísticas:")
print(tabela)

# Salvar a tabela em um arquivo
with open("comparacao_heuristicas.txt", "w") as file:
    file.write(str(tabela))
