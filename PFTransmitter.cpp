#include "PFTransmitter.h"

#include <Arduino.h>

namespace PF_n
{
  /*
    Construktor
  */
  transmitter_c::channel_c::channel_c() :
    _actualMessageLength( 0 ),
    _channel( CHANNEL_NUM ),
    _data( 0 ),
    _mode( MODE_NONE ),
    _pin( -1 ),
    _outputA( PWM_OUTPUT_FLOAT ),
    _outputB( PWM_OUTPUT_FLOAT ),
    _repeats( 0 ),
    _singleOutput( SINGLE_OUTPUT_A ),
    _singleOutputMode( SINGLE_OUTPUT_MODE_PWM ),
    _toggle( false )
  {
  }

  /*
    Initialize values
  */
  void transmitter_c::channel_c::init( int pin, channel_t channel )
  {
    _channel = channel;
    _pin = pin;
  }

  /*
    Get length of a cycle
  */
  unsigned int transmitter_c::channel_c::cycleLength()
  {
    return 26;
  }

  /*
   Get maximum length of a message
  */
  unsigned int transmitter_c::channel_c::maximumMessageLength()
  {
    return 16000;
  }

  /*
    Wait (cycles)
  */
  void transmitter_c::channel_c::pauseCycles( unsigned int cycles ) const
  {
    pauseTime( cycles * cycleLength() );
  }

  /*
    Wait (miliseconds)
  */
  void transmitter_c::channel_c::pauseTime( unsigned int waitTime ) const
  {
    delayMicroseconds( waitTime );
    _actualMessageLength += waitTime;
  }

  /*
    Send message
  */
  void transmitter_c::channel_c::sendMessage()
  {
    _actualMessageLength = 0;

    if ( _mode != MODE_NONE )
    {
      for ( int nibble = NIBBLE_1; nibble < NIBBLE_NUM; nibble++ )
      {
        _nibbles[ nibble ] = 0;
      }

      writeStartStopBit();
      writeToggle();
      writeEscape();
      writeChannel();
      if ( _mode == MODE_COMBO_PWM )
      {
        writePwmOutput();
      }
      else
      {
        writeAddress();
        writeMode();
        writeData();
      }
      writeLRC();
      
      writeNibbles();

      writeStartStopBit();
    }

    pauseTime( maximumMessageLength() - _actualMessageLength );

    endMessage();
  }

  /*
    Reset values after message was send
  */
  void transmitter_c::channel_c::endMessage()
  {
    if ( _mode == MODE_COMBO_PWM ||
         _mode == MODE_COMBO_DIRECT ||
         ( _mode == MODE_SINGLE_OUTPUT && ( _data == SINGLE_OUTPUT_CSTID_FULL_FORWARD || _data == SINGLE_OUTPUT_CSTID_FULL_BACKWARD ) ) )
    {
      // Send message again in next cycle
    }
    else
    {
      _repeats++;
      if ( _repeats >= 5 )
      {
        _mode = MODE_NONE;
        _toggle = !_toggle;
        _repeats = 0;
      }
    }
  }

  /*
    Set a message for Combo-Direct-Mode
  */
  void transmitter_c::channel_c::setMessageComboDirect( comboDirectOutput_t outputA, comboDirectOutput_t outputB )
  {
    _mode = MODE_COMBO_DIRECT;
    _outputA = outputA;
    _outputB = outputB;
  }

  /*
    Set a message for Combo-PWM-Mode
  */
  void transmitter_c::channel_c::setMessageComboPWM( pwmOutput_t outputA, pwmOutput_t outputB )
  {
    _mode = MODE_COMBO_PWM;
    _outputA = outputA;
    _outputB = outputB;
  }

  /*
    Set a message for Extended-Mode
  */
  void transmitter_c::channel_c::setMessageExtended( extendedData_t data )
  {
    _mode = MODE_EXTENDED;
    _data = data;
  }

  /*
    Set a message for Single-Output-CSTID-Mode
  */
  void transmitter_c::channel_c::setMessageSingleOutputCstid( singleOutput_t output, singleOutputCstid_t data )
  {
    _mode = MODE_SINGLE_OUTPUT;
    _singleOutputMode = SINGLE_OUTPUT_MODE_CSTID;
    _singleOutput = output;
    _data = data;
  }

  /*
    Set a message for Single-Output-PWM-Mode
  */
  void transmitter_c::channel_c::setMessageSingleOutputPWM( singleOutput_t output, pwmOutput_t data )
  {
    _mode = MODE_SINGLE_OUTPUT;
    _singleOutputMode = SINGLE_OUTPUT_MODE_PWM;
    _singleOutput = output;
    _data = data;
  }

  /*
    Send address-bit
  */
  void transmitter_c::channel_c::writeAddress() const
  {
    _nibbles[ NIBBLE_2 ] |= 0 << 3; // TODO: implementieren
  }

  /*
    Send channel
  */
  void transmitter_c::channel_c::writeChannel() const
  {
    _nibbles[ NIBBLE_1 ] |= _channel;
  }

  /*
    Send payload
  */
  void transmitter_c::channel_c::writeData() const
  {
    if ( _mode == MODE_COMBO_DIRECT )
    {
      _nibbles[ NIBBLE_3 ] = ( _outputB << 2 ) | _outputA;
    }
    else
    {
      _nibbles[ NIBBLE_3 ] = _data;
    }
  }

  /*
    Calculate checksum
  */
  void transmitter_c::channel_c::writeLRC() const
  {
    _nibbles[ NIBBLE_4 ] = 0xF ^ _nibbles[ NIBBLE_1 ] ^ _nibbles[ NIBBLE_2 ] ^ _nibbles[ NIBBLE_3 ];
  }

  /*
    Send escape-bit
  */
  void transmitter_c::channel_c::writeEscape() const
  {
    if ( _mode == MODE_COMBO_PWM )
    {
      _nibbles[ NIBBLE_1 ] |= 1 << 2;
    }
  }

  /*
   Send a high-bit
  */
  void transmitter_c::channel_c::writeHighBit() const
  {
//    Serial.println( "High" );
    writeMark();
    pauseCycles( 21 );
  }

  /*
    Send a low-bit
  */
  void transmitter_c::channel_c::writeLowBit() const
  {
//    Serial.println( "Low" );
    writeMark();
    pauseCycles( 10 );
  }

  /*
    Send start/stop bit
  */
  void transmitter_c::channel_c::writeStartStopBit() const
  {
//    Serial.println( "Start/Stop" );
    writeMark();
    pauseCycles( 39 ); 
  }

  /*
    Send IR-mark
  */
  void transmitter_c::channel_c::writeMark() const
  {
    static unsigned int halfCycleLength = cycleLength() / 2;

    for ( int xx = 0; xx < 6; xx++ )
    {
      digitalWrite( _pin, HIGH );
      pauseTime( halfCycleLength );
      digitalWrite( _pin, LOW );
      pauseTime( halfCycleLength );
    }
  }

  /*
    Send mode
  */
  void transmitter_c::channel_c::writeMode() const
  {
    int newMode = 0;

    switch ( _mode )
    {
    case MODE_EXTENDED:
      newMode = 0;
      break;

    case MODE_COMBO_DIRECT:
      newMode = 1;
      break;

    case MODE_SINGLE_OUTPUT:
      newMode = ( 1 << 2 ) | ( _singleOutputMode << 1 ) | _singleOutput;
      break;

    default:
      // hier sollte das Programm nie hinkommen
      break;
    }

    _nibbles[ NIBBLE_2 ] |= newMode;
  }

  /*
    Send all nibbles
  */
  void transmitter_c::channel_c::writeNibbles() const
  {
    for ( int nibble = NIBBLE_1; nibble < NIBBLE_NUM; nibble++ )
    {
//      Serial.println( _nibbles[ nibble ] );
      for ( int bit = 3; bit >= 0; bit-- )
      {
        if ( ( _nibbles[ nibble ] & ( 1 << bit ) ) > 0 )
        {
          writeHighBit();
        }
        else
        {
          writeLowBit();
        }
      }
    }
  }

  /*
    Send data for Combo-PWM-Mode
  */
  void transmitter_c::channel_c::writePwmOutput() const
  {
    _nibbles[ NIBBLE_2 ] = _outputB;
    _nibbles[ NIBBLE_3 ] = _outputA;
  }

  /*
    Send toggle-bit
  */
  void transmitter_c::channel_c::writeToggle() const
  {
    if ( _toggle )
    {
      _nibbles[ NIBBLE_1 ] |= 1 << 3;
    }
  }

  /*
    Constructor
  */
  transmitter_c::transmitter_c( int pin ) :
    _pin( pin )
  {
    _channels[ CHANNEL_1 ].init( pin, CHANNEL_1 );
    _channels[ CHANNEL_2 ].init( pin, CHANNEL_2 );
    _channels[ CHANNEL_3 ].init( pin, CHANNEL_3 );
    _channels[ CHANNEL_4 ].init( pin, CHANNEL_4 );
  }

  /*
   Send messages an all four channels
  */
  void transmitter_c::sendMessages()
  {
    const unsigned int tm = channel_c::maximumMessageLength() / 1000;

#if 0
    for ( int xx = 0; xx < 3; xx++ )
    {
      for ( int channel = CHANNEL_4; channel >= CHANNEL_1; channel-- )
      {
        _channels[ channel ].sendMessage();
      }
      delay( tm ); // Die Zeit von einem Start bis zum nächsten ist 5*tm. Vier Messages wurden gesendet, also noch 1*tm warten
    }

    delay( 2 * tm ); // das ist (6+2*Ch)*tm für Ch=1, davon wurden noch 2*tm abgezogen, die schon gewartet wurden

    for ( int channel = CHANNEL_1; channel <= CHANNEL_4; channel++ )
    {
      _channels[ channel ].sendMessage();
    }

    delay( 4 * tm ); // durch Abzählen ermittelt

    for ( int channel = CHANNEL_1; channel <= CHANNEL_4; channel++ )
    {
      _channels[ channel ].sendMessage();
      delay( 2 * tm ); // durch Abzählen ermittelt
    }
#else
    for ( int channel = CHANNEL_1; channel <= CHANNEL_4; channel++ )
    {
      _channels[ channel ].sendMessage();
    }
    delay( tm );
#endif
  }

  /*
    Set a message for Combo-Direct-Mode
  */
  void transmitter_c::setMessageComboDirect( channel_t channel, comboDirectOutput_t outputA, bool inverseOutputA,
                                             comboDirectOutput_t outputB, bool inverseOutputB )
  {
    _channels[ channel ].setMessageComboDirect( inverseComboDirect( outputA, inverseOutputA ),
                                                inverseComboDirect( outputB, inverseOutputB ) );
  }

  /*
    Set a message for Combo-PWM-Mode
  */
  void transmitter_c::setMessageComboPWM( channel_t channel, pwmOutput_t outputA, bool inverseOutputA,
                                          pwmOutput_t outputB, bool inverseOutputB )
  {
    _channels[ channel ].setMessageComboPWM( inversePwm( outputA, inverseOutputA ), inversePwm( outputB, inverseOutputB ) );
  }

  /*
    Set a message for Extended-Mode
  */
  void transmitter_c::setMessageExtended( channel_t channel, extendedData_t data )
  {
    _channels[ channel ].setMessageExtended( data );
  }

  /*
    Set a message for Single-Output-CSTID-Mode
  */
  void transmitter_c::setMessageSingleOutputCstid( channel_t channel, singleOutput_t output, singleOutputCstid_t data )
  {
    _channels[ channel ].setMessageSingleOutputCstid( output, data );
  }

  /*
    Set a message for Single-Output-PWM-Mode
  */
  void transmitter_c::setMessageSingleOutputPWM( channel_t channel, singleOutput_t output, pwmOutput_t data, bool inverse )
  {
    _channels[ channel ].setMessageSingleOutputPWM( output, inversePwm( data, inverse ) );
  }

  /*
    Invert direction of a PWM message
  */
  transmitter_c::pwmOutput_t transmitter_c::inversePwm( pwmOutput_t value, bool inverse )
  {
    if ( inverse )
    {
      switch ( value )
      {
      case PWM_OUTPUT_FORWARD_1:  return PWM_OUTPUT_BACKWARD_1;
      case PWM_OUTPUT_FORWARD_2:  return PWM_OUTPUT_BACKWARD_2;
      case PWM_OUTPUT_FORWARD_3:  return PWM_OUTPUT_BACKWARD_3;
      case PWM_OUTPUT_FORWARD_4:  return PWM_OUTPUT_BACKWARD_4;
      case PWM_OUTPUT_FORWARD_5:  return PWM_OUTPUT_BACKWARD_5;
      case PWM_OUTPUT_FORWARD_6:  return PWM_OUTPUT_BACKWARD_6;
      case PWM_OUTPUT_FORWARD_7:  return PWM_OUTPUT_BACKWARD_7;
      case PWM_OUTPUT_BACKWARD_7: return PWM_OUTPUT_FORWARD_7;
      case PWM_OUTPUT_BACKWARD_6: return PWM_OUTPUT_FORWARD_6;
      case PWM_OUTPUT_BACKWARD_5: return PWM_OUTPUT_FORWARD_5;
      case PWM_OUTPUT_BACKWARD_4: return PWM_OUTPUT_FORWARD_4;
      case PWM_OUTPUT_BACKWARD_3: return PWM_OUTPUT_FORWARD_3;
      case PWM_OUTPUT_BACKWARD_2: return PWM_OUTPUT_FORWARD_2;
      case PWM_OUTPUT_BACKWARD_1: return PWM_OUTPUT_FORWARD_1;
      default:                    break;
      }
    }

    return value;
  }

  /*
    Invert direction of a Combo-Direct message
  */
  transmitter_c::comboDirectOutput_t transmitter_c::inverseComboDirect( comboDirectOutput_t value, bool inverse )
  {
    if ( inverse )
    {
      switch ( value )
      {
      case COMBO_DIRECT_OUTPUT_FORWARD:  return COMBO_DIRECT_OUTPUT_BACKWARD;
      case COMBO_DIRECT_OUTPUT_BACKWARD: return COMBO_DIRECT_OUTPUT_FORWARD;
      default:                           break;
      }
    }

    return value;
  }

}
