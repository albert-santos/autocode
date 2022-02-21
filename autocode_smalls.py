import pandas as pd
import numpy as np

def autocode_smalls (planilha_small):

    arquivo_txt = planilha_small.split('.')
    arquivo_txt = arquivo_txt[0]

    with open(f"{arquivo_txt}.txt",'w') as f: # Limpando  o arquivo
        pass

    small_position = planilha_small

    df_small = pd.read_excel(small_position)
    print(df_small)
    small_matrix = df_small.to_numpy()
    print(small_matrix)
    print(small_matrix.shape)
    print(np.size(small_matrix, 1))

    hora = 1.0
    for posicao in range(np.size(small_matrix, 1)):  # Eixos x e y para cada hora
        if posicao%2 != 0  and posicao!= 0 :
            print(f'INICIO HORA {hora}')
            with open(f'{arquivo_txt}.txt', 'a') as arquivo:
                    arquivo.write(f'\nINICIO HORA {hora}\n')
            
            for small in range(0, np.size(small_matrix, 0)):  # Quantidade de smalls    
                
                if small==0:
                    print(f'    positionAlloc->Add (Vector ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]}, 0.0));')
                    # positionAlloc->Add (Vector (  0,  0, 0));
                    with open(f'{arquivo_txt}.txt', 'a') as arquivo:
                        arquivo.write(f'    positionAlloc->Add (Vector ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]}, 0.0));\n')

                elif small_matrix[small, (posicao - 1)] != 0 or small_matrix[small,posicao] != 0 :
                    print(f'    positionAlloc->Add ( Vector ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]}, 0.0));')

                    with open(f'{arquivo_txt}.txt', 'a') as arquivo:
                        arquivo.write(f'    positionAlloc->Add (Vector ({small_matrix[small, (posicao - 1)]},{small_matrix[small, posicao]}, 0.0));\n')
            
            with open(f'{arquivo_txt}.txt', 'a') as arquivo:
                    arquivo.write(f'FIM HORA {hora}\n')
            hora += 1
