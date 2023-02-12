#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2); //Funzione per indicare i collegamenti
                                  //tra display LCD e Arduino
                                  //lcd(RS, E, D3, D2, D1, D0)
                                  
const float Tmax=50;              //Temperatura massima raggiungibile
const int ConvertionBits=10;      //Numero di bit del convertitore di Arduino

int DigitalValue=0;
float Temperature=0;

/**
 * Gli array sottostanti contengono le informazioni di ogni variabile usata
 * nell'ordine:
 * secondi, minuti, ore, giorno, mese, anno
 */
int X[6]={14,11,8,8,11,14};       //Numero della colonna sul display di ogni variabile
int Y[6]={1,1,1,0,0,0};           //Numero della riga sul display di ogni variabile
int Minimum[6]={0,0,0,1,1,0};     //Valore minimo di ogni variabile
int Maximum[6]={59,59,23,0,12,99};//Valore massimo di ogni variabile
int Array[6]={0,0,0,1,1,0};       //Valore iniziale di ogni variabile

int initial_seconds=0;
int previousSecond=0;

/**
 * Pin e valori dei pulsanti usati per gestire il display
 */
int confirmationPin=6;
int Pin_Up=7;
int Pin_Down=8;
int confirmationButton=1;
int Button_Up=1;
int Button_Down=1;

void setup() {
  pinMode(confirmationPin,INPUT);
  pinMode(Pin_Down,INPUT);
  pinMode(Pin_Up,INPUT);

  //Inizializzazione display
  lcd.begin(16,2);
  lcd.clear();
  delay(1000);
  
  initial_SettingTime();
  initial_seconds=millis()/1000;
}

/**
 * Funzione che viene chiamata ogni volta che bisogna 
 * stampare la data e l'ora
 */
void Print_Time_Date (unsigned int settingMode){
  for(unsigned int i=0; i<6; i++){

    //settingMode=1 se si è nella parte iniziale del programma 
    //in cui si imppostano data e ora iniziali
    if(settingMode==1){
      if(i==3){
        lcd.setCursor(0,0);
        lcd.print("Date:");
      }
      if(i==2){
        lcd.setCursor(0,1);
        lcd.print("Time:");
      }
    }
    
    lcd.setCursor(X[i],Y[i]);

    //se la variabile ha un valore <10 verrà stampata con
    //uno 0 davanti
    if(Array[i]<10){
      lcd.print("0");
      lcd.setCursor((X[i]+1),Y[i]);
    }
    
    lcd.print(Array[i]);
    if(i!=0)
      lcd.setCursor((X[i]+2),Y[i]);
    
    if(i==3 || i==4)
      lcd.print("/");
    if(i==2 || i==1)
      lcd.print(":");
  }
  return;
}

/**
 * Funzione per impostare il giorno massimo in base al mese e all'anno in cui si è 
 * gestisce anche gli anni bisestili
 */
void Set_MaximumDay(int Month, int Year){
  int Max;
  if(Month==11 || Month==9 ||Month==4 || Month==6)
    Max=30;
  if(Month==1 || Month==3 || Month==5 || Month==7 || Month==8 || Month==10 || Month==12)
    Max=31;

  int leapYear=Year%4;
  if(Month==2 && leapYear==0)
    Max=29;
  if(Month==2 && leapYear!=0)
    Max=28;

  Maximum[3]=Max;
}

/**
 * Funzione per impostare l'ora e la data iniziali
 */
void initial_SettingTime(){
  lcd.blink();
  
  for(unsigned int i=5; i>0; i--){
      while(confirmationButton!=0){
        //Legge se i pulsanti sono stati premuti
        confirmationButton=digitalRead(confirmationPin);
        Button_Up=digitalRead(Pin_Up);
        Button_Down=digitalRead(Pin_Down);

        Print_Time_Date(1);
        lcd.setCursor(X[i]+2,Y[i]);

        //se i pulsanti sono stati premuti agisce di conseguenza
        if(Button_Up==0)
          Array[i]++;
        if(Button_Down==0)
          Array[i]--;

        if(i==3)
          Set_MaximumDay(Array[i+1], Array[i+2]);

        //se la variabile raggiunge il suo massimo+1
        //viene portata al suo valore minimo
        if(Array[i]==Maximum[i]+1)
          Array[i]=Minimum[i];
          
        //se la variabile raggiunge il suo minimo-1
        //viene portata al suo valore massimo
        if(Array[i]==Minimum[i]-1)
          Array[i]=Maximum[i];

         delay(200);
      }
      confirmationButton=1;
  }
  lcd.noBlink();
  lcd.clear();
  return;
}

/**
 * Funzione per aggiornare, una volta impostata, la data e l'ora
 */
void Update_Date_Time(){
  for(unsigned int i=0; i<5; i++){
    if(i==3)
      Set_MaximumDay(Array[i+1], Array[i+2]);

    //quando la variabile raggiunge il suo massimo viene messa al 
    //suo minimo e la variabile seguente viene aumentata di 1
    //Es. se minuti=61 --> minuti=0 e ore+1
    if(Array[i]==Maximum[i]+1){
      Array[i]=Minimum[i];
      Array[i+1]++;
      if(i==0)
        initial_seconds=millis()/1000;
    }
  }
  return;
}

void loop() {
  //lettura del valore analogico di tensione della temperatura
  DigitalValue=analogRead(A0);
  //conversione del valore di tensione in un valore di temperatura in Celsius
  //Temperatura_rilevata : temperatura_massima = valore_digitale_rilevato : 1024
  Temperature=(DigitalValue*Tmax)/(pow(2,ConvertionBits));
  
  lcd.setCursor(0,0);
  lcd.print(Temperature);
  lcd.print("C");

  Print_Time_Date(0);

  //Aggiornamento dei secondi mediante la funzione millis()
  previousSecond=Array[0];
  while(previousSecond==Array[0])
    Array[0]=(millis()/1000)-initial_seconds;

  Update_Date_Time();
}
