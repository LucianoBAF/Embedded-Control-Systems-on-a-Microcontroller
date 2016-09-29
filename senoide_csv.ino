#include <TimerOne.h>
#include <Ultrasonic.h>

#define pino_pwm 3
#define pino_trigger 4
#define pino_echo 5
#define pino_led 13

volatile char incdec; //para controle da senoide: 1=inc 0=dec
volatile char senoide;
char frequencia;

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);
float cmMsec; //Distância lida pelo sensor ultrassônico
long microsec;//Variável utilizada para leitura do sensor ultrassônico



void setup() 
{
  Serial.begin(9600); //A serial será utilizada para se comunicar com o Processing
  
  pinMode(pino_pwm,OUTPUT);
  pinMode(pino_led, OUTPUT); //led para contole do funcionamento

  incdec=1;
  senoide=0;
  frequencia=10;
  
  Timer1.initialize((1/frequencia)*1000000); // Inicializa o Timer1 e configura período em microssegundos
  Timer1.attachInterrupt(interrupcao_timer1);
}

void interrupcao_timer1()
{
  if(senoide<255 && incdec==1)        senoide++;
  else if(senoide>=255 && incdec==1) {incdec=0; senoide--;}
  else if(senoide>0 && incdec==0)     senoide--;
  else if(senoide<=0 && incdec==0)   {incdec=1; senoide++;}
}

void loop() 
{
  analogWrite(pino_pwm,sin(map(senoide,0,255,0,180)));

  digitalWrite(pino_led,HIGH);//liga o led onboard
  
  //Lê a distância usando o sensor ultrassônico
  microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);

  digitalWrite(pino_led,LOW);//desliga o led onboard

  //analogWrite(pino_pwm,map(cmMsec,0,40,0,255));

  Serial.print(sin(map(senoide,0,255,0,180))); Serial.println(cmMsec);

  

}
