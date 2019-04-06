 /*
  * Código para Passa ou Repassa com arduino
  * 
  * Autores:
  * Denilson Gomes Vaz da Silva   <denilsongomes@alu.ufc.br>
  * Graduando em Engenharia da Computação - UFC
  * Cassio Viera Simplicio
  * Graduando em Mecatrônica - IFCE
 */

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
  pinMode(ledVermelho, OUTPUT); //Led vermalho como saida
  pinMode(displayAzul, OUTPUT); //Sinal para ativar displayAzul
  pinMode(displayVermelho, OUTPUT); //Sinal para ativar displayVermelho
  
  
  //Seguimentos dos Displays
  pinMode(12, OUTPUT); //PINO 2 -> SEGMENTO A  
  pinMode(11, OUTPUT); //PINO 3 -> SEGMENTO B
  pinMode(10, OUTPUT); //PINO 4 -> SEGMENTO C
  pinMode(9, OUTPUT); //PINO 5 -> SEGMENTO D
  pinMode(8, OUTPUT); //PINO 6 -> SEGMENTO E
  pinMode(7, OUTPUT); //PINO 7 -> SEGMENTO F
  pinMode(6, OUTPUT); //PINO 8 -> SEGMENTO G
  
  //Entradas
  pinMode(incrementAzul, INPUT_PULLUP); //Botão incrementa pontos azul como entrada
  pinMode(incrementVermelho, INPUT_PULLUP); //Botão incrementa pontos vermelho como entrada
  pinMode(botAzul, INPUT_PULLUP); //Botão azul como entrada
  pinMode(botVermelho, INPUT_PULLUP); //Botão vermelho como entrada
  
  // Sintaxe das interrupções:
  // attachInterrupt(numero_interrupt,funcao_a_executar,modo);
  // Modos LOW,CHANGE,RISING,FALLING
  
  attachInterrupt(0, azul, FALLING); //Interupção quando botão azul é presionado
  attachInterrupt(1, vermelho, FALLING); //Interupção quando botão vermelho é presionado
  
  Timer1.initialize(30000); // Inicializa o Timer1 e configura para um período de 10 milisegundos
  Timer1.attachInterrupt(callback); // Chama callback() a cada interrupção do Timer1
  
  estVermelho=0;
  estAzul=0;
  som =0;
}

void vermelho() {
  noInterrupts(); //Desativa as interrupções
  estVermelho = 1;
  som = 1;
}

void azul() {
  noInterrupts(); //Desativa as interrupções
  estAzul = 1;
  som = 1;
}

void callback()
{
  digitalWrite(displayAzul, !digitalRead(displayAzul));
  digitalWrite(displayVermelho, !digitalRead(displayVermelho));            
  if(digitalRead(displayAzul)){
    ligaSegmentosDisplay(pontosAzul);
  }else{
    ligaSegmentosDisplay(pontosVermelho);
  }
}

void ligaSegmentosDisplay(byte digit){ //FUNÇÃO QUE ACIONA O DISPLAY
  byte pino = 2;

  for (byte contadorSegmentos = 0; contadorSegmentos < 7; ++contadorSegmentos){ //PARA "contadorSegmentos"
    //IGUAL A 0, ENQUANTO "contadorSegmentos" MENOR QUE 7, INCREMENTA "contadorSegmentos"
    digitalWrite(pino, seven_seg_digits[digit][contadorSegmentos]); //PERCORRE O ARRAY E LIGA OS
    //SEGMENTOS CORRESPONDENTES AO DIGITO
    ++pino; //INCREMENTA "pino"
  }
}

void loop() {
   
  if(som){ //Caso algum participante aperte o botão
    Serial.println("Alarme ativado"); //Exibe mensagem na serial

    //Garantir que apenas uma equipe terá o direito de resposta
    if(estAzul && estVermelho){ //Pro caso de as duas interrupções executarem
      int num = random(0,100);
      if(num%2==0){
        estVermelho = 0;
        Serial.println("Led Azul será aceso");
      }else{
        estAzul = 0;
        Serial.println("Led Vermelho será aceso");
      }
    }

    //Acende o led da equipe que vai responder e ativa o som
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    tone(buzzer,166);

    //Espera o tempo de resposta   
    for (int i=tempo/1000;i>=0;i--){
      
      delay(1000);
    }

    //Para o sinal sonoro
    noTone(buzzer);
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
