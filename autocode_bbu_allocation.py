import pandas as pd
import numpy as np

def autocode_bbu_allocation(planilha_bbu_allocation, planilha_rrhs_status, number_of_bbus):

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
        

        print(f'Hora {float(hora)}')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                arquivo.write(f'\nINICIO HORA {float(hora)}\n')

        print(f'ALLOCATION {float(hora)}')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                arquivo.write(f'\n\tINICIO ALLOCATION {float(hora)}\n\n')
        
        # -------------- IMPLEMENTA VARIÁVEIS DE CONEXÃO DAS RRHS --------
        #
        #  Ex: uint32_t connect_RRH_{ ID da RRH } = 0;

        rrh_counter = 1
        # Percorre todas RRHs
        for rrh_indice in range(np.size(bbu_allocation_matrix, 1)):  
            
            status = rrh_status_matrix[hora-1, rrh_indice]
            
            if   status == 1:
                # print(f'continue: {bbu_allocation_matrix[hora-1, rrh_indice]}')
                print(f'\tuint32_t connect_RRH_{rrh_counter} = 0;')
                with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                        arquivo.write(f'\tuint32_t connect_RRH_{rrh_counter} = 0;\n')

                rrh_counter += 1

        print('\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\n')

        #-----------------------------------------------------------------------------
        



        # -------------- IMPLEMENTA VARIÁVEIS DE CONEXÃO DAS BBUS --------
        #
        # Ex: uint32_t connect_bbu_{ ID da BBU } = 0;

        # Percorre todas BBUs e cria variáveis necessárias para o ns-3 nomeadas:
        # uint32_t connect_BBU_{Id da rrh}
        for bbu_indice in range(1, number_of_bbus + 1):
            print(f'\tuint32_t connect_bbu_{bbu_indice} = 0;')
            with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\tuint32_t connect_bbu_{bbu_indice} = 0;\n')

        #-----------------------------------------------------------------------------



        # -------------- INICÍCIO DA ALOCAÇÃO RRH-BBU --------
        
        # Inicio do for
        print('\n\tfor (std::map<uint64_t, uint64_t >::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it) \n\t{\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\n\tfor (std::map<uint64_t, uint64_t >::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it) \n\t{\n')
        
        # Comentário do código
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\n\t\t//Associando cada antena a uma BBU de acordo com a alocação teste\n')
        
        # Início do Switch case
        print('\n\t\tswitch (it->second)\n\t\t{\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\n\t\tswitch (it->second)\n\t\t{\n')


        rrh_counter = 1
        # Percorre todas RRHs e realiza a alocação
        for rrh_indice in range(np.size(bbu_allocation_matrix, 1)):  

            status = rrh_status_matrix[hora-1, rrh_indice]
            
            if   status == 1:
                with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\t\t\tcase {rrh_counter}:\n')
                    arquivo.write(f'\t\t\t\tconnect_RRH_{rrh_counter}++;\n')
                    arquivo.write(f'\t\t\t\tmymap3[m_mymap[it->first]]= {bbu_allocation_matrix[hora-1, rrh_indice]};\n')
                    arquivo.write(f'\t\t\t\tconnect_bbu_{bbu_allocation_matrix[hora-1, rrh_indice]}++;\n')
                    arquivo.write(f'\t\t\t\t//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: {bbu_allocation_matrix[hora-1, rrh_indice]}"<<std::endl;\n')
                    arquivo.write('\t\t\t\tbreak;\n')

                rrh_counter += 1

            
        # Alocação da Macro. Uma macro é representada por quatro eBn com antenas direcionadas de forma diferente
        for rrh_indice in range(4): 
                with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\t\t\tcase {rrh_counter}:\n')
                    arquivo.write(f'\t\t\t\tmymap3[m_mymap[it->first]]= 6 ;\n')
                    arquivo.write(f'\t\t\t\tconnect_bbu_6++;\n')
                    arquivo.write(f'\t\t\t\t//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 6"<<std::endl;\n')
                    arquivo.write('\t\t\t\tbreak;\n')

                rrh_counter += 1



        # Fim do switch case e do for
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\t\t\tdefault:\n')
                    arquivo.write(f'\t\t\t\tmymap3[m_mymap[it->first]]= {number_of_bbus+1};\n')
                    arquivo.write('\t\t\t\t//std::cout<<"ip: "<<m_mymap[it->first]<<" não associado"<<std::endl;\n')
                    arquivo.write('\t\t\t\tbreak;\n')
                    arquivo.write('\t\t}\n')
                    arquivo.write('\t}\n')



        # Código para exibir os mapas de alocação
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:


            arquivo.write('\n\tstd::cout <<" MAPA 1: IMSI X IP" << std::endl;\n')
            arquivo.write('\tfor (std::map<uint64_t, Ipv4Address>::iterator it=m_mymap.begin(); it!=m_mymap.end(); ++it)\n')
            arquivo.write('\t\tstd::cout <<"imsi: "<< it->first << " ip address: " << it->second << std::endl;\n\n')

            arquivo.write('\tstd::cout <<" MAPA 2: IMSI X RRH_ID"<< std::endl;\n')
            arquivo.write('\tfor (std::map<uint64_t, uint64_t>::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it)\n')
            arquivo.write('\t\tstd::cout <<"imsi: "<< it->first << " connected to cellid: " << it->second << std::endl; \n\n')

            arquivo.write('\tstd::cout <<" MAPA 3: IP X BBU"<< std::endl;\n')
            arquivo.write('\tfor (std::map<Ipv4Address, uint64_t >::iterator it=mymap3.begin(); it!=mymap3.end(); ++it)\n')
            arquivo.write('\t\tstd::cout <<"ip address: "<< it->first << " bbu: " << it->second << std::endl; \n\n')
            


        # Código para exibir a quantidade de usuários em cada RRH
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\tstd::cout << " " << std::endl;\n')
                    arquivo.write('\tstd::cout <<" Qtd de Usuários por RRH"<< std::endl;\n')

        rrh_counter = 1
        for rrh_indice in range(np.size(bbu_allocation_matrix, 1)):  

            status = rrh_status_matrix[hora-1, rrh_indice]
            
            if status == 1:
                with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\tstd::cout <<"RRH {rrh_counter}: " << connect_RRH_{rrh_counter} << " usuários" << std::endl;\n')


                rrh_counter += 1

        
        # Código para exibir a quantidade de usuários por BBU
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\n\tstd::cout << " " << std::endl;\n')

        for bbu_indice in range(1, number_of_bbus + 1):
            print(f'\tuint32_t connect_bbu_{bbu_indice} = 0;')
            with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\tstd::cout <<"BBU {bbu_indice}: " << connect_bbu_{bbu_indice} << " usuários" << std::endl;\n')
	    

        # Fim do método allocation
        print(f'FIM ALLOCATION {float(hora)}\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\n\tFIM ALLOCATION {float(hora)}\n\n')
                    

        # -------------- FIM DA ALOCAÇÃO RRH-BBU --------
    
    
    

	    # -------------- INÍCIO DO MÉTODO UPDATE PARA ALOCAÇÃO RRH-BBU --------
        # Esse método atualiza a alocação caso ocorra handover

        print(f'\n\tINICIO UPDATE {float(hora)}\n\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\tINICIO UPDATE {float(hora)}\n')

        # Início do Switch case
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\n\tswitch (cellid)\n\t\t{\n')


        rrh_counter = 1
        # Percorre todas RRHs e realiza a alocação
        for rrh_indice in range(np.size(bbu_allocation_matrix, 1)):  

            status = rrh_status_matrix[hora-1, rrh_indice]
            
            if   status == 1:
                with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\t\t\tcase {rrh_counter}:\n')
                    arquivo.write(f'\t\t\t\tmymap3[m_mymap[imsi]]= {bbu_allocation_matrix[hora-1, rrh_indice]};\n')
                    arquivo.write(f'\t\t\t\t//std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: {bbu_allocation_matrix[hora-1, rrh_indice]} (Handover)"<<std::endl;\n')
                    arquivo.write('\t\t\t\tbreak;\n')

                rrh_counter += 1


        
        # Fim do switch case e do for
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write('\t\t\tdefault:\n')
                    arquivo.write(f'\t\t\t\tmymap3[m_mymap[imsi]]= {number_of_bbus+1};\n')
                    arquivo.write('\t\t\t\tstd::cout<<"ip: "<<m_mymap[imsi]<<" não associado"<<std::endl;\n')
                    arquivo.write('\t\t\t\tbreak;\n')
                    arquivo.write('\t\t}\n')



        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'\tFIM UPDATE {float(hora)}\n\n') 
        
        # ------------------------------------------------------------------------------
        
        
        
        print(f'FIM HORA {hora}\n')
        with open(f"{arquivo_txt}.txt", 'a') as arquivo:
                    arquivo.write(f'FIM HORA {float(hora)}\n')
        hora += 1
