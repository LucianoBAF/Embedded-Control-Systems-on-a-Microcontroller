# Electromagnetic control system on a microcontroller
Este repositório tem como funcionalidade armazenar os arquivos do projeto de controle de momvimento linear de uma bobina ao longo de um cilindro ferromagnético, operando em loop fechado. Abaixo há uma descrição de cada arquivo.
<table>
  <tr>
    <th>Nome do arquivo</th>
    <th>Descrição</th>
  </tr>
  <tr>
    <td>Bibliotecas Arduino.rar</td>
    <td>Biblioteca para utilização do Timer1 e leitura do sensor ultrassônica HC-SR04</td>
  </tr>
  <tr>
    <td>PRCSS_CSV.rar</td>
    <td>Programa em Processing para ler dados no padrão CSV via serial</td>
  </tr>
  <tr>
    <td>Projeto controle - Eagle.rar</td>
    <td>Projeto do eagle com esquemático e pcb</td>
  </tr>
  <tr>
    <td>YOUTUBE-Arduino - controlador geral.rar</td>
    <td>Todo material necessário para se controlar o projeto pelo Arduino</td>
  </tr>
  <tr>
    <td>YOUTUBE-Matlab e Arduino - Teste senoides.rar</td>
    <td>Todo material necessário para se obter a função de transferência do sistema através do teste das senoides</td>
  </tr>
  <tr>
    <td>motor_de_passo.rar</td>
    <td>Programa para arduino que controla um motor de passo desde um â inicial até 90º, depois até 0º e fica repetindo 0º a 90º</td>
  </tr>
    <tr>
    <td>projeto_eletromagnetismo.ino</td>
    <td>Projeto em arduino. Falta implementar contador para amostragens de controle iguais, além da lógica de manipulação de posição por joystick referente à parte de "um movimento até um extremo e posterior retorno ao centro do joystick" = alterção de 1cm. Além disso está pendente a comunicação entre arduino e matlab em tempo real </td>
  </tr>
    <tr>
    <td>projeto_eletromagnetismo_atmel.rar</td>
    <td>Projeto em Atmel Studio 7. Falta implementar contador para amostragens de controle iguais </td>
  </tr>
</table>
