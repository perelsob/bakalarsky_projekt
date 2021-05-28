
    
    #include <ros.h>
    #include <geometry_msgs/Twist.h>
    #include <komunikacia/senzor_data.h> //kniznica pre specifikaciu message dat zo senzorov
    #include <komunikacia/rychlost_pasov.h>
    #include <std_msgs/String.h>
    #include <MeAuriga.h>
    #include <MeTemperature.h>
    #define BUZZER_PORT  45
    #define r_kolesa 2  //polomer cm
    #define Speed_RPM 150 //rychlost pre motory, hodnota PWM, vlavo vpravo *0.7
    
    MeRGBLed led( 0, 12);
    MeEncoderOnBoard Encoder_1(SLOT1);
    MeEncoderOnBoard Encoder_2(SLOT2);
    MeOnBoardTemp temp(PORT_13);
    MeLightSensor lightSensor(PORT_12);
    MeSoundSensor dbsense(14);
    MeUltrasonicSensor ultrasonic(PORT_10);
    MeBuzzer buzzer;

    
  
    komunikacia::senzor_data senzor_DATA;
    komunikacia::rychlost_pasov rychlost_PASOV;
    std_msgs::String stav;

       
    void zisti_rychlost();

void Forward(void)
{
  Encoder_1.setMotorPwm(-Speed_RPM); //pravy pas
  Encoder_2.setMotorPwm(Speed_RPM); //lavy
  stav.data="vpred";
}
void Backward(void)
{
  Encoder_1.setMotorPwm(Speed_RPM);
  Encoder_2.setMotorPwm(-Speed_RPM);
  stav.data="vzad";
}

void TurnLeft(void)
{//pravy vpred
  Encoder_1.setMotorPwm(-Speed_RPM*0.7);
  Encoder_2.setMotorPwm(-Speed_RPM*0.7);
  stav.data="vlavo";
}
void TurnRight(void)
{//lavy vpred
  Encoder_1.setMotorPwm(Speed_RPM*0.7);
  Encoder_2.setMotorPwm(Speed_RPM*0.7);
  stav.data = "vpravo";
}
  
void Stop(void)
{
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
  stav.data="stoji";
}

void isr_process_encoder1(void) // count the ticks - i.e. how far the motor has moved
{
  if(digitalRead(Encoder_1.getPortB()) == 0)
  {
  Encoder_1.pulsePosMinus();
  }
  else
  {
  Encoder_1.pulsePosPlus();
  }
}
void isr_process_encoder2(void) // count the ticks - i.e. how far the motor has moved
{
  if(digitalRead(Encoder_2.getPortB()) == 0)
  {
  Encoder_2.pulsePosMinus();
  }
  else
  {
  Encoder_2.pulsePosPlus();
  }
}

void messageCb( const geometry_msgs::Twist& pohyb)
  {
    
  ledky( 0, 0, 80); // red, green,blue
  pasy(pohyb);   // vykona pohyb
  ledky( 0, 0, 0);
    
  }

//Set up the ros node publisher and subscriber

ros::Publisher pub_temp("data_senzorov", &senzor_DATA); 
ros::Publisher pub_pasy("pasy_rychlost", &rychlost_PASOV);
ros::Publisher pub_pohyb("pohyb_stav", &stav);
ros::Subscriber<geometry_msgs::Twist> sub("turtle1/cmd_vel", &messageCb );
      
ros::NodeHandle nh;
   
   
   void setup()
   {
     led.setpin( 44 );
       //Set PWM 8KHz
    // Serial.begin(115200);
    Serial.begin(57600);
     TCCR1A = _BV(WGM10);
     TCCR1B = _BV(CS11) | _BV(WGM12);
     TCCR2A = _BV(WGM21) | _BV(WGM20);
     TCCR2B = _BV(CS21);

     attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING); 
     attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);

      
     nh.initNode();
     nh.subscribe(sub);
     nh.advertise(pub_temp);
     nh.advertise(pub_pasy);
     nh.advertise(pub_pohyb);
     
     
     buzzer.setpin(BUZZER_PORT);
     buzzer.noTone();
     buzzer.tone(523, 200);   //bzuciak 523Hz, 200ms
     delay(100);
     //buzzer.noTone();             
     buzzer.tone(523, 200);
     
   }
   

       unsigned long cas_pred=0;
       const long interval= 1000;

   void loop()
   { 
    unsigned long cas_teraz = millis(); //millis zistuje aktualny cas, casovac pre posielanie dat zo senzorov
  if (cas_teraz - cas_pred >= interval) 
    {
        cas_pred = cas_teraz;
        senzor_DATA.teplota= temp.readValue(); // °C
        senzor_DATA.svetlo= lightSensor.read(); //
        senzor_DATA.zvuk=dbsense.strength();
        senzor_DATA.vzdialenost=ultrasonic.distanceCm(); //cm

        
        if (senzor_DATA.vzdialenost<40)
        buzzer.tone(1244, 50);
    
        pub_temp.publish(&senzor_DATA);

        zisti_rychlost();
        Stop() ;
        pub_pohyb.publish(&stav);
    } 

     nh.spinOnce(); 
    
   }

   void ledky(uint8_t red, uint8_t green, uint8_t blue)
   {
    for (uint8_t t = 0;t < 13; t++)  
   
      {
        led.setColorAt( t, red, green, blue );
      }
    led.show();   
   }

   void pasy(const geometry_msgs::Twist pohyb)
   { 
    if (pohyb.linear.x > 0)
        {
           Forward();
        }
    else if (pohyb.linear.x < 0)
        {  
          Backward();
        } 
    else if (pohyb.angular.z > 0) 
        {  
          TurnLeft(); 
        }
    else if (pohyb.angular.z < 0)  
        {
          TurnRight();
        }
    else 
        {
          Stop() ;
        }

    pub_pohyb.publish(&stav);
    
    zisti_rychlost();
    delay(100);
    zisti_rychlost();
    delay(100);
    zisti_rychlost(); 
    delay(100); 
    zisti_rychlost(); 
    delay(100);
    zisti_rychlost(); 
    delay(100); 
    zisti_rychlost(); 
    Stop() ;
    pub_pohyb.publish(&stav);
   } 
    

  void zisti_rychlost()
    {
      Encoder_1.loop();
      Encoder_2.loop();
           
      rychlost_PASOV.pas_p= (Encoder_1.getCurrentSpeed() / 60) * (-2) * 3,14 *r_kolesa; //cm/s, -2 pravy motor je zapojeny opacne
      rychlost_PASOV.pas_l= (Encoder_2.getCurrentSpeed() / 60) * 2 * 3,14 *r_kolesa; //  cm/s
      
      pub_pasy.publish(&rychlost_PASOV);
    
    }
    
   
