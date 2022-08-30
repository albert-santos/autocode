import shutil # biblioteca para copiar arquivos txt
import os #biblioteca para obter informações do sistema
from pathlib import Path
from autocode_smalls import autocode_smalls
from autocode_users import autocode_users
from autocode_bbu_allocation import  autocode_bbu_allocation

print('\n')
linha = '-'*50
print(linha)
print('AUTOCODE PARA NS-3:'.center(50))
print(linha)
while(1):
    modo = str(input('Indique o cenário utilizado (Opcões: SUI ou ECC): '))

    #Caminho para as planilhas de usuários e antenas (SUI-Matlab)
    if modo.strip().upper() == 'SUI':
        planilha_smalls = 'SUI_planilhas/SmallPosition.xls'
        planilha_users = 'SUI_planilhas/UserPosition.xls'
        #Diretório que será passado para o NS-3
        diretorio_ns3 = f'./dir_ns3_SUI/SUI'
        break
    elif modo.strip().upper() == 'ECC':
        planilha_smalls = 'ECC_planilhas/SmallPosition_with_JasmineModel.xls'
        planilha_users = 'ECC_planilhas/UserPosition_with_JasmineModel.xls'
        planilha_bbu_allocation = 'ECC_planilhas/mapping_rrh_bbu_sectors_with_JasmineModel.xls'
        planilha_status_rrh = 'ECC_planilhas/rrhs_status_with_JasmineModel.xls'
        #Diretório que será passado para o NS-3
        diretorio_ns3 = f'./dir_ns3_ECC/ECC'
        break
    else:
        print('\nCENÁRIO INCORRETO! TENTE NOVAMENTE.\n')

number_of_bbus = 6

quantidade_arquivos_flowmon = int(input('Indique a quantidade de arquivos flowmon: '))

#Gerando txt formatado para a alocação RRH-BBU
autocode_bbu_allocation(planilha_bbu_allocation, planilha_status_rrh, number_of_bbus)

#Gerando txt formatado para a alocação de usuários e de antenas
autocode_smalls (planilha_smalls)
autocode_users(planilha_users)

# Obtem o diretório atual
cwd = os.getcwd()


#---CONVERTE O ARQUIVO MAIN DE .CC PARA .TXT
# Caminho do arquivo que serve como base
src = f'{cwd}/arquivo_base/main.cc'
# Novos arquivos em formato cc
dest = f'{cwd}/arquivo_base/main.txt'
shutil.copy(src, dest)
# Caminho do arquivo que serve como base
src_main = f'{cwd}/arquivo_base/main.txt'


#---CONVERTE O ARQUIVO MY-CONTROLLER DE .CC PARA .TXT
# Caminho do arquivo que serve como base
src = f'{cwd}/arquivo_base/my-controller.cc'
# Novos arquivos em formato cc
dest = f'{cwd}/arquivo_base/my-controller.txt'
shutil.copy(src, dest)
# Caminho do arquivo txt do my-controller
src_mycontroller = f'{cwd}/arquivo_base/my-controller.txt'







# Abrindo o arquivo com as antenas
smalls_txt = planilha_smalls.split('.')
smalls_txt = smalls_txt[0]
smalls = open(f'{smalls_txt}.txt')
# Armazenando todas as linhas do arquivo de antenas
content_smalls = smalls.readlines()

#Abrindo o arquivo com o usuário
users_txt = planilha_users.split('.')
users_txt = users_txt[0]
users = open(f'{users_txt}.txt')
# Armazenando todas as linhas do arquivo de usuários
content_users = users.readlines()


# Abrindo o arquivo com a alocação RRH-BBU 
allocation_txt = planilha_bbu_allocation.split('.')
allocation_txt = allocation_txt[0]
allocation = open(f'{allocation_txt}.txt')
# Armazenando todas as linhas do arquivo de antenas
content_allocation = allocation.readlines()


for hora in range(1, quantidade_arquivos_flowmon + 1):

    if modo.strip().upper() == 'SUI':
        # Linha de código do flowmonitor para cada hora
        texto_flowmon = f'	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_{hora}.flowmon", false, false);\n'
        texto_animation = f'	  AnimationInterface anim ("animations/animation_SUI_{hora}.xml");\n'

    if modo.strip().upper() == 'ECC':
        # Linha de código do flowmonitor para cada hora
        texto_flowmon = f'	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_{hora}.flowmon", false, false);\n'
        texto_animation = f'	  AnimationInterface anim ("animations/animation_ECC_{hora}.xml");\n'

    # Passando o caminho de um novo arquivo main para a hora atual
    nova_main = f'{cwd}/arquivos_txt/main_{hora}.txt'
    # Copiando o conteudo do arquivo base para a main da hora atual
    dest = nova_main
    shutil.copy(src_main, dest)

    #Abrindo o arquivo main do ns-3 no formato txt
    arq = f'arquivos_txt/main_{hora}.txt'
    # main = open(arq, 'a')
    with open(arq, 'a') as arquivo:
        pass
    with open(arq, 'r') as arquivo:
        # Armazenando todas as linhas do main 
        content_main = arquivo.readlines()

    # Passando o caminho de um novo arquivo mycontroller para a hora atual
    new_mycontroller = f'{cwd}/arquivos_txt/my-controller-{hora}.txt'
    # Copiando o conteudo do arquivo base para a main da hora atual
    dest = new_mycontroller
    shutil.copy(src_mycontroller, dest)

    #Abrindo o arquivo mycontroller do ns-3 no formato txt
    arq_mycontroller = f'arquivos_txt/my-controller-{hora}.txt'
    # main = open(arq, 'a')
    with open(arq_mycontroller, 'a') as arquivo:
        pass
    with open(arq_mycontroller, 'r') as arquivo:
        # Armazenando todas as linhas do 
        content_mycontroller = arquivo.readlines()


    




    # ANIMATION
    # ---REMOVE LINHAS JÁ EXISTENTES DE ANIMATION---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        if '//AUTOCODE ANIMATION INICIO' in content_main[linha].strip(): #Demarcador da linha de inicio da alocação de usuários
            inicio_animation = linha # armazena a linha de inicio da alocação dos usuários
            # print(linha)

        if '//AUTOCODE ANIMATION FIM' in content_main[linha].strip(): #Demarcador da linha final da alocação de usuários
            fim_animation = linha # armazena a linha final da alocação dos usuários
            # print(linha)

    print(f'Linha inicial do animaton: {inicio_animation}')
    print(f'Linha final do animation: {fim_animation}')

    # Percorrendo somente as linhas de alocação dos usuários
    for linha in range((inicio_animation + 1), fim_animation):
        # Removendo as linhas de alocação que já existem
        content_main.remove(content_main[(inicio_animation + 1)])

    # Limpando o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o aquivo main atual
        with open(arq, 'a') as arquivo:

            # Escrevendo cada linha do content_main (sem as linhas de alocação) para o main.txt
            arquivo.write(content_main[linha])




    # ANIMATION
    # ----ADICIONA LINHAS DE ANIMATION---

    # Armazena cada linha de alocação do txt de usuários para a hora
    texto = texto_animation
    # Insere a linha de alocação acima no content_main
    content_main.insert((inicio_animation+1), texto)

    # Limpando  o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abri o arquivo main atual
        with open(arq, 'a') as arquivo:

            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])
    

    # SMALLS
    # ---REMOVE LINHAS JÁ EXISTENTES DE ALOCAÇÃO DE ANTENAS---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        if '	//AUTOCODE SMALLS INICIO' in content_main[linha]: #Demarcador da linha de inicio da alocação das antenas
            inicio_small = linha # armazena a linha de inicio da alocação das antenas
            # print(linha)

        if '	//AUTOCODE SMALLS FIM' in content_main[linha]: #Demarcador da linha final da alocação das antenas
            fim_small = linha # armazena a linha de fim da alocação das antenas
            # print(linha)

    print(f'Linha inicial da alocação das smalls: {inicio_small}')
    print(f'Linha final da alocação das smalls: {fim_small}')

    # Percorrendo somente as linhas de alocação das antenas
    for linha in range((inicio_small + 1), fim_small):

        # Removendo as linhas de alocação que já existem
        content_main.remove(content_main[(inicio_small + 1)])

    # Limpando o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abrindo o arquivo main atual
        with open(arq, 'a') as arquivo:

            # Escrevendo cada linha do content_main (sem as linhas de alocação) para o main.txt
            arquivo.write(content_main[linha])



    # SMALLS
    # ---ADICIONA LINHAS DE ALOCAÇÃO DE ANTENAS DE ACORDO COM A PLANILHA DE SMALLS---

    #Percorrendo as linhas do arquivo de antenas
    for linha in range(len(content_smalls)):

        #Demarcador da linha de inicio da locação das antenas da hora 
        if f'INICIO HORA {hora}.0' in content_smalls[linha]:
            # Armazena a linha de inicio de alocação da hora
            inicio_hora = linha 

        #Demarcador da linha final de alocação das antenas da hora     
        if f'FIM HORA {hora}.0' in content_smalls[linha]:
            # Armazena a linha final de alocação da hora
            fim_hora = linha
            
    print(f'Inicio da hora para smalls: {inicio_hora}')
    print(f'Fim da hora para smalls: {fim_hora}')

    quantidade_smalls = 0
    # Percorre a quantidade de linhas de alocação da hora 
    for linha in range((inicio_hora + 1), fim_hora):
        # Armazena cada linha de alocação do txt de antenas para a hora
        texto = content_smalls[linha]
        # Insere a linha de alocação acima no content_main
        content_main.insert((inicio_small+1), texto)

        #Contador da quantidade de smalls
        quantidade_smalls += 1

    print(f'Quantidade de smalls da hora {hora}: {quantidade_smalls+1}')
    # Limpa o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o arquivo main atual
        with open(arq, 'a') as arquivo:
            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])






    # USUÁRIOS
    # ---REMOVE LINHAS JÁ EXISTENTES DE ALOCAÇÃO DE USUÁRIOS---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        if '	//AUTOCODE USERS INICIO' in content_main[linha]: #Demarcador da linha de inicio da alocação de usuários
            inicio_user = linha # armazena a linha de inicio da alocação dos usuários
            # print(linha)

        if '	//AUTOCODE USERS FIM' in content_main[linha]: #Demarcador da linha final da alocação de usuários
            fim_user = linha # armazena a linha final da alocação dos usuários
            # print(linha)

    print(f'Linha inicial da alocação dos usuários: {inicio_user}')
    print(f'Linha final da alocação dos usuários: {fim_user}')

    # Percorrendo somente as linhas de alocação dos usuários
    for linha in range((inicio_user + 1), fim_user):
        # Removendo as linhas de alocação que já existem
        content_main.remove(content_main[(inicio_user + 1)])

    # Limpando o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o aquivo main atual
        with open(arq, 'a') as arquivo:

            # Escrevendo cada linha do content_main (sem as linhas de alocação) para o main.txt
            arquivo.write(content_main[linha])





    # USUÁRIOS
    # ----ADICIONA LINHAS DE ALOCAÇÃO DE USUÁRIOS DE ACORDO COM A PLANILHA DE USUÁRIOS---


    #Percorrendo as linhas do arquivo de usuários
    for linha in range(len(content_users)):

        #Demarcador da linha de inicio da locação dos usuários da hora 
        if f'INICIO HORA {hora}.0' in content_users[linha]:

            # Armazena a linha de inicio de alocação da hora
            inicio_hora_usr = linha

        #Demarcador da linha final da locação dos usuários da hora    
        if f'FIM HORA {hora}.0' in content_users[linha]:

            # Armazena a linha de inicio de alocação da hora
            fim_hora_usr = linha
            
    print(f'Inicio da hora para usuários: {inicio_hora_usr}')
    print(f'Fim da hora para usuários: {fim_hora_usr}')

    quantidade_users = 0
    # Percorre a quantidade de linhas de alocação de usuários da hora 
    for linha in range((inicio_hora_usr + 1), fim_hora_usr):

        # Armazena cada linha de alocação do txt de usuários para a hora
        texto = content_users[linha]
        # Insere a linha de alocação acima no content_main
        content_main.insert((inicio_user+1), texto)

        # Contador para armazenar a quantidade de usuários
        quantidade_users += 1

    print(f'Quantidade de usuários da hora {hora}: {quantidade_users+1}')

    # Limpando  o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abri o arquivo main atual
        with open(arq, 'a') as arquivo:

            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])



    








    # ALLOCATION
    # ---REMOVE LINHAS JÁ EXISTENTES DE ALOCAÇÃO DE RRH-BBU---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_mycontroller)):
        if '//AUTOCODE ALLOCATION INICIO' in content_mycontroller[linha].strip(): #Demarcador da linha de inicio da alocação das antenas
            inicio_allocation = linha # armazena a linha de inicio da alocação das antenas
            print(linha)

        if '//AUTOCODE ALLOCATION FIM' in content_mycontroller[linha].strip(): #Demarcador da linha final da alocação das antenas
            fim_allocation = linha # armazena a linha de fim da alocação das antenas
            # print(linha)

    print(f'Linha inicial da alocação das smalls: {inicio_allocation}')
    print(f'Linha final da alocação das smalls: {fim_allocation}')

    # Percorrendo somente as linhas de alocação
    for linha in range((inicio_allocation + 1), fim_allocation):

        # Removendo as linhas de alocação que já existem
        content_mycontroller.remove(content_mycontroller[(inicio_allocation + 1)])

    # Limpando o arquivo mycontroller atual
    with open(arq_mycontroller,'w') as f:
        pass

    #Percorrendo as linhas do arquivo mycontroller
    for linha in range(len(content_mycontroller)):

        # Abrindo o arquivo mycontroller atual
        with open(arq_mycontroller, 'a') as arquivo:

            # Escrevendo cada linha do content_mycontroller (sem as linhas de alocação) para o main.txt
            arquivo.write(content_mycontroller[linha])



    
    # ALLOCATION
    # ----ADICIONA LINHAS DE ALOCAÇÃO DE RRH-BBU DE ACORDO COM A PLANILHA DE ALOCAÇÃO---


    #Percorrendo as linhas do arquivo
    for linha in range(len(content_allocation)):

        #Demarcador da linha de inicio da locação dos usuários da hora 
        if f'INICIO ALLOCATION {hora}.0' in content_allocation[linha]:

            # Armazena a linha de inicio de alocação da hora
            inicio_hora_allocation = linha

        #Demarcador da linha final da locação da hora    
        if f'FIM ALLOCATION {hora}.0' in content_allocation[linha]:

            # Armazena a linha de inicio de alocação da hora
            fim_hora_allocation = linha
            
    print(f'Inicio da hora para alocação: {inicio_hora_allocation}')
    print(f'Fim da hora para alocação: {fim_hora_allocation}')

    quantidade_rrh = 0
    # Percorre a quantidade de linhas de alocação de usuários da hora 
    for linha in range((fim_hora_allocation-1), inicio_hora_allocation, -1):

        # Armazena cada linha de alocação do txt de usuários para a hora
        texto = content_allocation[linha]
        # Insere a linha de alocação acima no content_mycontroller
        content_mycontroller.insert((inicio_allocation+1), texto)

        # Contador para armazenar a quantidade de usuários
        quantidade_rrh += 1

    print(f'Quantidade de rrhs da hora {hora}: {quantidade_rrh+1}')

    # Limpando  o arquivo mycontroller atual
    with open(arq_mycontroller,'w') as f:
        pass

    #Percorrendo as linhas do arquivo mycontroller
    for linha in range(len(content_mycontroller)):

        # Abri o arquivo mycontroller atual
        with open(arq_mycontroller, 'a') as arquivo:

            arquivo.write(content_mycontroller[linha])









    # UPDATE ALLOCATION
    # ---REMOVE LINHAS JÁ EXISTENTES DE ALOCAÇÃO DE RRH-BBU NO MÉTODO UPDATE---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_mycontroller)):
        if '//AUTOCODE UPDATE INICIO' in content_mycontroller[linha].strip(): #Demarcador da linha de inicio da alocação das antenas
            inicio_update = linha # armazena a linha de inicio da alocação das antenas
            print(linha)

        if '//AUTOCODE UPDATE FIM' in content_mycontroller[linha].strip(): #Demarcador da linha final da alocação das antenas
            fim_update = linha # armazena a linha de fim da alocação das antenas
            # print(linha)

    print(f'Linha inicial da alocação das smalls: {inicio_update}')
    print(f'Linha final da alocação das smalls: {fim_update}')

    # Percorrendo somente as linhas de alocação
    for linha in range((inicio_update + 1), fim_update):

        # Removendo as linhas de alocação que já existem
        content_mycontroller.remove(content_mycontroller[(inicio_update + 1)])

    # Limpando o arquivo mycontroller atual
    with open(arq_mycontroller,'w') as f:
        pass

    #Percorrendo as linhas do arquivo mycontroller
    for linha in range(len(content_mycontroller)):

        # Abrindo o arquivo mycontroller atual
        with open(arq_mycontroller, 'a') as arquivo:

            # Escrevendo cada linha do content_mycontroller (sem as linhas de alocação) para o main.txt
            arquivo.write(content_mycontroller[linha])



    
    # UPDATE ALLOCATION
    # ----ADICIONA LINHAS DE ALOCAÇÃO DE RRH-BBU DE ACORDO COM A PLANILHA DE ALOCAÇÃO---


    #Percorrendo as linhas do arquivo
    for linha in range(len(content_allocation)):

        #Demarcador da linha de inicio da locação dos usuários da hora 
        if f'INICIO UPDATE {hora}.0' in content_allocation[linha]:

            # Armazena a linha de inicio de alocação da hora
            inicio_update_allocation = linha

        #Demarcador da linha final da locação da hora    
        if f'FIM UPDATE {hora}.0' in content_allocation[linha]:

            # Armazena a linha de inicio de alocação da hora
            fim_update_allocation = linha
            
    print(f'Inicio da hora para alocação: {inicio_update_allocation}')
    print(f'Fim da hora para alocação: {fim_update_allocation}')

    quantidade_rrh = 0
    # Percorre a quantidade de linhas de alocação de usuários da hora 
    for linha in range((fim_update_allocation-1), inicio_update_allocation, -1):

        # Armazena cada linha de alocação do txt de usuários para a hora
        texto = content_allocation[linha]
        # Insere a linha de alocação acima no content_mycontroller
        content_mycontroller.insert((inicio_update+1), texto)

        # Contador para armazenar a quantidade de usuários
        quantidade_rrh += 1

    print(f'Quantidade de rrhs da hora {hora}: {quantidade_rrh+1}')

    # Limpando  o arquivo mycontroller atual
    with open(arq_mycontroller,'w') as f:
        pass

    #Percorrendo as linhas do arquivo mycontroller
    for linha in range(len(content_mycontroller)):

        # Abri o arquivo mycontroller atual
        with open(arq_mycontroller, 'a') as arquivo:

            arquivo.write(content_mycontroller[linha])









    # FLOWMONITOR
    # ---REMOVE LINHAS JÁ EXISTENTES DO FLOWMONITOR---

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        if '//INICIO FLOW MONITOR' in content_main[linha].strip(): #Demarcador da linha de inicio da alocação de usuários
            inicio_flow = linha # armazena a linha de inicio da alocação dos usuários
            # print(linha)

        if '//FIM FLOW MONITOR' in content_main[linha].strip(): #Demarcador da linha final da alocação de usuários
            fim_flow = linha # armazena a linha final da alocação dos usuários
            # print(linha)

    print(f'Linha inicial do flowmonitor: {inicio_flow}')
    print(f'Linha final do flowmonitor: {fim_flow}')

    # Percorrendo somente as linhas de alocação dos usuários
    for linha in range((inicio_flow + 1), fim_flow):
        # Removendo as linhas de alocação que já existem
        content_main.remove(content_main[(inicio_flow + 1)])

    # Limpando o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o aquivo main atual
        with open(arq, 'a') as arquivo:

            # Escrevendo cada linha do content_main (sem as linhas de alocação) para o main.txt
            arquivo.write(content_main[linha])






    # FLOWMONITOR
    # ----ADICIONA LINHAS DE FLOWMONITOR---

    # Armazena cada linha de alocação do txt de usuários para a hora
    texto = texto_flowmon
    # Insere a linha de alocação acima no content_main
    content_main.insert((inicio_flow+1), texto)

    # Limpando  o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abri o arquivo main atual
        with open(arq, 'a') as arquivo:

            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])






    # NÚMERO DE RRHS E DE USUÁRIOS
    # --- REMOVE LINHAS DE CONFIGURAÇÃO DO Nº DE USUÁRIOS E DE RRHS ----

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        if '//AUTOCODE NUMBEROFRRHS INICIO' in content_main[linha].strip(): #Demarcador da linha de inicio da configuração do número de RRHs
            inicio_numero_rrhs = linha # armazena a linha de inicio da configuração do número de RRHs

        if '//AUTOCODE NUMBEROFRRHS FIM' in content_main[linha].strip(): #Demarcador da linha final da configuração do número de RRHs
            fim_numero_rrhs = linha # armazena a linha final da configuração do número de RRHs

        if '//AUTOCODE NUMBEROFUSERS INICIO' in content_main[linha].strip(): #Demarcador de início final da configuração do número de usuários
            inicio_numero_users = linha # armazena a linha de início da configuração do número de usuários

        if '//AUTOCODE NUMBEROFUSERS FIM' in content_main[linha].strip(): #Demarcador da linha final da configuração do número de usuários
            fim_numero_users = linha # armazena a linha final da configuração do número de usuários

    print(f'Linha inicial do Nº de RRHs: {inicio_numero_rrhs}')
    print(f'Linha final do Nº de RRHs: {fim_numero_rrhs}')

    print(f'Linha inicial do Nº de usuários: {inicio_numero_users}')
    print(f'Linha final do Nº de usuários: {fim_numero_users}\n')

    # Percorrendo somente as linhas da configuração do número de RRHs
    for linha in range((inicio_numero_rrhs + 1), fim_numero_rrhs):
        # Removendo as linhas de configuração do número de RRHs  que já existem
        content_main.remove(content_main[(inicio_numero_rrhs + 1)])

    # Percorrendo somente as linhas da configuração do número de usuários
    for linha in range((inicio_numero_users + 1), fim_numero_users):
        # Removendo as linhas de configuração do número de usuários  que já existem
        content_main.remove(content_main[(inicio_numero_users)])

    # Limpando o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o aquivo main atual
        with open(arq, 'a') as arquivo:

            # Escrevendo cada linha do content_main (sem as linhas de configuração do nº de RRHs) para o main.txt
            arquivo.write(content_main[linha])



    # NÚMERO DE RRHS E DE USUÁRIOS
    # --- ADICIONA LINHAS DE CONFIGURAÇÃO DO Nº DE USUÁRIOS E DE RRHS ---

    # Armazena cada linha de alocação do txt de usuários para a hora
    texto_numero_rrhs = f'	  uint16_t numberOfRrhs = {quantidade_smalls};\n'
    texto_numero_users = f'	  uint16_t numberOfNodes = {quantidade_users};\n'
    # Insere a linha de alocação acima no content_main
    content_main.insert((inicio_numero_rrhs +1), texto_numero_rrhs)
    content_main.insert((inicio_numero_users +1), texto_numero_users)

    # Limpando  o arquivo main atual
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abri o arquivo main atual
        with open(arq, 'a') as arquivo:

            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])







# --- CONVERTER ARQUIVOS TXT PARA CC ---
for i in range(1, quantidade_arquivos_flowmon+1):

    # Caminho do arquivo que serve como base
    src = f'{cwd}/arquivos_txt/main_{i}.txt'
    # Novos arquivos em formato cc
    dest = f'{cwd}/arquivos_cc/main_{i}.cc'
    shutil.copy(src, dest)

    # Caminho do arquivo que serve como base
    src = f'{cwd}/arquivos_txt/my-controller-{i}.txt'
    # Novos arquivos em formato cc
    dest = f'{cwd}/arquivos_cc/my-controller-{i}.cc'
    shutil.copy(src, dest)



# ORGANIZAR OS DIRETÓRIOS PARA O NS-3
# --- DIRETÓRIOS PARA O NS-3 ---
for i in range(1, quantidade_arquivos_flowmon+1):
    # Criando diretórios para armazenar os arquivos necessários para executar no NS-3
    try:
        diretorio = f'{diretorio_ns3}_{i}'
        os.makedirs(diretorio)
    except:
        pass

    # Caminho do arquivo cc que será copiado
    src = f'arquivos_cc/main_{i}.cc'
    # Caminho de destino da cópia do arquivo cc
    dest = diretorio
    # Copiando o arquivo
    shutil.copy(src, dest)

    # Caminho do arquivo cc que será copiado
    src = f'arquivos_cc/my-controller-{i}.cc'
    # Caminho de destino da cópia do arquivo cc
    dest = diretorio
    # Copiando o arquivo
    shutil.copy(src, dest)

    # Caminho do arquivo cc que será copiado
    src = f'biblioteca_ns3/my-controller.h'
    # Caminho de destino da cópia do arquivo cc
    dest = diretorio
    # Copiando o arquivo
    shutil.copy(src, dest)
