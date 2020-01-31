#include "JECCbotAPI.h"

ApiCommand currentCommand;

ResponseMessage initJECCbot()
{
  HWInit();

  currentCommand.id = CMD_NOP;
  
  ResponseMessage message;
  message.length = sprintf(message.message, ":init\n");
  message.error = false;

  return message;
}

ResponseMessage errorMessage(int error)
{
  ResponseMessage m;

  m.error = true;
  m.length = sprintf(m.message, ":e%03d\n", error);

  return m;
}

ResponseMessage processCommand(char *command)
{
  char *msg = (char *)malloc(sizeof(char)*(strlen(command) + 1));
  strcpy(msg, command);

  ResponseMessage message;

  bool noCommandError = true;
  bool commandNotFoundError = true;
  bool syntaxError = true;

  if (msg[0] == ':')
  {
    noCommandError = false;
    
    if (msg[1] == MSG_DRV_SPEED)
    {
      commandNotFoundError = false;

      int left, right, messageLen;

      if (msg[2] == '-')
      {
        left = -(100 * ((int)msg[3] - 48) + 10 * ((int)msg[4] - 48) + 1 * ((int)msg[5] - 48));
        if (msg[6] == '-')
        {
          right = -(100 * ((int)msg[7] - 48) + 10 * ((int)msg[8] - 48) + 1 * ((int)msg[9] - 48));
          messageLen = 10;
        }
        else
        {
          right = (100 * ((int)msg[6] - 48) + 10 * ((int)msg[7] - 48) + 1 * ((int)msg[8] - 48));
          messageLen = 9;
        }
      }
      else
      {
        left = (100 * ((int)msg[2] - 48) + 10 * ((int)msg[3] - 48) + 1 * ((int)msg[4] - 48));
        if (msg[5] == '-')
        {
          right = -(100 * ((int)msg[6] - 48) + 10 * ((int)msg[7] - 48) + 1 * ((int)msg[8] - 48));
          messageLen = 9;
        }
        else
        {
          right = (100 * ((int)msg[5] - 48) + 10 * ((int)msg[6] - 48) + 1 * ((int)msg[7] - 48));
          messageLen = 8;
        }
      }
      if (msg[messageLen] == '\n')
      {
        syntaxError = false;

        //setMotors(left, right);
        currentCommand.id = CMD_SETMOTORS;
        currentCommand.param1 = left;
        currentCommand.param2 = right;
        
        if (left >= 0)
        {
          if (right >= 0)
          {
            message.length = sprintf(message.message, ":%c%03d%03d\n", MSG_DRV_SPEED, left, right);
          }
          else
          {
            message.length = sprintf(message.message, ":%c%03d-%03d\n", MSG_DRV_SPEED, left, -right);
          }
        }
        else
        {
          if (right >= 0)
          {
            message.length = sprintf(message.message, ":%c-%03d%03d\n", MSG_DRV_SPEED, -left, right);
          }
          else
          {
            message.length = sprintf(message.message, ":%c-%03d-%03d\n", MSG_DRV_SPEED, -left, -right);
          }
        }
        message.error = false;
      }
    }
    else if (msg[1] == MSG_DRV_FREQ)
    {
      commandNotFoundError = false;

      int freq = 10000 * ((int)msg[2] - 48) + 1000 * ((int)msg[3] - 48) + 100 * ((int)msg[4] - 48) + 10 * ((int)msg[5] - 48) + 1 * ((int)msg[6] - 48);
      if (msg[7] == '\n')
      {
        syntaxError = false;

        changeMotorPwmFrequency(freq);

        message.length = sprintf(message.message, ":%c%05d\n", MSG_DRV_FREQ, freq);
        message.error = false;
      }
    }
    else if (msg[1] == MSG_COMPASS_HEADING)
    {
      commandNotFoundError = false;

      if (msg[2] == '\n')
      {
        syntaxError = false;
        int heading = getHeading();
        if(heading > 0)
        {
          message.length = sprintf(message.message, ":%c%03d\n", MSG_COMPASS_HEADING, heading);
        }
        else
        {
          message.length = sprintf(message.message, ":%c-%03d\n", MSG_COMPASS_HEADING, -heading);          
        }
        message.error = false;
      }
      else
      {
        message = errorMessage(MSG_ERROR_SYNTAX);
      }
    }
    else if (msg[1] == MSG_COMPASS_CAL)
    {
      commandNotFoundError = false;

      if (msg[2] == '\n')
      {
        syntaxError = false;

        message.length = sprintf(message.message, ":%c%03d\n", MSG_COMPASS_CAL, getCompassCal());
        message.error = false;
      }
    }
    if (msg[1] == MSG_DRV_HEADING)
    {
      commandNotFoundError = false;

      int heading, speedMax, messageLen;

      if (msg[2] == '-')
      {
        heading = -(100 * ((int)msg[3] - 48) + 10 * ((int)msg[4] - 48) + 1 * ((int)msg[5] - 48));
        if (msg[6] == '-')
        {
          speedMax = -(100 * ((int)msg[7] - 48) + 10 * ((int)msg[8] - 48) + 1 * ((int)msg[9] - 48));
          messageLen = 10;
        }
        else
        {
          speedMax = (100 * ((int)msg[6] - 48) + 10 * ((int)msg[7] - 48) + 1 * ((int)msg[8] - 48));
          messageLen = 9;
        }
      }
      else
      {
        heading = (100 * ((int)msg[2] - 48) + 10 * ((int)msg[3] - 48) + 1 * ((int)msg[4] - 48));
        if (msg[5] == '-')
        {
          speedMax = -(100 * ((int)msg[6] - 48) + 10 * ((int)msg[7] - 48) + 1 * ((int)msg[8] - 48));
          messageLen = 9;
        }
        else
        {
          speedMax = (100 * ((int)msg[5] - 48) + 10 * ((int)msg[6] - 48) + 1 * ((int)msg[7] - 48));
          messageLen = 8;
        }
      }
      if (msg[messageLen] == '\n')
      {
        syntaxError = false;

        currentCommand.id = CMD_DRIVEHEADING;
        currentCommand.param1 = heading;
        currentCommand.param2 = speedMax;
        
        if (heading >= 0)
        {
          if (speedMax >= 0)
          {
            message.length = sprintf(message.message, ":%c%03d%03d\n", MSG_DRV_HEADING, heading, speedMax);
          }
          else
          {
            message.length = sprintf(message.message, ":%c%03d-%03d\n", MSG_DRV_HEADING, heading, -speedMax);
          }
        }
        else
        {
          if (speedMax >= 0)
          {
            message.length = sprintf(message.message, ":%c-%03d%03d\n", MSG_DRV_HEADING, -heading, speedMax);
          }
          else
          {
            message.length = sprintf(message.message, ":%c-%03d-%03d\n", MSG_DRV_HEADING, -heading, -speedMax);
          }
        }
        message.error = false;
      }
    }
  }
  
  if(noCommandError)
  {
    message = errorMessage(MSG_ERROR_NOCOMMAND);
  }
  else if(commandNotFoundError)
  {
    message = errorMessage(MSG_ERROR_NOTFOUND);
  }
  else if(syntaxError)
  {
    message = errorMessage(MSG_ERROR_SYNTAX);
  }

  return message;
}

void updateJECCbot()
{
  switch(currentCommand.id)
  {
    case CMD_NOP: setMotors(0, 0); break;
    case CMD_SETMOTORS: setMotors(currentCommand.param1, currentCommand.param2); break;
    case CMD_DRIVEHEADING: moveHeading(currentCommand.param1, currentCommand.param2); break;
  }
}
