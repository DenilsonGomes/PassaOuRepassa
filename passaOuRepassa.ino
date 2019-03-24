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
int incrementAzul = 5; //Variavel para led azul
int buzzer = 6; //Variavel para sinal sonoro

const int DIO = 7; //Define o pino 8 para o sinal digital
const int CLK = 8; //Define o pino 9 para o clock

int incrementVermelho = 9; //Botao para incrementar pontuação dos vermelhos
int ledVermelho = 10;; //Variavel para led vermelho

int pontosAzul = 0; //Variavel para pontuação da equipe Azul
int pontosVermelho = 0; //Variavel para pontuação da equipe Vermelha
int tempo = 5000; //Tempo que o participante tem para responder a pergunta após apertar o botão

volatile int som; //Variavel para controlar o sinal sonoro
volatile int estVermelho; //Variavel para controlar o sinal visual da equipe Vermelha
volatile int estAzul; //Variavel para controlar o sinal visual da equipe Azul

// -- Instancia de objetos --
TM1637Display display(CLK, DIO);  //Inicializa objeto displays com CLK e DIO

void setup() {

  Serial.begin(9600);
  
  display.setBrightness(0x0a);  //Determina o brilho dos leds do minino (0x00) ate o maximo (0xff)
  pinMode(ledAzul, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(incrementAzul, INPUT);
  pinMode(incrementVermelho, INPUT);
  
  // Sintaxe das interrupções:
  // attachInterrupt(numero_interrupt,funcao_a_executar,modo);
  // Modos LOW,CHANGE,RISING,FALLING
  
  attachInterrupt(0, azul, RISING);
  attachInterrupt(1, vermelho, RISING);
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
//    noInterrupts(); //Desativa as interrupções
    Serial.println("Alarme ativado"); //Aciona o sinal sonoro

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
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    tone(buzzer,166);

    for (int i=tempo/1000;i>=0;i--){
      display.showNumberDecEx(i); //Exibe o tempo
      delay(1000);
    }
    
    noTone(buzzer);
    Serial.println("Tempo para resposta esgotado!!");

    int a = digitalRead(incrementAzul);
    int v = digitalRead(incrementVermelho);
    Serial.println("Esperando pelo botão que diz qual equipe pontuou!");
    
    while(!a && !v){
      a = digitalRead(incrementAzul);
      v = digitalRead(incrementVermelho);
    }

    if(a){
      Serial.println("Equipe azul pontuou!");
      pontosAzul++;
    }else{
      Serial.println("Equipe Vermelha pontuou!");
      pontosVermelho++;
    }

    estVermelho = 0;
    estAzul = 0;
    som = 0;
    digitalWrite(ledVermelho,estVermelho);
    digitalWrite(ledAzul,estAzul);
    
    interrupts();
  }
  
}
