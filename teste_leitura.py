import shutil

src = r'/home/lpo_albert/Documentos/meus_codigos/autocode/main.txt'
dest = r'/home/lpo_albert/Documentos/meus_codigos/autocode/main_1.txt'
shutil.copy(src, dest)

# Abrindo o arquivo com as antenas
smalls = open('small_position.txt')
# Armazenando todas as linhas do arquivo de antenas
content_smalls = smalls.readlines()

#Abrindo o arquivo com o usuário
users = open('user_position.txt')
# Armazenando todas as linhas do arquivo de usuários
content_users = users.readlines()


for hora in range(1, 25):

    #Abrindo o arquivo main do ns-3 no formato txt
    arq = f'main_{hora}.txt'
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

    # Limpando o arquivo main.txt
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abrindo o arquivo main
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

    # Percorre a quantidade de linhas de alocação da hora 
    for linha in range((inicio_hora + 1), fim_hora):
        # Armazena cada linha de alocação do txt de antenas para a hora
        texto = content_smalls[linha]
        # Insere a linha de alocação acima no content_main
        content_main.insert((inicio_small+1), texto)

    # Limpa o arquivo main.txt
    with open(arq,'w') as f: # Limpando  o arquivo
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o arquivo main
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

    # Limpando o arquivo main.txt
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):
        # Abre o aquivo main.txt
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

    # Percorre a quantidade de linhas de alocação de usuários da hora 
    for linha in range((inicio_hora_usr + 1), fim_hora_usr):

        # Armazena cada linha de alocação do txt de usuários para a hora
        texto = content_users[linha]
        # Insere a linha de alocação acima no content_main
        content_main.insert((inicio_user+1), texto)

    # Limpando  o arquivo main.txt
    with open(arq,'w') as f:
        pass

    #Percorrendo as linhas do arquivo main
    for linha in range(len(content_main)):

        # Abri o arquivo main.txt
        with open(arq, 'a') as arquivo:

            # Escreve cada linha do content_main no main.txt
            arquivo.write(content_main[linha])