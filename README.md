Universidade de Brasília
Departamento de Ciência da Computação
Transmissão de Dados

Chat utilizando Socket TCP
    
Alunos: 
	Nathalya Torres Córdova – 10/0018513
        Pedro Matheus F. dos Santos  - 13/0060674
        Vitor Alves Duarte - 13/001846


Compilação do programa:

gcc nhem3.c -o nhem3

Execução:

* As entradas esperadas para o programa são: 1- ./nhem3 -S [port]
  					     2- ./nhem3 host [port]

	-1 se refere a execução do lado servidor e 2 a execução do lado cliente;
	-A porta deve ser informada em [port], caso não seja informada assume-se uma porta padrão: 9696.


* A comunicação é estabelecida a partir do momento em que é digitado no cliente o comando "OI SRV" e é recebida a resposta do servidor "OI CLT";
* Para finalizar a comunicação é necessario digitar no servidor o comando "XAU CLT";
* A comunicação é dada de forma intercalada, ou seja cada terminal envia uma mensagem após ter recebido uma mensagem anterior. Caso sejam digitadas várias mensagens em sequencia, elas serão bufferizadas e transmitidas nas próximas oportunidades.


