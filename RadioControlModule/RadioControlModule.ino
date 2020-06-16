/***********************************************************************************/  
/*************************** RADIO CONTROL MODULE  *********************************/                               
/***********************************************************************************/
//==================================================================================
//  Creation Date:      12/04/2017
//  Revision History 
//  Name:               Date:         Description
//  Mateus S. Franco    13.04.17      Leds and calib() implementation
//  Mateus S. Franco    23.04.17      Bug fixes and calib() function modification                           
//===================================================================================


/* Global Variables and Defines Section */

/*  Turnigy 9X
 *  ch1 (Thrust)  - min = 105 max = 191
 *  ch3 (Aileron) - min = 105 max = 191 
 */
//=========================
//=== CHANNELS AND AXIS ===
//=========================
#define ch1 8 // x - aileron
int ch1_min =  103;
int ch1_max =  190;

#define ch2 7 //  y - elevator
int ch2_min =  95;
int ch2_max = 190;

#define ch5 2
int ch5_thresh = 120;
int calib_flag=0;

int calib_sw = 120;
double x,   y;
double x1;
double s1,  s2;
double A=0.1;
#define LED_CALIB 5
#define LED_THRUST_M 6

//==================
//=== MOTOR PINS ===
//==================
double m1, m2;
#define m1_D 3
#define m1_R 11
#define m2_D 10
#define m2_R 9

//================================================
//  AUTHOR:       Mateus S. Franco
//  CREATE DATE:    13/04/17
//  PURPOSE:      Lock motors  immediately
//  SPECIAL NOTES:    -
//================================================
//  Change History:
//
//================================================
void EMERGENCY_STOP() {
  digitalWrite(m1_D,LOW);
  digitalWrite(m2_D,LOW);
  digitalWrite(m1_R,LOW);
  digitalWrite(m2_R,LOW);
  digitalWrite(LED_THRUST_M,LOW);
  digitalWrite(LED_CALIB,HIGH);
  delay(250);
  digitalWrite(LED_CALIB,LOW);
  delay(250);
}

//=========================================================
//  AUTHOR:       Mateus S. Franco
//  CREATE DATE:    13/04/17
//  PURPOSE:      Renew min and max values for axis
//  SPECIAL NOTES:    -
//=========================================================
//  Change History:
//  23.04.17 calib() acts as long as ch5 is active 
//
//=========================================================
void calib() {
  EMERGENCY_STOP();
  digitalWrite(LED_CALIB,HIGH);
  digitalWrite(LED_THRUST_M,LOW);
  ch1_min = 1000; ch2_min = 1000;
  ch1_max = 0;    ch2_max = 0; 
  while(calib_sw > ch5_thresh) {
    x = pulseIn(ch1,HIGH,100000)/10;
    y = pulseIn(ch2,HIGH,100000)/10;
    calib_sw = pulseIn(ch5,HIGH,100000)/10;
    //Serial.print(x); Serial.print(", "); Serial.println(y);
    if(ch1_min > x) ch1_min = x;  if(ch1_max < x) ch1_max = x;
    if(ch2_min > y) ch2_min = y;  if(ch2_max < y) ch2_max = y;
  }
  calib_flag=1;
  digitalWrite(LED_CALIB,LOW);
}

//=======================================================================
//  AUTHOR:         Mateus S. Franco
//  CREATE DATE:    12/04/17
//  PURPOSE:        Get data from receiver and call PWM controllers
//  SPECIAL NOTES:    -
//=======================================================================
//  Change History:   
//  14.04.17 LED_CALIB and LED_CALIB control functions and calib() calls
//  23.04.17 Map changed from [-255,255] to [-250,250] according to hardware limitations         
//
//=======================================================================
void readSignal() {
  x = pulseIn(ch1,HIGH,100000)/10;
  y = pulseIn(ch2,HIGH,100000)/10;
  //Serial.print("(x,y) = ");  Serial.print(x);  Serial.print(", "); Serial.println(y);
  calib_sw = pulseIn(ch5,HIGH,100000)/10;
  if(x !=0 && y != 0 ) {
    if(calib_sw > ch5_thresh && calib_flag == 0) {
      Serial.println("calib()");
      calib();
    } else {
        x = map(x,ch1_min,ch1_max,-250,250);
        y = map(y,ch2_min,ch2_max,-250,250);
        if(abs(x) >= abs(y))
        {
          analogWrite(LED_THRUST_M,abs(x));
        }
        else{
          analogWrite(LED_THRUST_M,abs(y));
        }
        //Serial.print("(x,y) = ");  Serial.print(x);  Serial.print(", "); Serial.println(y);
       }  
     } else {
         Serial.println("Fail Code");
         EMERGENCY_STOP();
       }   
    
}

//=======================================================================
//  AUTHOR:         Mateus S. Franco
//  CREATE DATE:    12/04/17
//  PURPOSE:        Motor 1 (left) PWM controller
//  SPECIAL NOTES:    -
//=======================================================================
//  Change History:   
//  04.05.17 Map changed from [-255,255] to [-250,250] according to hardware limitations
//  25.05.17 Implementation of single exponential average filter
//
//=======================================================================
void m1_pwm() {
  if(x >= -250 && x < -30) {
    m1 = y + ((y/250) * x);
  } else {
    m1 = y;
  }
  //m1 = (A*s1) + ((1-A) * m1);
  //Serial.print("(s1,m1) = "); Serial.println(s1, m1);
}

//=======================================================================
//  AUTHOR:         Mateus S. Franco
//  CREATE DATE:    21/05/17
//  PURPOSE:        Motor 2 (right) PWM controller
//  SPECIAL NOTES:    -
//=======================================================================
//  Change History:   
//  04.05.17 Map changed from [-255,255] to [-250,250] according to hardware limitations
//  25.05.17 Implementation of single exponential average filter
//
//=======================================================================
void m2_pwm() {
  if(x > 30 && x <= 250) {
    m2 = y - ((y/250) * x);
  }else {
    m2 = y;
  }
  //m2 = (A*s2) + ((1-A) * m2);
  //Serial.print("(s2,m2) = "); Serial.println(s2, m2);
}

void x1_filter() {
  x1=(A*x) + ((1-A) * x1);
}

//=======================================================================
//  AUTHOR:         Mateus S. Franco
//  CREATE DATE:    12/04/17
//  PURPOSE:        setup function
//  SPECIAL NOTES:    -
//=======================================================================
//  Change History:   
//
//=======================================================================
void setup() {
  Serial.begin(9600);
  pinMode(ch1,INPUT);
  pinMode(ch2,INPUT);
  pinMode(LED_CALIB, OUTPUT);
  pinMode(LED_THRUST_M,OUTPUT);
  x = 0;  y = 0;
}

//=======================================================================
//  AUTHOR:         Mateus S. Franco
//  CREATE DATE:    12/04/17
//  PURPOSE:        Main loop
//  SPECIAL NOTES:    -
//=======================================================================
//  Change History:   
//  04.05.17 Joystick deadzone changed from [-10,10] to [-15,15] according to code adaptations
//  27.05.17 LED_THRUST indicator implementation
//=======================================================================
void loop() {
  readSignal();
  m1_pwm();
  m2_pwm();
  //x1_filter();
  if(y > 30) {
    Serial.print("Frente: "); Serial.print(m1);  Serial.print(", "); Serial.println(m2);
    analogWrite(m1_D,m1); analogWrite(m2_D,m2); digitalWrite(m1_R,LOW); digitalWrite(m2_R,LOW);
    
  } else {
    if(y < -30) {
      Serial.print("Re   : "); Serial.print(m1);  Serial.print(", "); Serial.println(m2);
      analogWrite(m1_R,-m1); analogWrite(m2_R,-m2); digitalWrite(m1_D,LOW); digitalWrite(m2_D,LOW);
    } else {
      if(x > 30) {
        Serial.print("Giro horario: "); Serial.print(x);  Serial.print(", "); Serial.println(-x1);
        analogWrite(m1_D,x); analogWrite(m2_R,x); digitalWrite(m1_R,LOW); digitalWrite(m2_D,LOW);
      } else {
        if(x < -30) {
          Serial.print("Giro anti horario: "); Serial.print(x);  Serial.print(", "); Serial.println(-x1);
          analogWrite(m1_R,-x); analogWrite(m2_D,-x); digitalWrite(m1_D,LOW); digitalWrite(m2_R,LOW);
        } else {
          Serial.print("Parado"); //Serial.println("0,0"); 
          //m1=0; m2=0; x1=0;
          digitalWrite(m1_D,LOW); digitalWrite(m1_R,LOW); digitalWrite(m2_D,LOW); digitalWrite(m2_R,LOW);
        }
      }
    }
  }
}  
