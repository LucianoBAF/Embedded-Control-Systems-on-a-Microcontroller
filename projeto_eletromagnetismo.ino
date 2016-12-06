#include <TimerOne.h>
#include <Ultrasonic.h>
#include <Console.h>

//--------------------------------------------------------------------
//                           Defines
//--------------------------------------------------------------------
#define pino_pwm 3
#define pino_trigger A1
#define pino_echo 2
//#define pino_led 13
#define pino_reles A3  //muda o estado dos reles da fonte de corrente quando passa por zero
#define pino_joystick A5

//--------------------------------------------------------------------


//--------------------------------------------------------------------
//                    Sensor ultrassônico
//--------------------------------------------------------------------
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);
float cmMsec; //Distância lida pelo sensor ultrassônico
long microsec;//Variável utilizada para leitura do sensor ultrassônico
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//                    Declaração de variáveis
//--------------------------------------------------------------------
volatile char tempo_ok =0; // variavel que é setada quando termina o tempo de amostragem
float joystick_read = 0,joystick_read_old = 0,duty_cycle=0;
float coil_pos = 0, control=0;

double u[2] = {0,0}, e[2] = {0,0}; //vetor de controle e de erro
                                  //posição 1 = atual
                                  //posição 0 = anterior

//--------------------------------------------------------------------




//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------



void interrupcao_timer1()
{
  tempo_ok =1;
}



void setup() 
{
  pinMode(pino_echo,INPUT);
  pinMode(pino_pwm,OUTPUT);
  pinMode(pino_trigger,OUTPUT);
  //pinMode(pino_led,OUTPUT);
  pinMode(pino_reles,OUTPUT);
  pinMode(pino_joystick,INPUT);

  cmMsec=5;
  Serial.begin(9600); //Inicia serial


  //dentro do initialize vai 1000000./frequencia = periodo
  //frequencia tem que ser duas vezes mais rápida que o polo mais rápido de Tr(s)
  //Polo mais rapido em  -49.0996 +- 1.9540i
  Timer1.initialize((1000000/50.0)/2.0); // Inicializa o Timer1 e configura período em microssegundos
  Timer1.attachInterrupt(interrupcao_timer1);


  //inicialização do vetor de control e de erro
  duty_cycle=128;
  analogWrite(pino_pwm,128);
  u[1]=128;
  e[1]=0;
}



void loop() 
{
   joystick_read_old = joystick_read;
   //joystick_read = joystick_average_position();
   joystick_read = map(analogRead(pino_joystick),0,1023,0,14.5);

   
        //if(js_press_pin==1) //funcao SW PRESSED
        //  joystick_read = joystick_press;
    
        //probe_request();
        //position = ((probeCountEchoByte)*14.5)/255;
   //Lê a distância usando o sensor ultrassônico
   microsec = ultrasonic.timing();
   cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
   coil_pos=cmMsec;

    
   /* 
   //executa a ação específica baseado no joystick SE HOUVER ALTERAÇÃO DO ESTADO DO MESMO
   if((joystick_read!=joystick_read_old))//&&(joystick_read!=joystick_free))
   {
      switch(joystick_read)
      {
        case joystick_left:   if(control>1) control--;        break;  //DECREMENTA A POSIÇÃO EM 1CM
        case joystick_right:  if(control<14)  control++;        break;  //INCREMENTA A POSIÇÃO EM 1CM
        case joystick_press:  control = CONTROL_POSITION_MIDPOINT;  break;  //CENTRALIZA BOBINA
        default:                            break;  //NAO DEVE OCORRER
      }
    }
  */

//******************************************************************************************

  /*
  _clk_past = T_clk;
  T_clk = get_time();
  if(sgn_error0 < CONTROL_POSITION_ERROR)
     control_signal0 = 0;
  else
  {
    //T_clk = período em segundos entre samples, para uso na definição da função de controle, medido com o timer
    //define o controlador, e recalcula as constantes dependentes do período de sample caso o mesmo mude
    if((T_clk_past =! T_clk)) 
    {
      T_40 = 40*T_clk;
      T_50 = 50*T_clk;
      K_1 = (2 - T_50)/(T_50 + 2);
      K_2 = 0.06*(T_40 - 2)/(T_50 + 2);
      K_3 = 0.06*(T_40 + 2)/(T_50 + 2);
    }
    control_signal1 = control_signal0;
    control_signal0 = K_1*control_signal1 + K_2*sgn_error1 + K_3*sgn_error0;
    //duty_cycle = control_signal; //possivelmente precisará de uma variável multiplicando
    if(control_signal0 > CONTROL_MAXIMUM_OUTPUT)
    control_signal0 = CONTROL_MAXIMUM_OUTPUT; //se certificando de que o limite é respeitado
  }*/


  //Equação de recorrência 
  //u[n]=u[n-1]*0.04322+e[n]*0,04997-0,004048*e[n-1]
  
  
  u[0]= duty_cycle/255.0;
  e[0]= u[0]-(coil_pos/14.5);
  u[0]= u[0]*0.04322+e[1]*0.04997-0.004048*e[0];


  //por os reles de acordo com a direção de movimento desejada
  if(u[0]<u[1]) analogWrite(pino_reles,LOW); 
  if(u[0]>u[1]) analogWrite(pino_reles,HIGH);
  
  u[1]=u[0];
  e[1]=e[0];
  
  

  analogWrite(pino_pwm,u[0]*255);

  //exporta para serial
  Serial.print(duty_cycle);
  Serial.print(','); 
  Serial.println(coil_pos);


  while(tempo_ok!=1) {};//espera fechar o tempo de amostragem. Variavel vai pra 1 na interrupção
  tempo_ok=0; 
  
  Timer1.initialize((1000000/50.0)/2.0);
}

