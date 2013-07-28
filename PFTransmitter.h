#ifndef PF_TRANSMITTER_H
#define PF_TRANSMITTER_H

namespace PF_n
{
  class transmitter_c
  {
  public:
    enum channel_t
    {
      CHANNEL_1   = 0,
      CHANNEL_2   = 1,
      CHANNEL_3   = 2,
      CHANNEL_4   = 3,
      CHANNEL_NUM = 4
    };

    enum singleOutput_t
    {
      SINGLE_OUTPUT_A = 0,
      SINGLE_OUTPUT_B = 1
    };

    enum singleOutputMode_t
    {
      SINGLE_OUTPUT_MODE_PWM   = 0,
      SINGLE_OUTPUT_MODE_CSTID = 1
    };

    enum comboDirectOutput_t
    {
      COMBO_DIRECT_OUTPUT_FLOAT       = 0,
      COMBO_DIRECT_OUTPUT_FORWARD     = 1,
      COMBO_DIRECT_OUTPUT_BACKWARD    = 2,
      COMBO_DIRECT_OUTPUT_BRAKE_FLOAT = 3,
    };

    enum extendedData_t
    {
      EXTENDED_DATA_BRAKE_FLOAT            = 0,
      EXTENDED_DATA_INCREMENT_A            = 1,
      EXTENDED_DATA_DECREMENT_A            = 2,
      EXTENDED_DATA_TOGGLE_FORWARD_FLOAT_B = 4,
      EXTENDED_DATA_TOGGLE_ADDRESS         = 6,
      EXTENDED_DATA_ALIGN_TOGGLE           = 7
    };

    enum pwmOutput_t
    {
      PWM_OUTPUT_FLOAT       = 0,
      PWM_OUTPUT_FORWARD_1   = 1,
      PWM_OUTPUT_FORWARD_2   = 2,
      PWM_OUTPUT_FORWARD_3   = 3,
      PWM_OUTPUT_FORWARD_4   = 4,
      PWM_OUTPUT_FORWARD_5   = 5,
      PWM_OUTPUT_FORWARD_6   = 6,
      PWM_OUTPUT_FORWARD_7   = 7,
      PWM_OUTPUT_BRAKE_FLOAT = 8,
      PWM_OUTPUT_BACKWARD_7  = 9,
      PWM_OUTPUT_BACKWARD_6  = 10,
      PWM_OUTPUT_BACKWARD_5  = 11,
      PWM_OUTPUT_BACKWARD_4  = 12,
      PWM_OUTPUT_BACKWARD_3  = 13,
      PWM_OUTPUT_BACKWARD_2  = 14,
      PWM_OUTPUT_BACKWARD_1  = 15
    };

    enum singleOutputCstid_t
    {
      SINGLE_OUTPUT_CSTID_TOGGLE_FULL_FORWARD          = 0,
      SINGLE_OUTPUT_CSTID_TOGGLE_DIRECTION             = 1,
      SINGLE_OUTPUT_CSTID_INCREMENT_NUMERICAL_PWM      = 2,
      SINGLE_OUTPUT_CSTID_DECREMENT_NUMERICAL_PWM      = 3,
      SINGLE_OUTPUT_CSTID_INCREMENT_PWM                = 4,
      SINGLE_OUTPUT_CSTID_DECREMENT_PWM                = 5,
      SINGLE_OUTPUT_CSTID_FULL_FORWARD                 = 6,
      SINGLE_OUTPUT_CSTID_FULL_BACKWARD                = 7,
      SINGLE_OUTPUT_CSTID_TOGGLE_FULL_FORWARD_BACKWARD = 8,
      SINGLE_OUTPUT_CSTID_CLEAR_C1                     = 9,
      SINGLE_OUTPUT_CSTID_SET_C1                       = 10,
      SINGLE_OUTPUT_CSTID_TOGGLE_C1                    = 11,
      SINGLE_OUTPUT_CSTID_CLEAR_C2                     = 12,
      SINGLE_OUTPUT_CSTID_SET_C2                       = 13,
      SINGLE_OUTPUT_CSTID_TOGGLE_C2                    = 14,
      SINGLE_OUTPUT_CSTID_TOGGLE_FULL_BACLWARD         = 15
    };

  private:
    enum mode_t
    {
      MODE_NONE          = 0,
      MODE_EXTENDED      = 1,
      MODE_COMBO_DIRECT  = 2,
      MODE_SINGLE_OUTPUT = 3,
      MODE_COMBO_PWM     = 4
    };

    enum nibble_t
    {
      NIBBLE_1   = 0,
      NIBBLE_2   = 1,
      NIBBLE_3   = 2,
      NIBBLE_4   = 3,
      NIBBLE_NUM = 4
    };

    class channel_c
    {
    public:
      channel_c();

      static unsigned int cycleLength();
      void                init( int, channel_t );
      static unsigned int maximumMessageLength();
      void                sendMessage();
      void                setMessageComboDirect( comboDirectOutput_t, comboDirectOutput_t );
      void                setMessageComboPWM( pwmOutput_t, pwmOutput_t );
      void                setMessageExtended( extendedData_t );
      void                setMessageSingleOutputCstid( singleOutput_t, singleOutputCstid_t );
      void                setMessageSingleOutputPWM( singleOutput_t, pwmOutput_t );
      void                setMode( mode_t );

    private:
      void endMessage();
      void pauseCycles( unsigned int ) const;
      void pauseTime( unsigned int ) const;
      void writeAddress() const;
      void writeChannel() const;
      void writeData() const;
      void writeEscape() const;
      void writeHighBit() const;
      void writeLowBit() const;
      void writeLRC() const;
      void writeMark() const;
      void writeMode() const;
      void writeNibbles() const;
      void writePwmOutput() const;
      void writeStartStopBit() const;
      void writeToggle() const;

      mutable unsigned int _actualMessageLength;
      channel_t            _channel;
      unsigned int         _data;
      mode_t               _mode;
      mutable unsigned int _nibbles[ NIBBLE_NUM ];
      int                  _pin;
      unsigned int         _outputA;
      unsigned int         _outputB;
      unsigned int         _repeats;
      singleOutput_t       _singleOutput;
      singleOutputMode_t   _singleOutputMode;
      bool                 _toggle;
    };

  public:
    transmitter_c( int );

    void sendMessages();
    void setMessageComboDirect( channel_t, comboDirectOutput_t, bool, comboDirectOutput_t, bool );
    void setMessageComboPWM( channel_t, pwmOutput_t, bool, pwmOutput_t, bool );
    void setMessageExtended( channel_t, extendedData_t );
    void setMessageSingleOutputCstid( channel_t, singleOutput_t, singleOutputCstid_t );
    void setMessageSingleOutputPWM( channel_t, singleOutput_t, pwmOutput_t, bool );

  private:
    static comboDirectOutput_t inverseComboDirect( comboDirectOutput_t, bool );
    static pwmOutput_t         inversePwm( pwmOutput_t, bool );

    channel_c _channels[ CHANNEL_NUM ];
    int       _pin;
  };
}

#endif
