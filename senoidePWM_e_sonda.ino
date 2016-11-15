#include <TimerOne.h>
#include <Ultrasonic.h>
#include <Console.h>

#define pino_pwm 3
#define pino_trigger 4
#define pino_echo 5
#define pino_led 13


//*****************
#define pino_reles 6  //muda o estado dos reles da fonte de corrente quando passa por zero
//*****************
static float AMP_MAX = 1.0;     //alterar esse valor para modular a amplitude da senoide produzida. E.g, 1=5V 0.5=2,5V


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
  frequencia=10;//em kHz

  //*****************
  digitalWrite(pino_reles,LOW);
  //*****************
  
  Timer1.initialize((1/frequencia)*1000000); // Inicializa o Timer1 e configura período em microssegundos
  Timer1.attachInterrupt(interrupcao_timer1);
}

void interrupcao_timer1()
{
  if(senoide<255 && incdec==1)        senoide++;
  else if(senoide>=255 && incdec==1) {incdec=0; senoide--;}
  else if(senoide>0 && incdec==0)     senoide--;
  else if(senoide<=0 && incdec==0)   {incdec=1; senoide++;}

  //*****************
  if(senoide==128) 
  {
    if (incdec==1)digitalWrite(pino_reles,HIGH);
    else          digitalWrite(pino_reles,LOW);
  }
  //*****************
}

void loop() 
{
  analogWrite(pino_pwm,sin(map(senoide,0,255,0,180))*255*AMP_MAX);
  
  digitalWrite(pino_led,HIGH);//liga o led onboard
  
  //Lê a distância usando o sensor ultrassônico
  microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);

  //analogWrite(pino_pwm,map(cmMsec,0,40,0,255));

  Serial.print(sin(map(senoide,0,255,0,180))*AMP_MAX);Serial.println(','); Serial.println(cmMsec);
  //Console.print(sin(map(senoide,0,255,0,180))*AMP_MAX); Console.println(cmMsec);

   
  //--------NÃO LEMBRO SE ESSE DELAY ERA PARA EVITAR OVERLAPPING DA LEITURA---------
  //----------------------CONSIDERAR DESCOMENTAR CASO TENHA RUÍDO-------------------
  //delay(100);



  digitalWrite(pino_led,LOW);//desliga o led onboard

}
