import pandas as pd
import numpy as np

with open("small_position.txt",'w') as f: # Limpando  o arquivo
    pass

small_position = 'SmallPosition.xls'

df_small = pd.read_excel(small_position)

print(df_small)

small_matrix = df_small.to_numpy()
print(small_matrix)
print(small_matrix.shape)

hora = 1.0
for posicao in range(np.size(small_matrix, 1)):  # Eixos x e y para cada hora
    if posicao%2 != 0  and posicao!= 0 :
        print(f'INICIO HORA {hora}')
        with open('small_position.txt', 'a') as arquivo:
                arquivo.write(f'\nINICIO HORA {hora}\n')
        
        for small in range(np.size(small_matrix, 0)):  # Quantidade de smalls    
            
            if small==0:
                print(f'    Comando para escrever a lista ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]})')

                with open('small_position.txt', 'a') as arquivo:
                    arquivo.write(f'    Comando para escrever a lista ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]})\n')

            elif small_matrix[small, (posicao - 1)] != 0 or small_matrix[small,posicao] != 0 :
                print(f'    Comando para escrever a lista ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]})')

                with open('small_position.txt', 'a') as arquivo:
                    arquivo.write(f'    Comando para escrever a lista ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]})\n')
        
        with open('small_position.txt', 'a') as arquivo:
                arquivo.write(f'FIM HORA {hora}\n')
        hora += 1
                


    


        


# print(user_matrix[29, 10])
# print(np.size(user_matrix, 0))

# for usuario in range(np.size(user_matrix, 0)):
#     print(usuario)
#     if usuario%2 != 0  and usuario!= 0 :
#         print('True')





