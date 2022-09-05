import pandas as pd
import numpy as np

def autocode_users(planilha_users):

    arquivo_txt = planilha_users.split('.')
    arquivo_txt = arquivo_txt[0]

    with open(f"{arquivo_txt}.txt",'w') as f: # Limpando  o arquivo
        pass

    user_position = planilha_users

    df_usuarios = pd.read_excel(user_position, header=None)

    user_matrix = df_usuarios.to_numpy()
    print(user_matrix.shape)

    hora = 1.0
    for posicao in range(np.size(user_matrix, 1)):  # Eixos x e y para cada hora
        if posicao%2 != 0  and posicao!= 0 :
            print(f'Hora {hora}')
            with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\nINICIO HORA {hora}\n')
            
            for usuario in range(np.size(user_matrix, 0)):  # Quantidade de usuarios    
                
                if user_matrix[usuario, (posicao - 1)] != 0 and user_matrix[usuario,posicao] != 0:
                    print(f'    positionAlloc4->Add (Vector ({user_matrix[usuario, (posicao - 1)]},{user_matrix[usuario, posicao]}, 0.0));')
                    #positionAlloc4->Add (Vector ( 406, 32, 0));
                    with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                        arquivo.write(f'    positionAlloc4->Add (Vector({user_matrix[usuario, (posicao - 1)]},{user_matrix[usuario, posicao]}, 0.0));\n')

            with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'FIM HORA {hora}\n')
            hora += 1

