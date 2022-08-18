import pandas as pd
import numpy as np

def autocode_bbu_allocation(planilha_bbu_allocation, planilha_rrhs_status):

    arquivo_txt = planilha_bbu_allocation.split('.')
    arquivo_txt = arquivo_txt[0]

    with open(f"{arquivo_txt}.txt",'w') as f: # Limpando  o arquivo
        pass

    bbu_allocation = planilha_bbu_allocation
    df_bbu_allocation = pd.read_excel(bbu_allocation, header = None)
    bbu_allocation_matrix = df_bbu_allocation.to_numpy()
    print(bbu_allocation_matrix.shape)
    # print(np.size(bbu_allocation_matrix, 1))

    rrhs_status = planilha_rrhs_status
    df_planilha_rrhs_status = pd.read_excel(rrhs_status, header=None)
    rrh_status_matrix = df_planilha_rrhs_status.to_numpy()
    print(f'Shape do vetor de status das RRHs {rrh_status_matrix.shape}')

    

    
    for hora in range(1, np.size(bbu_allocation_matrix, 0) + 1): # Número de linhas que representa as horas
        
      
        print(f'Hora {hora}')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                arquivo.write(f'\nINICIO HORA {hora}\n')
        
        # Percorre todas RRHs
        for rrh_indice in range(np.size(bbu_allocation_matrix, 1)):  

            status = rrh_status_matrix[hora-1, rrh_indice]
            if   status == 1:
                print(f'continue: {bbu_allocation_matrix[hora-1, rrh_indice]}')

                # Implementar código que vai para o NS-3 ------------
            
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'FIM HORA {hora}\n')
        hora += 1




autocode_bbu_allocation('ECC_planilhas/mapping_rrh_bbu_sectors_with_JasmineModel.xls', 'ECC_planilhas/rrhs_status_with_JasmineModel.xls')