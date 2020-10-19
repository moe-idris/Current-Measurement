// Program to measure consumed current by a load with the SCT013-30A current sensor

const int SensorPin = A0 , RefPin = A2; //definition of the arduino sensor and reference pin constants
const int Rshunt = 33 ; //shunt resistance value

// Definition of auxiliary variables 
unsigned long time_ant = 0, difTime = 0, act_time = 0;
const int sampleDuration = 20;
int count_integral = 0;
double rawSquaredSum = 0;
double Iant = 0;

double freq = 50 ; //definition of the grid frequency (50 Hz in Europe)

double n_trafo = 1000; //definition of the transformer turn ratio determined from the SCT013 current sensor datasheet

// Definition of measured current variable 
double Irms = 0;

/****For the sake of data transmission from the arduino to the computer, 
  the baud rate is defined using the following command. In this case the baud rate is set to 115200 bits/s****/
  Serial.begin(115200); 


}

// Loop code
void loop()
{
  act_time = micros();
  difTime = act_time - time_ant;

  int RawValue = 0;

  if (difTime >= 1000) {
    time_ant = act_time + (difTime - 1000);

    /***** The ADC (Analog Digital convertor) input from the sensor and the voltage reference point *****/
    /* We got the value in bits and the relation is 5V - 1023 bits*/
    
     double ADC_sensor = analogRead(A0); //definition of the sensor pin in the arduino that is to be read and converted into digital information
     double ADC_ref = analogRead(A2); //definition of the voltage reference pin in the arduino that is to be read and converted into digital information

    
    /***** The measured ADC inputs of the sensor and reference point are translated into voltage values using the following formula *******/
    double V_sens = ADC_sensor*5.0/1023;
    double V_ref = ADC_ref*5.0/1023;

    // Instantaneous current is then calculated by using the voltage difference, shunt resistance value, and transformer turn ratio value as follows//
    double Iinst =  n_trafo * (V_sens - V_ref) / Rshunt;
      
    // The instantaneous current (calculated above) is then printed using the following command//
    Serial.println(Iinst);
    
    // The integral in the RMS current equation is calculated as follows//
    rawSquaredSum += Iinst * Iinst * 0.001;
    
    // Count 20 ms
    count_integral++;
  }

  // Each 20 ms, calculte the RMS
  if (count_integral >= sampleDuration) 
  {
    /***The RMS current is then calculated due to the fact that the instantaneous current is not useful in determining the mean current consumption of the kettle.
    This is mainly because the input current itself is sinusoidal (alternating current) ***/
 
    Irms = sqrt(freq * rawSquaredSum);
    
    // Counter and integral reset//
    count_integral = 0;
    rawSquaredSum = 0;

    // Low-pass filter//
    double Ifilt = 0.8 * Iant + 0.2 * Irms;
    Iant = Ifilt;
    
    // RMS is then printed using the following command again //
   Serial.println(Ifilt);
  }
}
