 /*
  * Código para Passa ou Repassa com arduino
  * 
  * Autores:
  * Denilson Gomes Vaz da Silva   <denilsongomes@alu.ufc.br>
  * Graduando em Engenharia da Computação - UFC
  * Cassio Viera Simplicio
  * Graduando em Mecatrônica - IFCE
 */

// -- Bibliotecas --
#include <TM1637Display.h> // Inclui a biblioteca do Display

// -- Variaveis e Constantes --
int botAzul = 2; //Botao da equipe Azul
int botVermelho = 3; //Botao da equipe Vermelho
int ledAzul = 4; //Variavel para led azul
int incrementAzul = 5; //Variavel para incrementar pontuação azul
int buzzer = 6; //Variavel para sinal sonoro

const int DIO = 7; //Define o pino 8 para o sinal digital
const int CLK = 8; //Define o pino 9 para o clock

int incrementVermelho = 9; //Botao para incrementar pontuação dos vermelhos
int ledVermelho = 10; //Variavel para led vermelho

int pontosAzul = 0; //Variavel para pontuação da equipe Azul
int pontosVermelho = 0; //Variavel para pontuação da equipe Vermelha
int tempo = 5000; //Tempo que o participante tem para responder a pergunta após apertar o botão

volatile int som; //Variavel para controlar o sinal sonoro
volatile int estVermelho; //Variavel para controlar o sinal visual da equipe Vermelha
volatile int estAzul; //Variavel para controlar o sinal visual da equipe Azul

// -- Instancia de objetos --
TM1637Display display(CLK, DIO);  //Inicializa objeto displays com CLK e DIO

void setup() {

  Serial.begin(9600); //Habilita comunicação Serial
  display.setBrightness(0x0a);  //Determina o brilho dos leds do minino (0x00) ate o maximo (0xff)

  //Saídas
  pinMode(ledAzul, OUTPUT); //Led azul como saida
  pinMode(buzzer, OUTPUT); //Buzzer como saida
  pinMode(ledVermelho, OUTPUT); //Led vermalho como saida

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

void loop() {
  uint8_t segto = 0x80; // Atribui os dois pontos a variavel
  display.showNumberDecEx(pontosAzul*1000 + pontosVermelho); //Exibe a pontuação
 
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
      display.showNumberDecEx(i); //Exibe o tempo
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
