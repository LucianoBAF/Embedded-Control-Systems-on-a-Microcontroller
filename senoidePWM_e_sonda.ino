#include <TimerOne.h>
#include <Ultrasonic.h>

//*********************
//   PINOS DEFINIDOS
#define pino_controle 3
#define pino_trigger  4
#define pino_echo     5
#define pino_reles    7
//*********************

//////////////////////////////////////////////////////////////////////////////
/////////// constantes do teste      /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// número de testes a serem realizados
#define N_TESTES 10  
//frequências logaritmicamente espaçadas para o teste. Em Hz.
float freq_tab[N_TESTES] ={
  0.2000f,
  0.2860f,
  0.4090f,
  0.5848f,
  0.8363f,
  1.1958f,
  1.7100f,
  2.4452f,
  3.4966f,
  5.0000f 
};
float const AMP_MAX        = 1.0f; //amplitude máxima da senóide (entre zero e um)
float const tempo_testes   = 10.f;   //tempo, em segundos, que deve durar cada teste
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/////////// estados globais          /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
unsigned int osc_completas = 0; // oscilações completas no teste atual
float        T1;                // período entre interrupção 1
unsigned int rep_tab[N_TESTES]; //número de repetições para cada frequência
Ultrasonic ultrasonic(pino_trigger, pino_echo, 1800); //argumentos: trigger, echo, timeout.
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
///////////  variáveis de passo da senóide.       ////////////////////////////
///////////  Estas variáveis são alteradas durante interrupção e devem ser  //
///////////  lidas em bloco com interrupções desabilitadas.                 //
//////////////////////////////////////////////////////////////////////////////
volatile unsigned int nt1 = 0; // número de vezes que interrupção 1 rodou durante o teste atual
volatile byte passo_int = 0;   //passo da senóide [0..255] (associado a fase)
volatile byte teste_atual;     //flag para sinalizar teste atual. Incrementada quando teste acaba.
//////////////////////////////////////////////////////////////////////////////


void interrupcao_timer1()
{  
  //pontos em que precisa inverter direção da corrente
  if(passo_int == 0)
    digitalWrite(pino_reles, LOW);
  else if(passo_int == 128)
    digitalWrite(pino_reles, HIGH);
  else if(passo_int == 255)
      osc_completas++; //completou uma senóide

  //verifica se teste não acabou
  if(osc_completas < rep_tab[teste_atual])
  {
    //incremento para próxima iteração
      ++passo_int; 
    //incrementa contagem para tempo global
      ++nt1;
  }
  else
  {
    //quando teste acabou: 
    Timer1.stop(); //pára timer
    teste_atual++; //sinaliza que o teste atual acabou
  }
}

//inicia teste N
void inicia_teste(byte N)
{
  //evita leitura fora da tabela
  if(N >= N_TESTES)
    return;

  float frequencia = freq_tab[N]; // frequencia do teste
  T1 = (1.f/(frequencia*256.f));  //256 iterações por período
  unsigned long T1micross = 1000000.f*T1; // Período em microssegundos. 
  
  Serial.println("%-----------------");
  Serial.print("%teste ");
  Serial.println(N);
  Serial.print("%f = ");
  Serial.print(frequencia);
  Serial.print("Hz, AMP = ");
  Serial.println(AMP_MAX);
  Serial.println("%tempo , C(t), y(t)");
  Serial.print("%T1 = ");
  Serial.print(T1micross);
  Serial.println("us");
  Serial.println("%-----------------");
  Serial.print("TESTE");
  Serial.print(N);
  Serial.println(" = [");
  
  passo_int = 0; //reinicia passos da interrupção
  nt1 = 0;       //reinicia contagem de número de interrupções
  osc_completas = 0; //reinicia contagem de interrupções completas
  Timer1.attachInterrupt(interrupcao_timer1); // interrupção de passo
  Timer1.setPeriod(T1micross); // reinicia timer com novo período
}

//setup() : essa função é chamada automaticamente uma vez, antes de loop ser chamada pela primeira vez
void setup() 
{
  teste_atual = 0;
  pinMode(pino_echo,     INPUT);
  pinMode(pino_controle, OUTPUT);
  pinMode(pino_trigger,  OUTPUT);
  pinMode(pino_reles,    OUTPUT);
  
  digitalWrite(pino_reles,LOW);
  Serial.begin(115200); //serial

  //número aproximado de ciclos para cada frequência que preenche tempo do teste
  for(byte k =0; k<N_TESTES; k++)
    rep_tab[k] = (freq_tab[k]*tempo_testes+0.5f);
  delay(500);
  
  Timer1.initialize(); // Inicializa o timer (necessário antes de usá-lo)
  Timer1.stop();
}

//função que fecha código do MATLAB, muito marota.
void fecha_cod()
{
  Serial.println("];");
  Serial.println("");
  Serial.println("%separa dados");

  for(int k =0; k<N_TESTES; k++)
  {
    Serial.print("figure(");
    Serial.print(k+1);
    Serial.println(");");
    Serial.print("[t, c, y] = fsepara(TESTE");
    Serial.print(k);
    Serial.println(");");
    Serial.println("plot(t, c, t, y);");
    Serial.print("title('Teste f = ");
    Serial.print(freq_tab[k]);
    Serial.print("Hz, AMPMAX = ");
    Serial.print(AMP_MAX);
    Serial.println("');");
    Serial.println("xlabel('t [s]')");
    Serial.println("legend('Controle','Bobina')");
    Serial.print("print('t");
    Serial.print(k);
    Serial.println("' ,'-dpng')");
  }
};

//loop() : durante o funcionamento do Arduino, esta função é repetida infinitamente 
void loop() 
{
  ////////////////////////////////////////
  //Seção crítica de aquisição de dados //
  ////////////////////////////////////////
    noInterrupts();
    byte passo = passo_int;
    byte teste = teste_atual;
    unsigned int nint1 = nt1;
    interrupts();
  ////////////////////////////////////////
  
  ////////////////////////////////////////
  //Parte que controla ordem dos testes //
  ////////////////////////////////////////
  //se completou todos testes, faz nada
  static unsigned int teste_prox = 0;
  static byte fecha_var = 0;
  if(teste_prox == teste)
  {
    if(teste_prox >= N_TESTES)
    {
      //fecha array de resultados de último teste
      if(fecha_var == 0)
      {
        fecha_cod();
        fecha_var++;
      }
      analogWrite(pino_controle, 0);
      digitalWrite(pino_reles, 0);
      delay(250);
      return;
    }
    //fecha declaração de array de resultados
    if(teste_prox != 0)
      Serial.println("];");
    inicia_teste(teste_prox++);
    return;
  }
  ////////////////////////////////////////

  ////////////////////////////////////////
  //Parte que calcula saída de controle //
  ////////////////////////////////////////
  //calcula magnitude do sinal de controle (senóide retificada)
    float ang = passo<128 ? passo : passo - 128; //retifica
    float dir = passo<128 ? 1.f : -1.f;
    ang *= (3.1415f/128.f); //mapeia de 0..128 para 0..pi
    float mag = (AMP_MAX*sin(ang)*255.f); //calcula amplitude e mapaeia para 0..255
    analogWrite(pino_controle, mag); //escreve amplitude de controle
  ////////////////////////////////////////
      
  ////////////////////////////////////////
  //Parte que faz medições              //
  ////////////////////////////////////////
  //mede tempo de eco (sonda)
  float sonda_cm = float(ultrasonic.Timing())/58.f;
  //estima tempo global
  float t = nint1*T1;
  //exporta para serial
  Serial.print('[');
  Serial.print(t);
  Serial.print(' ');
  Serial.print(mag*dir/255.f);
  Serial.print(' ');
  Serial.print(sonda_cm);
  Serial.println(']');
  ////////////////////////////////////////
  
  delay(1);
}

