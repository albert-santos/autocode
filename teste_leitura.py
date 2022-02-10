
#Abrindo o arquivo main do ns-3 no formato txt
main = open('main.txt')
# Armazenando todas as linhas do main 
content_main = main.readlines()

# Abrindo o arquivo com as antenas
smalls = open('small_position.txt')
# Armazenando todas as linhas do arquivo de antenas
content_smalls = smalls.readlines()

#Abrindo o arquivo com o usuário
users = open('user_position.txt')
# Armazenando todas as linhas do arquivo de usuários
content_users = users.readlines()


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
with open("main.txt",'w') as f:
    pass

#Percorrendo as linhas do arquivo main
for linha in range(len(content_main)):

    # Abrindo o arquivo main
    with open('main.txt', 'a') as arquivo:

        # Escrevendo cada linha do content_main para o main.txt
        arquivo.write(content_main[linha])


# ---ADICIONA LINHAS DE ALOCAÇÃO DE ANTENAS DE ACORDO COM A PLANILHA DE SMALLS---

#Percorrendo as linhas do arquivo de antenas
for linha in range(len(content_smalls)):

    #Demarcador da linha de inicio da locação das antenas da hora 
    if 'INICIO HORA 1.0' in content_smalls[linha]:
        # Armazena a linha de inicio de alocação da hora
        inicio_hora = linha 

    #Demarcador da linha final de alocação das antenas da hora     
    if 'FIM HORA 1.0' in content_smalls[linha]:
        # Armazena a linha final de alocação da hora
        fim_hora = linha
        
print(f'Inicio da hora: {inicio_hora}')
print(f'Fim da hora: {fim_hora}')

# Percorre a quantidade de linhas de alocação da hora 
for linha in range((inicio_hora + 1), fim_hora):
    # Armazena cada linha de alocação do txt de antenas para a hora
    texto = content_smalls[linha]
    # Insere a linha de alocação acima no content_main
    content_main.insert((inicio_small+1), texto)

# Limpa o arquivo main.txt
with open("main.txt",'w') as f: # Limpando  o arquivo
    pass

#Percorrendo as linhas do arquivo main
for linha in range(len(content_main)):
    # Abre o arquivo main
    with open('main.txt', 'a') as arquivo:
        # Escreve cada linha do content_main no main.txt
        arquivo.write(content_main[linha])








#Percorrendo as linhas do arquivo main
for linha in range(len(content_main)):
    if '	//AUTOCODE USERS INICIO' in content_main[linha]:
        inicio_user = linha
        # print(linha)

    if '	//AUTOCODE USERS FIM' in content_main[linha]:
        fim_user = linha
        # print(linha)

print(f'Inicio user: {inicio_user}')
print(f'Fim user: {fim_user}')

for linha in range((inicio_user + 1), fim_user):
    content_main.remove(content_main[(inicio_user + 1)])


with open("main.txt",'w') as f: # Limpando  o arquivo
    pass

#Percorrendo as linhas do arquivo main
for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])

#Percorrendo as linhas do arquivo de usuários
for linha in range(len(content_users)):
    if 'INICIO HORA 1.0' in content_users[linha]:
        inicio_hora_usr = linha
        
    if 'FIM HORA 1.0' in content_users[linha]:
        fim_hora_usr = linha
        
print(f'Inicio hora usr: {inicio_hora_usr}')
print(f'Fim hora usr: {fim_hora_usr}')

for linha in range((inicio_hora_usr + 1), fim_hora_usr):
    texto = content_users[linha]
    content_main.insert((inicio_user+1), texto)


with open("main.txt",'w') as f: # Limpando  o arquivo
    pass

#Percorrendo as linhas do arquivo main
for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])