import pandas as pd
import numpy as np

with open("user_position.txt",'w') as f: # Limpando  o arquivo
    pass

user_position = 'UserPosition.xls'
small_position = 'SmallPosition.xls'

df_usuarios = pd.read_excel(user_position)
df_small = pd.read_excel(small_position)

print(df_small)
print(df_usuarios)

user_matrix = df_usuarios.to_numpy()
small_matrix = df_small.to_numpy()
print(small_matrix)
print(user_matrix.shape)

hora = 1.0
for posicao in range(np.size(user_matrix, 1)):  # Eixos x e y para cada hora
    if posicao%2 != 0  and posicao!= 0 :
        print(f'Hora {hora}')
        with open('user_position.txt', 'a') as arquivo:
                arquivo.write(f'\nINICIO HORA {hora}\n')
        
        for usuario in range(np.size(user_matrix, 0)):  # Quantidade de usuarios    
            
            if user_matrix[usuario, (posicao - 1)] != 0 and user_matrix[usuario,posicao] != 0:
                print(f'    Comando para escrever a lista ({user_matrix[usuario, (posicao - 1)]},{user_matrix[usuario, posicao]})')

                with open('user_position.txt', 'a') as arquivo:
                    arquivo.write(f'    Comando para escrever a lista ({user_matrix[usuario, (posicao - 1)]},{user_matrix[usuario, posicao]})\n')

        with open('user_position.txt', 'a') as arquivo:
                arquivo.write(f'FIM HORA {hora}\n')
        hora += 1
    


        


# print(user_matrix[29, 10])
# print(np.size(user_matrix, 0))

# for usuario in range(np.size(user_matrix, 0)):
#     print(usuario)
#     if usuario%2 != 0  and usuario!= 0 :
#         print('True')





