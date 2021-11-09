#include <Wire.h>
#include <Servo.h>
#include "Adafruit_TCS34725.h"
 
/* Example code for the Adafruit TCS34725 breakout library */
 
/* Connect SCL to analog 5
Connect SDA to analog 4
Connect VDD to 3.3V DC
Connect GROUND to common ground */
 
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725();

//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);




// Cikis hazne pozisyonları arrayi
//const int Cikis[8]={30, 50, 70, 90, 110, 130, 150, 178};
const int Cikis[8]={25, 50, 80, 110, 140, 170, 180};

// Varsayilan Pozisyonlar
int Hazne=170, Incele=116, Dok=58, HazneSifir=148, KizakSifir=0, tHazne=-1, tIncele=-1, tDok=-1, Kirmizi=0, Yesil=0, Mavi=0, Berraklik=0, Hassasiyet=5, tHassasiyet=-1,
ServoDelay=30, kalKirmizi=0, kalMavi=0, kalYesil=0, bosSayac=0;

int renk=-1, RFlag=1;
const byte LED=4;

//Bulunan renklering  rgb degerleri arrayde sakla 8 farkli rengi saklayabil.
int bulunanRenkler[8][3]={};

//int arr[2][3] = {{10,20,30},{5,2,4}};

Servo Kizak, Tasiyici;
int TasiyiciPos=-1, KizakPos=-1;

void TasiyiciCevir(int Hedef)
{
  if (Hedef >180 || Hedef <0)
  return;

  while (Tasiyici.read()!=Hedef)
  {
    if (Tasiyici.read()<Hedef && Tasiyici.read()<=180 && Tasiyici.read()>=0)
    {
      Tasiyici.write(Tasiyici.read()+1);
//      Serial.println(Tasiyici.read());
      delay(ServoDelay);
    }
    else if (Tasiyici.read()>Hedef && Tasiyici.read()<=180 && Tasiyici.read()>=0)
    {
      Tasiyici.write(Tasiyici.read()-1);
//      Serial.println(Tasiyici.read());
      delay(ServoDelay);
    }
     
  }
  
}

void KizakCevir(int Hedef)
{
  if (Hedef >180 || Hedef <0)
  return;

  while (Kizak.read()!=Hedef)
  {
    if (Kizak.read()<Hedef && Kizak.read()<=180 && Kizak.read()>=0)
    {
      Kizak.write(Kizak.read()+1);
//      Serial.println(Kizak.read());
      delay(ServoDelay);
    }
    else if (Kizak.read()>Hedef && Kizak.read()<=180 && Kizak.read()>=0)
    {
      Kizak.write(Kizak.read()-1);
//      Serial.println(Kizak.read());
      delay(ServoDelay);
    }     
  }  
}

void BeyazKalibrasyon(){
  TasiyiciCevir(Dok);
  TasiyiciCevir(Incele);
  RenkOku();
  kalKirmizi=Kirmizi;
  kalMavi = Mavi;
  kalYesil= Yesil;
}


void BulunanRenkResetle(){
  for (int i=0;i<8;i++)
  {
    bulunanRenkler[i][0]=0;
    bulunanRenkler[i][1]=0;
    bulunanRenkler[i][2]=0;
  }
  KizakCevir(KizakSifir);
}

void RenkOku(){
  digitalWrite(LED,HIGH);
  delay(1000);
  uint16_t kirmizi, yesil, mavi, berrak;
  tcs.getRawData(&kirmizi, &yesil, &mavi, &berrak);  
  digitalWrite(LED,LOW);

  float k,y,m;

  uint32_t toplam = berrak;

  k = kirmizi;
  k /= toplam;

  y = yesil;
  y /= toplam;

  m = mavi;
  m /= toplam;

  k *=256;
  y *=256;
  m *=256;

  Kirmizi = k;
  Yesil = y;
  Mavi = m;
  Berraklik = berrak;
}


int RengeKararVer(){

  RenkOku();

  if (kalKirmizi>=(Kirmizi-Hassasiyet) && kalKirmizi<=(Kirmizi+Hassasiyet) &&
  kalMavi>=(Mavi-Hassasiyet) && kalMavi<=(Mavi+Hassasiyet) &&
  kalYesil>=(Yesil-Hassasiyet) && kalYesil<=(Yesil+Hassasiyet))
  {
    return 8;
  }
  
//  digitalWrite(LED,LOW);
  Serial.println((String)"Kirmizi :"+Kirmizi);
  Serial.println((String)"Yesil :"+Yesil);
  Serial.println((String)"Mavi :"+Mavi); 
  Serial.println((String)"Clear :"+Berraklik); 
  delay(1000);

  int arrSize = sizeof(bulunanRenkler)/sizeof(bulunanRenkler[0]);
  
  for (int i=0; i<7;i++)
  {
    if ((int)bulunanRenkler[i][0]!=0 && (int)bulunanRenkler[i][1]!=0 && (int)bulunanRenkler[i][2]!=0) 
    {
      if (bulunanRenkler[i][0]>=(Kirmizi-Hassasiyet) && bulunanRenkler[i][0]<=(Kirmizi+Hassasiyet) &&
        bulunanRenkler[i][1]>=(Yesil-Hassasiyet) && bulunanRenkler[i][1]<=(Yesil+Hassasiyet) &&
        bulunanRenkler[i][2]>=(Mavi-Hassasiyet) && bulunanRenkler[i][2]<=(Mavi+Hassasiyet))
        {
        Serial.println((String)"CIKIS :"+i);
        return i;
        }
    }
    else
    {
      bulunanRenkler[i][0]=Kirmizi;
      bulunanRenkler[i][1]=Yesil;
      bulunanRenkler[i][2]=Mavi;
      Serial.println((String)"CIKIS-2 :"+i);
      return i;
    }
  }
  Serial.println((String)"CIKIS-3 :-1");
  return 8;  
 
}

void setup(void) {

  Serial.begin(9600);
  Kizak.attach(3);
  Tasiyici.attach(2);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  
  if (tcs.begin()) {
  Serial.println("Sensor bulundu");
  } else {
    Serial.println("TCS34725 bulunamadi");
    while (1);
  }  

  Serial.println("******************* Komutlar ****************");
  Serial.println("R0 : Robotu calistirip ayiklama islemine baslar.");
  Serial.println("R1 : Robotu resetler. Onceden ayiklanmis renkleri ve sirasini sifirlar.");
  Serial.println("R2 : Kizak pozisyonlarini dolasir. Ayiklananlarin toplanacagi kaplarin posiyonlarini belirlemekte kullanilir.");
  Serial.println("R3 : Robot pause edilir. Resetten farkli olarak onceden ayiklanmis renkler ve siralari sifirlanmaz. R0 ile kaldigi yerden devam eder.");
  Serial.println("R5 : Serbest hareket modu. T ve K değerlerine göre hareke ettirmek için kullanilir.");
  Serial.println("T(int) : Tasiyiciyi girilen integer degere kadar hareket ettirir. -1 degeri resetler. Gecerli degerler 0-180 arasidir.");
  Serial.println("K(int) : Kizagi girilen integer degere kadar hareket ettirir. -1 degeri resetler. Gecerli degerler 0-180 arasidir.");
  Serial.println("H(int) : Hassasiyet ayari. Okunan renk degeri ile +/- hassasiyet degeri kadarli aralik gecerli renk olarak kabul edilir. Varsayilan deger 5 dir.");  
  Serial.println("Z(int) : Tasiyici icin hazne pozisyonunun degerini degistirmek icin kullanilir. Varsayilani 164 dir.");
  Serial.println("Y(int) : Tasiyici icin inceleme pozisyonunun degerini degistirmek icin kullanilir. Varsayilani 110 dur.");
  Serial.println("X(int) : Tasiyici icin dokme pozisyonunun degerini degistirmek icin kullanilir. Varsayilani 58 dir.");
  
  
  KizakCevir(KizakSifir);
  BeyazKalibrasyon();
  TasiyiciCevir(HazneSifir);

}
 
void loop(void) {

  if (Serial.available()>0)
  {
    if (Serial.read() == 'R')  
      RFlag = Serial.parseInt();
      if (Serial.read() == 'T')  
      TasiyiciPos = Serial.parseInt();
        if (Serial.read() == 'K')  
          KizakPos = Serial.parseInt();
          if (Serial.read() == 'H')  
            tHassasiyet = Serial.parseInt();
              if (Serial.read() == 'Z')  
              tHazne = Serial.parseInt();
                if (Serial.read() == 'Y')  
                tIncele = Serial.parseInt();
                  if (Serial.read() == 'X')  
                   tDok = Serial.parseInt();
               
    //R-1T-1K-1H-1I-1D-1H-1
  }

  if (tHassasiyet!=-1)
    Hassasiyet=tHassasiyet;

  if (tHazne!=-1)
    Hazne=tHazne;

  if (tIncele!=-1)
    Incele=tIncele;

  if (tDok!=-1)
    Dok=tDok;

  if (RFlag==1)
  {    
    TasiyiciCevir(HazneSifir);
    BulunanRenkResetle();
    KizakCevir(KizakSifir);
  }
  else if (RFlag==2)
  {
    for (int i=0;i<8;i++)
    {
      KizakCevir(Cikis[i]);
      delay(1500);
    }
  }
  else if (RFlag==3)
  {
    TasiyiciCevir(HazneSifir);
    KizakCevir(90);
  }
  else if (RFlag==4)
  {
    TasiyiciCevir(Hazne);
    KizakCevir(90);
    TasiyiciCevir(Dok);    
  }
  else if (RFlag==5)
  {
     if (TasiyiciPos!=-1 && TasiyiciPos >=0 && TasiyiciPos<=180)
      Tasiyici.write(TasiyiciPos);
    if (KizakPos!=-1 && KizakPos >=0 && KizakPos<=180)
      Kizak.write(KizakPos);
  }
  else
  {
    TasiyiciCevir(Hazne);
    delay(300);
    TasiyiciCevir(Incele);
    renk = RengeKararVer();
    if (renk!=8) {
      Serial.println(renk);
      KizakCevir(Cikis[renk]);
      delay(300);
      TasiyiciCevir(Dok);
      bosSayac=0;
    }
    else
    {
      bosSayac++;
      
      if (bosSayac>=3)
      {
        Serial.println("3 denemede hazneden seker alamadim. Bitmis olabilir kontrol edin ve 'R0' komutu ile devam edin.");
        RFlag=3;
        bosSayac=0;
      }
    }
  }

}
