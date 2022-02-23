# Comandos que serão utilizados no terminal para executar os algoritmos SA e HDSO para cada hora

for i in range(1, 25):
    print(f'./waf --run SA_{i};', end='')

for i in range(1, 25):
    print(f'./waf --run HDSO_{i};', end='')
