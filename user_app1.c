/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  LcdCommand(LCD_CLEAR_CMD);
  LcdMessage(LINE1_START_ADDR, "Enter passcode:");
  LcdMessage(LINE2_START_ADDR, "0");
  LcdMessage(LINE2_START_ADDR + 6, "1");
  LcdMessage(LINE2_START_ADDR + 13, "2");
  LcdMessage(LINE2_END_ADDR - 4, "ENTER");
  PWMAudioSetFrequency(BUZZER1, 200);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Locked;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Warning;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/

  //Set passcode and length here
  static u8 u8Passcode[10] = {1,2,0,1,1,0,0,0,0,0}; 
  static u8 u8PasscodeLength = 5;
  
  //Other variables
  static u8 u8NumberOfPresses = 0;
  static u8 u8Inputarr[10] = {255,255,255,255,255,255,255,255,255,255};
  static u8 u8Index = 0;
  static u32 u32Timer = 0;
  static u8 u8Matches = 0;
  static u8 u8LockedMessage[] = "Enter passcode:";
  static u8 u8WarningMessage[] = "Incorrect passcode";
  static u8 u8UnlockedMessage[] = "Unlocked";
  static u8 u8ChangeWarningMessage[] = "Code Unsuccessful";
  static u8 u8ChangeSuccessMessage[] = "Code Successful";
  static u16 u16WarningFrequency = 50;
  static u16 u16UnlockedFrequency = 1000;
  static u16 u16SuccessFrequency = 500;
  static u8 u8TempIndex = 0;
  static u8 u8TempArr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
static void UserApp1SM_Locked(void)
{
  u32Timer = 0;
  //Used for entering digits into the input array 
   if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      u8Inputarr[u8Index] = 0;
      u8NumberOfPresses++;
      u8Index++;
    }
  else if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      u8Inputarr[u8Index] = 1;
      u8NumberOfPresses++;
      u8Index++;
    }
   else if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      u8Inputarr[u8Index] = 2;
      u8NumberOfPresses++;
      u8Index++;
    }
  //Checks the current state and/or performs operations
     LedOff(GREEN);
     LedOn(RED);
   //Once enter (button 3) is pressed this checks the input
  if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      u8Index = 0;
      u8Matches = 0;
      u8NumberOfPresses = 0;
      while(u8Index < u8PasscodeLength)
      {
        if(u8Inputarr[u8Index] == u8Passcode[u8Index])
        {
          u8Matches++;
        }
        u8Index++;
      }
      if(u8Matches == u8PasscodeLength)
      {
        u8Index = 0;
        UserApp1_pfStateMachine = UserApp1SM_Unlocked; //change state to unlocked
        PWMAudioSetFrequency(BUZZER1, u16UnlockedFrequency);
        PWMAudioOn(BUZZER1);
        LcdCommand(LCD_CLEAR_CMD);
        LcdMessage(LINE1_START_ADDR, u8UnlockedMessage);
        LedBlink(GREEN, LED_4HZ);
      }
      else
      {
       UserApp1_pfStateMachine = UserApp1SM_Warning; //change state to warning
       PWMAudioOn(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8WarningMessage);
       LedOff(RED);
       LedBlink(RED, LED_8HZ);
      }
    }
   //Checks if the number of entered digits is too long
   if(u8NumberOfPresses > u8PasscodeLength)
     {
       UserApp1_pfStateMachine = UserApp1SM_Warning; //change state to warning
       PWMAudioOn(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8WarningMessage);
       LedOff(RED);
       LedBlink(RED, LED_8HZ);
     }
  //Used to modify passcode
  if(IsButtonHeld(BUTTON0, 1000)&& IsButtonHeld(BUTTON1, 1000))
  {
    u8NumberOfPresses = 0;
    u8TempIndex = 0;
    while(u8Index < 10)
       {
         u8Inputarr[u8Index] = 255;
         u8Index++;
       }
    u8Index = 0;
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Set new passcode:");
    LcdMessage(LINE2_START_ADDR, "0");
    LcdMessage(LINE2_START_ADDR + 6, "1");
    LcdMessage(LINE2_START_ADDR + 13, "2");
    LcdMessage(LINE2_END_ADDR - 4, "ENTER");
    LedOff(RED);
    LedBlink(RED, LED_4HZ);
    LedBlink(GREEN, LED_4HZ);
    UserApp1_pfStateMachine = UserApp1SM_Change; //Change state to changing passcode
  }
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Warning(void)          
{
     u8NumberOfPresses = 0;
     u8Index = 0;
     if(u32Timer == 0)
     {
       while(u8Index < 10)
       {
         u8Inputarr[u8Index] = 255;
         u8Index++;
       }
       u8Index = 0;
     }
     if(u32Timer >= 1500)
     {
       LedOff(RED);
       u32Timer = 0;
       UserApp1_pfStateMachine = UserApp1SM_Locked; //change  to locked
       PWMAudioOff(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8LockedMessage);
       LcdMessage(LINE2_START_ADDR, "0");
       LcdMessage(LINE2_START_ADDR + 6, "1");
       LcdMessage(LINE2_START_ADDR + 13, "2");
       LcdMessage(LINE2_END_ADDR - 4, "ENTER");
     }
     u32Timer++;
} /* end UserApp1SM_Warning() */

static void UserApp1SM_Unlocked(void)          
{
  LedOff(RED);
     if(WasButtonPressed(BUTTON0)||WasButtonPressed(BUTTON1)||WasButtonPressed(BUTTON2)||WasButtonPressed(BUTTON3))
     {
       ButtonAcknowledge(BUTTON0);
       ButtonAcknowledge(BUTTON1);
       ButtonAcknowledge(BUTTON2);
       ButtonAcknowledge(BUTTON3);
       while(u8Index < 10)
       {
         u8Inputarr[u8Index] = 255;
         u8Index++;
       }
       u8Index = 0;
       UserApp1_pfStateMachine = UserApp1SM_Locked; // change state to locked
       PWMAudioSetFrequency(BUZZER1, u16WarningFrequency);
       PWMAudioOff(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8LockedMessage);
       LcdMessage(LINE2_START_ADDR, "0");
       LcdMessage(LINE2_START_ADDR + 6, "1");
       LcdMessage(LINE2_START_ADDR + 13, "2");
       LcdMessage(LINE2_END_ADDR - 4, "ENTER");
       LedOff(GREEN);
     }
} /* end UserApp1SM_Unlocked() */

static void UserApp1SM_Change(void)          
{
  if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      u8TempArr[u8TempIndex] = 0;
      u8TempIndex++;
    }
  else if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      u8TempArr[u8TempIndex] = 1;
      u8TempIndex++;
    }
   else if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      u8TempArr[u8TempIndex] = 2;
      u8TempIndex++;
    }
  
  if(WasButtonPressed(BUTTON3))
     {
       ButtonAcknowledge(BUTTON3);
       if(u8TempIndex == 0||u8TempIndex > 10)
       {
       UserApp1_pfStateMachine = UserApp1SM_ChangeWarning; //change state to change warning
       PWMAudioOn(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8ChangeWarningMessage);
       LedOff(RED);
       LedOff(GREEN);
       LedBlink(RED, LED_8HZ);
       }
       else
       {
       UserApp1_pfStateMachine = UserApp1SM_ChangeSuccess; // change state to success
       PWMAudioSetFrequency(BUZZER1, u16SuccessFrequency);
       PWMAudioOn(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8ChangeSuccessMessage);
       LedOff(GREEN);
       LedOff(RED);
       LedOn(BLUE);
       }
     }
  
//   if(u8TempIndex > 2)
//   {
//       UserApp1_pfStateMachine = UserApp1SM_ChangeWarning; //change state to change warning
//       PWMAudioOn(BUZZER1);
//       LcdCommand(LCD_CLEAR_CMD);
//       LcdMessage(LINE1_START_ADDR, u8ChangeWarningMessage);
//       LedOff(RED);
//       LedOff(GREEN);
//       LedBlink(RED, LED_8HZ);
//   }
} /* end UserApp1SM_Change() */

static void UserApp1SM_ChangeWarning(void)          
{
     u8NumberOfPresses = 0;
     u8TempIndex = 0;
     if(u32Timer == 0)
     {
       while(u8TempIndex < 10)
       {
         u8TempArr[u8TempIndex] = 0;
         u8TempIndex++;
       }
       u8TempIndex = 0;
     }
     if(u32Timer >= 1500)
     {
       LedOff(RED);
       u32Timer = 0;
       UserApp1_pfStateMachine = UserApp1SM_Locked; //change  to locked
       PWMAudioOff(BUZZER1);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8LockedMessage);
       LcdMessage(LINE2_START_ADDR, "0");
       LcdMessage(LINE2_START_ADDR + 6, "1");
       LcdMessage(LINE2_START_ADDR + 13, "2");
       LcdMessage(LINE2_END_ADDR - 4, "ENTER");
     }
     u32Timer++;
} /* end UserApp1SM_ChangeWarning() */

static void UserApp1SM_ChangeSuccess(void)          
{
       if(u32Timer >= 1500)
       {
       while(u8Index < u8PasscodeLength)
       {
         u8Passcode[u8Index] = 0;
         u8Index++;
       }
       u8Index = 0;
       while(u8Index < u8TempIndex)
       {
         u8Passcode[u8Index] = u8TempArr[u8Index];
         u8Index++;
       }
       u8Index = 0;
       u8PasscodeLength = u8TempIndex;
       u8TempIndex = 0;
       UserApp1_pfStateMachine = UserApp1SM_Locked; // change state to locked
       PWMAudioOff(BUZZER1);
       PWMAudioSetFrequency(BUZZER1, u16WarningFrequency);
       LcdCommand(LCD_CLEAR_CMD);
       LcdMessage(LINE1_START_ADDR, u8LockedMessage);
       LcdMessage(LINE2_START_ADDR, "0");
       LcdMessage(LINE2_START_ADDR + 6, "1");
       LcdMessage(LINE2_START_ADDR + 13, "2");
       LcdMessage(LINE2_END_ADDR - 4, "ENTER");
       LedOff(BLUE);
       }
       u32Timer++;
} /* end UserApp1SM_ChangeWarning() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
