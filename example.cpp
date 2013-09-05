#include <Arduino.h>

#include <PFTransmitter.h>

namespace
{
  // Digital
  const int pinIrLed                    = 8;
  const int pinButtonNorth              = 4;
  const int pinButtonSouth              = 5;
  const int pinButtonEast               = 3;
  const int pinButtonWest               = 6;
  const int pinButtonJoystick           = 2;
  const int pinInverseJoystickX         = 12;
  const int pinInverseJoystickY         = 11;
  const int pinInverseButtonsEastWest   = 10;
  const int pinInverseButtonsNorthSouth = 9;

  // Analog
  const int pinJoystickX = 0;
  const int pinJoystickY = 1;

  const int xLeft  = 0;
  const int xRight = 1023;
  const int yDown  = 1023;
  const int yUp    = 0;

  int xMid = 0;
  int yMid = 0;

  PF_n::transmitter_c transmitter( pinIrLed );

  /*
    Get joystick values and send message
  */
  void readJoystickComboPWM( PF_n::transmitter_c::channel_t channel, int pinX, int pinY, int xLeft, int xMid, int xRight,
                             int yUp, int yMid, int yDown )
  {
    const int valX = analogRead( pinX );
    const int valY = analogRead( pinY );
    PF_n::transmitter_c::pwmOutput_t outputA = PF_n::transmitter_c::PWM_OUTPUT_BRAKE_FLOAT;
    PF_n::transmitter_c::pwmOutput_t outputB = PF_n::transmitter_c::PWM_OUTPUT_BRAKE_FLOAT;

    if ( valX < xMid )
    {
      // Links
      outputA = PF_n::transmitter_c::pwmOutput_t( map( valX, xMid, xLeft,
                                                       PF_n::transmitter_c::PWM_OUTPUT_FLOAT, PF_n::transmitter_c::PWM_OUTPUT_FORWARD_7 ) );
    }
    else
    {
      // Rechts
      outputA = PF_n::transmitter_c::pwmOutput_t( map( valX, xMid, xRight,
                                                       PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_1 + 1,
                                                       PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_7 ) );
    }
    if ( outputA <= PF_n::transmitter_c::PWM_OUTPUT_FLOAT || outputA > PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_1 )
    {
      outputA = PF_n::transmitter_c::PWM_OUTPUT_BRAKE_FLOAT;
    }

    if ( valY < yMid )
    {
      // Vorwärts
      outputB = PF_n::transmitter_c::pwmOutput_t( map( valY, yMid, yUp,
                                                       PF_n::transmitter_c::PWM_OUTPUT_FLOAT, PF_n::transmitter_c::PWM_OUTPUT_FORWARD_7 ) );
    }
    else
    {
      // Rückwärts
      outputB = PF_n::transmitter_c::pwmOutput_t( map( valY, yMid, yDown,
                                                       PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_1 + 1,
                                                       PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_7 ) );
    }
    if ( outputB <=  PF_n::transmitter_c::PWM_OUTPUT_FLOAT || outputB > PF_n::transmitter_c::PWM_OUTPUT_BACKWARD_1 )
    {
      outputB = PF_n::transmitter_c::PWM_OUTPUT_BRAKE_FLOAT;
    }

    const bool inverseJoystickX = digitalRead( pinInverseJoystickX ) == HIGH;
    const bool inverseJoystickY = digitalRead( pinInverseJoystickY ) == HIGH;

    transmitter.setMessageComboPWM( channel, outputA, inverseJoystickX, outputB, inverseJoystickY );
  }

  /*
    Get button values and send message
  */
  void readButtons( PF_n::transmitter_c::channel_t channel, int pinNorth, int pinSouth, int pinEast, int pinWest )
  {
    const int buttonStateNorth = digitalRead( pinNorth );
    const int buttonStateSouth = digitalRead( pinSouth );
    const int buttonStateEast  = digitalRead( pinEast );
    const int buttonStateWest  = digitalRead( pinWest );
    PF_n::transmitter_c::comboDirectOutput_t outputA = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FLOAT;
    PF_n::transmitter_c::comboDirectOutput_t outputB = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FLOAT;

    if ( buttonStateNorth == HIGH && buttonStateSouth == LOW )
    {
      outputA = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FORWARD;
    }
    else if ( buttonStateNorth == LOW && buttonStateSouth == HIGH )
    {
      outputA = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_BACKWARD;
    }

    if ( buttonStateEast == HIGH && buttonStateWest == LOW )
    {
      outputB = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FORWARD;
    }
    else if ( buttonStateEast == LOW && buttonStateWest == HIGH )
    {
      outputB = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_BACKWARD;
    }

    const bool inverseButtonsNorthSouth = digitalRead( pinInverseButtonsNorthSouth ) == HIGH;
    const bool inverseButtonsEastWest   = digitalRead( pinInverseButtonsEastWest )   == HIGH;

    transmitter.setMessageComboDirect( channel, outputA, inverseButtonsNorthSouth, outputB, inverseButtonsEastWest );
  }

  /*
    Get value of joystick-button and send message
  */
  void readJoystickButton( PF_n::transmitter_c::channel_t channel, int pin )
  {
    const int buttonState = digitalRead( pin );
    PF_n::transmitter_c::comboDirectOutput_t outputA = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FLOAT;
    PF_n::transmitter_c::comboDirectOutput_t outputB = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FLOAT;

    if ( buttonState == LOW )
    {
      outputA = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FORWARD;
      outputB = PF_n::transmitter_c::COMBO_DIRECT_OUTPUT_FORWARD;
    }

    transmitter.setMessageComboDirect( channel, outputA, false, outputB, false );
  }
}

void setup()
{
  pinMode( pinButtonJoystick, INPUT );
  digitalWrite( pinButtonJoystick, HIGH );
  
  pinMode( pinButtonEast, INPUT );
  digitalWrite( pinButtonEast, HIGH );

  pinMode( pinButtonNorth, INPUT );
  digitalWrite( pinButtonNorth, HIGH );
  
  pinMode( pinButtonSouth, INPUT );
  digitalWrite( pinButtonSouth, HIGH );

  pinMode( pinButtonWest, INPUT );
  digitalWrite( pinButtonWest, HIGH );

  pinMode( pinInverseJoystickX, INPUT );
  digitalWrite( pinInverseJoystickX, HIGH );

  pinMode( pinInverseJoystickY, INPUT );
  digitalWrite( pinInverseJoystickY, HIGH );

  pinMode( pinInverseButtonsNorthSouth, INPUT );
  digitalWrite( pinInverseButtonsNorthSouth, HIGH );

  pinMode( pinInverseButtonsEastWest, INPUT );
  digitalWrite( pinInverseButtonsEastWest, HIGH );

  pinMode( pinIrLed, OUTPUT );

  xMid = analogRead( pinJoystickX );
  yMid = analogRead( pinJoystickY );
  
  Serial.begin( 9600 );
}

void loop()
{
/*
  Serial.print( analogRead( pinJoystickX ) );       //Read the position of the joysticks X axis and print it on the serial port.
  Serial.print( "," );
  Serial.print( analogRead( pinJoystickY ) );       //Read the position of the joysticks Y axis and print it on the serial port.
  Serial.print( "," );
  Serial.print( digitalRead( pinButtonJoystick ) ); //Read the value of the select button and print it on the serial port.
  Serial.print( digitalRead( pinButtonNorth ) );    //Read the value of the button 1 and print it on the serial port.
  Serial.print( digitalRead( pinButtonSouth ) ) ;   //Read the value of the button 2 and print it on the serial port.
  Serial.print( digitalRead( pinButtonEast ) );     //Read the value of the button 0 and print it on the serial port.
  Serial.println( digitalRead( pinButtonWest ) );   //Read the value of the button 3 and print it on the serial port.
  
  //Wait for 100 ms, then go back to the beginning of 'loop' and repeat.
  delay( 100 );
*/

  readJoystickComboPWM( PF_n::transmitter_c::CHANNEL_1, pinJoystickX, pinJoystickY, xLeft, xMid, xRight, yUp, yMid, yDown );
  readButtons( PF_n::transmitter_c::CHANNEL_2, pinButtonNorth, pinButtonSouth, pinButtonEast, pinButtonWest );
  readJoystickButton( PF_n::transmitter_c::CHANNEL_3, pinButtonJoystick );

  transmitter.sendMessages();
}
