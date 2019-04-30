 /*
  * Código para Passa ou Repassa Eletrônico
  * 
  * Autores:
  * Denilson Gomes Vaz da Silva   <denilsongomes@alu.ufc.br>
  * Graduando em Engenharia da Computação - UFC
  * Cassio Viera Simplicio
  * Graduando em Mecatrônica - IFCE
 */

// -- Bibliotecas --
#include <TimerOne.h>

// -- Variaveis e Constantes --
int botAzul = 2; //Botao da equipe Azul
int botVermelho = 3; //Botao da equipe Vermelho
int ledAzul = A0; //Variavel para led azul
int incrementAzul = A1; //Variavel para incrementar pontuação azul
int buzzer = A2; //Variavel para sinal sonoro

int incrementVermelho = A4; //Botao para incrementar pontuação dos vermelhos
int ledVermelho = A3; //Variavel para led vermelho

volatile byte pontosAzul = 0; //Variavel para pontuação da equipe Azul
volatile byte pontosVermelho = 0; //Variavel para pontuação da equipe Vermelha
int tempo = 5000; //Tempo que o participante tem para responder a pergunta após apertar o botão

volatile int som; //Variavel para controlar o sinal sonoro
volatile int estVermelho; //Variavel para controlar o sinal visual da equipe Vermelha
volatile int estAzul; //Variavel para controlar o sinal visual da equipe Azul

int displayAzul = 5; //Variavel para ativar display Azul
int displayVermelho = 13; //Variavel para ativar display Vermelho

byte seven_seg_digits[10][7] = {
 //Para leds acionando em HIGH (Display catodo Comum)
 { 1,1,1,1,1,1,0 },  //DIGITO 0 
 { 0,1,1,0,0,0,0 },  //DIGITO 1
 { 1,1,0,1,1,0,1 },  //DIGITO 2
 { 1,1,1,1,0,0,1 },  //DIGITO 3
 { 0,1,1,0,0,1,1 },  //DIGITO 4
 { 1,0,1,1,0,1,1 },  //DIGITO 5
 { 1,0,1,1,1,1,1 },  //DIGITO 6
 { 1,1,1,0,0,0,0 },  //DIGITO 7
 { 1,1,1,1,1,1,1 },  //DIGITO 8
 { 1,1,1,0,0,1,1 },  //DIGITO 9
};

void setup() {

  Serial.begin(9600); //Habilita comunicação Serial
  
  //Saídas
  pinMode(ledAzul, OUTPUT); //Led azul como saida
  pinMode(buzzer, OUTPUT); //Buzzer como saida
  pinMode(ledVermelho, OUTPUT); //Led vermelho como saida
  pinMode(displayAzul, OUTPUT); //Sinal para ativar displayAzul
  pinMode(displayVermelho, OUTPUT); //Sinal para ativar displayVermelho
  
  //Seguimentos dos Displays
  //Caso os pinos dos seguimentos mudem é preciso rever a função
  pinMode(12, OUTPUT); //PINO 12 -> SEGMENTO A  
  pinMode(11, OUTPUT); //PINO 11 -> SEGMENTO B
  pinMode(10, OUTPUT); //PINO 10 -> SEGMENTO C
  pinMode(9, OUTPUT); //PINO 9 -> SEGMENTO D
  pinMode(8, OUTPUT); //PINO 8 -> SEGMENTO E
  pinMode(7, OUTPUT); //PINO 7 -> SEGMENTO F
  pinMode(6, OUTPUT); //PINO 6 -> SEGMENTO G
  
  //Entradas
  pinMode(incrementAzul, INPUT_PULLUP); //Botão incrementa pontos azul como entrada
  pinMode(incrementVermelho, INPUT_PULLUP); //Botão incrementa pontos vermelho como entrada
  pinMode(botAzul, INPUT_PULLUP); //Botão azul como entrada
  pinMode(botVermelho, INPUT_PULLUP); //Botão vermelho como entrada
  
  attachInterrupt(0, azul, FALLING); //Chama azul() quando botão azul é presionado
  attachInterrupt(1, vermelho, FALLING); //Chama vermelho() quando botão vermelho é presionado
  
  Timer1.initialize(10000); // Inicializa o Timer1 e configura para um período de 10 milisegundos
  Timer1.attachInterrupt(callback); // Chama callback() a cada interrupção do Timer1

  // Configuração inicial
  estVermelho=0;
  estAzul=0;
  som =0;
  pontosAzul =0;
  pontosVermelho =0;

  //Começa a multiplexação
  digitalWrite(displayAzul,LOW); //Exibe display Azul
  digitalWrite(displayVermelho,HIGH); //Exibe display vermelho
}

//Função chamada quando o botão vermelho é pressionado
void vermelho() {
  noInterrupts(); //Desativa as interrupções
  estVermelho = 1;
  som = 1;
}

//Função chamada quando o botão azul é pressionado
void azul() {
  noInterrupts(); //Desativa as interrupções
  estAzul = 1;
  som = 1;
}

//Função chamada a cada interrupção do Timer
void callback()
{
  digitalWrite(displayAzul, !digitalRead(displayAzul)); //Troca o estado do display azul
  digitalWrite(displayVermelho, !digitalRead(displayVermelho)); //Troca o estado do display vermelho
  if(digitalRead(displayAzul)){ //caso formos ligar o display vermelho
    ligaSegmentosDisplay(pontosVermelho); //Exibimos a pontuação da equipe vermelha
  }else{ //caso formos ligar o display azul
    ligaSegmentosDisplay(pontosAzul); //Exibimos a pontuação da equipe azul
  }
}

//Função que exibe a pontuação no display
void ligaSegmentosDisplay(byte digit){
  byte pino = 12; //Pino do seguimento A

  for (byte contadorSegmentos = 0; contadorSegmentos < 7; ++contadorSegmentos){ //PARA "contadorSegmentos"
    //IGUAL A 0, ENQUANTO "contadorSegmentos" MENOR QUE 7, INCREMENTA "contadorSegmentos"
    digitalWrite(pino, seven_seg_digits[digit][contadorSegmentos]); //PERCORRE O ARRAY E LIGA OS
    //SEGMENTOS CORRESPONDENTES AO DIGITO
    --pino; //DECREMENTA "pino"
  }
}

void loop() {
   
  if(som){ //Caso algum participante aperte o botão
    Serial.println("Alarme ativado"); //Exibe mensagem na serial

    //Garantir que apenas uma equipe terá o direito de resposta
    if(estAzul && estVermelho){ //Pro caso de as duas interrupções executarem
      int num = random(0,100); //Gera num aleatorio entre 0 e 99
      if(num%2==0){ //Caso num seja par
        estVermelho = 0; //Equipe azul ganha o direito de resposta
        Serial.println("Led Azul será aceso");
      }else{ //Caso num seja impar
        estAzul = 0; //Equipe vermelha ganha o direito de resposta
        Serial.println("Led Vermelho será aceso");
      }
    }

    //Acende o led da equipe que vai responder e ativa o som
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    digitalWrite(buzzer,HIGH);

    //Espera o tempo de resposta   
    for (int i=tempo/1000;i>=0;i--){
      
      delay(1000);
    }

    //Para o sinal sonoro
    digitalWrite(buzzer,LOW);
    Serial.println("Tempo para resposta esgotado!!");

    //Pontuação
    int a = !digitalRead(incrementAzul); //ler botão incrementAzul
    int v = !digitalRead(incrementVermelho); //ler botão incrementVermelho
    
    Serial.println("Esperando pelo botão que diz qual equipe pontuou!");
    
    while(!a && !v){ //Enquanto nenhum dos botoes de incrementar pontução for apertado
      a = !digitalRead(incrementAzul); //ler botão incrementAzul
      v = !digitalRead(incrementVermelho); //ler botão incrementVermelho
    }
    //Aqui algun dos botoes foi apertado
    if(a){ //Caso foi o botao azul
      Serial.println("Equipe azul pontuou!");
      pontosAzul++; //incrementa pontuação azul
    }else{ //Caso foi o botao vermelho
      Serial.println("Equipe Vermelha pontuou!");
      pontosVermelho++; //incrementa pontuação vermelha
    }

    //reseta estado das saidas para nova pergunta
    estVermelho = 0;
    estAzul = 0;
    som = 0;
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    
    interrupts(); //Torna a esperar por interrupções
  }
}
