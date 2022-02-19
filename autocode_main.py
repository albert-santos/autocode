import shutil # biblioteca para copiar arquivos txt
import os #biblioteca para obter informações do sistema
from autocode_smalls import autocode_smalls
from autocode_users import autocode_users

autocode_smalls ('SmallPosition.xls')
autocode_users('UserPosition.xls')

diretorio_ns3 = f'./dir_ns3_SA/'
#diretorio_ns3 = f'./dir_ns3_HDSO/'

# Obtem o diretório atual
cwd = os.getcwd()

# Caminho do arquivo que serve como base
src = f'{cwd}/main.txt'

# Abrindo o arquivo com as antenas
smalls = open('small_position.txt')
# Armazenando todas as linhas do arquivo de antenas
content_smalls = smalls.readlines()

#Abrindo o arquivo com o usuário
users = open('user_position.txt')
# Armazenando todas as linhas do arquivo de usuários
content_users = users.readlines()


for hora in range(1, 25):

    # Passando o caminho de um novo arquivo main para a hora atual
    nova_main = f'{cwd}/arquivos_txt/main_{hora}.txt'

    # Copiando o conteudo do arquivo base para a main da hora atual
    dest = nova_main
    shutil.copy(src, dest)

    #Abrindo o arquivo main do ns-3 no formato txt
    arq = f'arquivos_txt/main_{hora}.txt'
    # main = open(arq, 'a')
    with open(arq, 'a') as arquivo:
        arquivo.write(f'')

    with open(arq, 'r') as arquivo:
        # Armazenando todas as linhas do main 
        content_main = arquivo.readlines()

    

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
    texto = f'	  flowmon->SerializeToXmlFile ("scratch/SA_{hora}/switch_SA{hora}.flowmon", false, false);\n'
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
    texto_numero_rrhs = f'	  uint16_t numberOfRrhs = {quantidade_smalls+1};\n'
    texto_numero_users = f'	  uint16_t numberOfNodes = {quantidade_users+1};\n'
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
for i in range(1, 25):

    # Caminho do arquivo que serve como base
    src = f'{cwd}/arquivos_txt/main_{i}.txt'

    # Novos arquivos em formato cc
    dest = f'{cwd}/arquivos_cc/main_{i}.cc'
    shutil.copy(src, dest)





# --- DIRETÓRIOS PARA O NS-3 ---
for i in range(1, 25):
    # Criando diretórios para armazenar os arquivos necessários para executar no NS-3
    try:
        diretorio = f'{diretorio_ns3}SA_{i}'
        os.makedirs(diretorio)
    except:
        pass

    # Caminho do arquivo cc que será copiado
    src = f'arquivos_cc/main_{i}.cc'
    # Caminho de destino da cópia do arquivo cc
    dest = diretorio
    # Copiando o arquivo
    shutil.copy(src, dest)

    # Copiar as bilbiotecas my_controller para os diretórios NS-3
    src_biblioteca1 = 'my-controller.cc'
    shutil.copy(src_biblioteca1, dest)
    src_biblioteca2 = 'my-controller.h'
    shutil.copy(src_biblioteca2, dest)