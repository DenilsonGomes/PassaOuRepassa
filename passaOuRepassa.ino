 /*
 * Autores:
 * Denilson Gomes Vaz da Silva
 * Graduando em Engenharia da Computação - UFC
 * Cassio Viera Simplicio
 * Graduando em Mecatrônica - IFCE
 */

// -- Bibliotecas --
#include <TM1637Display.h> // Inclui a biblioteca do Display

// -- Variaveis e Constantes --
int botAzul = 2; //Botao Azul
int botVermelho = 3; //Botao Vermelho
int ledAzul = 4; //Variavel para azul
int buzzer = 5; //Variavel para som
int ledVermelho = 6; //Variavel para led vermelho
int incrementAzul = 7; //Botao para incrementar pontuação dos azuis
int reseta = 8; //Botao para resetar a pontuação
int incrementVermelho = 9; //Botao para incrementar pontuação dos vermelhos

volatile int som;
volatile int estVermelho;
volatile int estAzul;

const int DIO = 10; //Define o pino 8 para o sinal digital
const int CLK = 11; //Define o pino 9 para o clock

// -- Instancia de objetos --
TM1637Display display(CLK, DIO);  //Inicializa objeto displays com CLK e DIO

void setup() {

  Serial.begin(9600);
  
  display.setBrightness(0x0a);  //Determina o brilho dos leds do minino (0x00) ate o maximo (0xff)
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  
  // Sintaxe das interrupções:
  // attachInterrupt(numero_interrupt,funcao_a_executar,modo);
  // Modos LOW,CHANGE,RISING,FALLING
  
  attachInterrupt(0, azul, RISING);
  attachInterrupt(1, vermelho, RISING);
}

void vermelho() {
  estVermelho = 1;
  som = 1;
  noInterrupts(); //Desativa as interrupções
}

void azul() {
  estAzul = 1;
  som = 1;
  noInterrupts(); //Desativa as interrupções
}

void loop() {
  uint8_t segto = 0x80; // Atribui os dois pontos a variavel
  display.showNumberDecEx(1001); //Exibe o tempo com os dois pontos

  if(som){ //Caso algum participante aperte o botão
    
    Serial.println("Alarme ativado");
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    tone(5,166);
    delay(5000);
    estVermelho = 0;
    estAzul = 0;
    som = 0;
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    noTone(5);
    interrupts();
  }
  
}
