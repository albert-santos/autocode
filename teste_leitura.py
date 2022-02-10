
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


for linha in range(len(content_main)):
    if '	//AUTOCODE SMALLS INICIO' in content_main[linha]:
        inicio_small = linha
        # print(linha)

    if '	//AUTOCODE SMALLS FIM' in content_main[linha]:
        fim_small = linha
        # print(linha)

print(f'Inicio small: {inicio_small}')
print(f'Fim small: {fim_small}')

for linha in range((inicio_small + 1), fim_small):
    content_main.remove(content_main[(inicio_small + 1)])


with open("main.txt",'w') as f: # Limpando  o arquivo
    pass

for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])



for linha in range(len(content_smalls)):
    if 'INICIO HORA 1.0' in content_smalls[linha]:
        inicio_hora = linha
        
    if 'FIM HORA 1.0' in content_smalls[linha]:
        fim_hora = linha
        
print(f'Inicio hora: {inicio_hora}')
print(f'Fim hora: {fim_hora}')

for linha in range((inicio_hora + 1), fim_hora):
    texto = content_smalls[linha]
    content_main.insert((inicio_small+1), texto)


with open("main.txt",'w') as f: # Limpando  o arquivo
    pass

for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])









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

for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])


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

for linha in range(len(content_main)):
    with open('main.txt', 'a') as arquivo:
        arquivo.write(content_main[linha])