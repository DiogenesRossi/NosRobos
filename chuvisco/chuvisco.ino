// Codigo para o Robo Chuvisco Pestana
// By drossi

#include <Servo.h> 

// Movimentacao
Servo pSR;         // Servo Direito  - Motorista
Servo pSL;         // Servo Esquerdo - Passageiro
Servo neck;        // Servo do sensor de distancia

int pSRS = 93;     // Servo Direiro  Ponto de parada  
int pSLS = 66;     // Servo Esquerdo Ponto de parada

const int PinPestanaDireita = 2;     // the number of the pushbutton pin
const int PinPestanaEsquerda = 5;     // the number of the pushbutton pin

int neckPos=100;

#define echoPin 13  
#define trigPin 12  

void inicializaServos(){
  pSR.attach(10);  // pin_10, servo direito
  pSL.attach(9);   // pin_09, servo esquerdo
  neck.attach(8);  // pin_08, servo neck
  anda(0,0,0);     // para os motores
}

void anda(int velocidade, int flagRe, int valueDirection){
  int adjR=0;
  int adjL=0;

  if ( valueDirection > 0)
    adjR=valueDirection;
  else
    adjL=valueDirection;

  if (flagRe==1)
    velocidade=-velocidade;
    
  pSR.write(pSRS-velocidade+adjR);
  Serial.print(" M.Dir: ");
  Serial.print(pSRS-velocidade+adjR);
  pSL.write(pSLS+velocidade+adjL);
  Serial.print(" M.Esq: ");
  Serial.println(pSLS+velocidade+adjL);
}



// Sensores 

// Estado do Sensor -> 0-Branco, 1-Preto, 2-Indeterminado, 3-Inoperante
// Line Read      last     min                  max      estado
int lineRead[16]={0,0,0,0, 1023,1023,1023,1023, 0,0,0,0, 0,0,0,0};
void lnGetData(){
  lineRead[0] = analogRead(A0);  
  lineRead[1] = analogRead(A1);  
  lineRead[2] = analogRead(A2);  
  lineRead[3] = analogRead(A3);

  for (int p=0; p<4; p++){
    // Guarda Extremos por sensor
    if (lineRead[p+0] < lineRead[p+4])
      lineRead[p+4]=lineRead[p+0];
    if (lineRead[p+0] > lineRead[p+8])
      lineRead[p+8]=lineRead[p+0];

    int range=lineRead[p+8]-lineRead[p+4];  // maximo-minimo
    int value=lineRead[p+0]-lineRead[p+4];  // lido-minimo
    if ( range < 100 )
      lineRead[p+12]=3;  // Inoperante 
    else if (( value < range*.45)) 
      lineRead[p+12]=1;  // Preto
    else if (( value > range*.55) ) 
      lineRead[p+12]=0;  // Branco
    else
      lineRead[p+12]=2;  // Indeterminado
  }
}

void getLineRead(){
  lnGetData();
  showLineRead();
}  

void showLineRead(){
  for (int s=0; s<4; s++){
//    Serial.print(lineRead[s]);
//    Serial.print(" :[ ");
//    Serial.print(lineRead[s+4]);
//    Serial.print("-");
//    Serial.print(lineRead[s+8]);
    Serial.print("  [");
    Serial.print(lineRead[s+12]);
    Serial.print("] - ");
  }
  Serial.print(getDirection());
  Serial.print(""); 

//  for (int s=0; s<4; s++){
//    Serial.print(lineRead[s]);
//    Serial.print(" :[ ");
//    Serial.print(lineRead[s+4]);
//    Serial.print("-");
//    Serial.print(lineRead[s+8]);
//    Serial.print(" ]  (");
//    Serial.print(lineRead[s+12]);
//    Serial.print(") ");
//  }
//  Serial.println(""); 
}  

void colisao(int l, int r){
 int velocidade=1;
 if (l==0){
  pSR.write(pSRS+velocidade+4);
  pSL.write(pSLS-velocidade);
  delay(1000);
 } 
 if (r==0){
  pSL.write(pSLS-(velocidade+4));
  pSR.write(pSRS+velocidade);
  delay(1000);
 }  
}

void leiaPestanas(){
    int direita = digitalRead(PinPestanaDireita);
    int esquerda = digitalRead(PinPestanaEsquerda);
    colisao(esquerda,direita);
}

void readDistance(){
    int initialPos = 100;
    int distance = getPing();
    neck.write(initialPos);
    if(distance < 600){
      anda(0,0,0);
      Serial.println(distance);
      delay(400);
      changeDirection(distance);
    }
}


void changeDirection(int initial){
  int left, right;
  neck.write(50);
  delay(300);
  left = getPing();
  neck.write(150);
  delay(300);
  right = getPing();
  neck.write(100);
  delay(400);
  if(left > right) {
    gira(1);
  }else{
    gira(0);
  }  
  delay(200);
  
}

int getPing(){
  digitalWrite(trigPin, LOW);     
  delayMicroseconds(2);         // delay de 2 microssegundos
  digitalWrite(trigPin, HIGH);    
  delayMicroseconds(10);        //delay de 10 microssegundos  
  digitalWrite(trigPin, LOW);   //seta o pino 12 com pulso baixo novamente  
  return pulseIn(echoPin,HIGH);  //pulseInt lê o tempo entre a chamada e o pino entrar em high
}


// Perifericos
// Som
int speakerPin = 13;
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}
void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  // toque o tom correspondente ao nome da nota
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration*100);
    }
  }
}

// 
int getDirection(){
  int d=0;
  if (lineRead[12]==1)
    d+=3;
  if (lineRead[13]==1)
    d+=1;
  if (lineRead[14]==1)
    d-=1;
  if (lineRead[15]==1)
    d-=3;
  return d;  
}

void gira(int direita)
{
  int vel=3;
  if (direita){
    pSR.write(pSRS+vel);
    pSL.write(pSLS+vel);
  }
  else
  {
    pSR.write(pSRS-vel);
    pSL.write(pSLS-vel);
  }
  delay(700);
}

// Programa principal
void setup() 
{ 
  Serial.begin(9600);
  pinMode(speakerPin, OUTPUT);
  pinMode(PinPestanaEsquerda, INPUT);     
  pinMode(PinPestanaDireita, INPUT);     
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  inicializaServos();
} 

void loop() 
{
  getLineRead();
  anda(4,0,getDirection());
  leiaPestanas();  
  readDistance();  
  delay(40);
}


