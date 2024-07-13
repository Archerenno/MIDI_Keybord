//Archer Simpson, October 2023
//Throughout my project I have utilised portions of code from avaliable libary sources including: A library demo of IR remote controls by Nicohood; 'IRLRemote', A library demo of MIDI controls by Arduino; 'MIDIUSB' adapted with
//refrence to this youtube tutorial posted by Good Electronics: https://www.youtube.com/watch?v=K-OPme8-BNA. All other code has been developed from scratch by yours truly

#include "PinChangeInterrupt.h"
#include "IRLremote.h"
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
//including all the libraries to variables and functions I will be using in my code

#define pinIR 8
CNec IRLremote;
//defining the pin number where the IR reciever is located. Line 12 refers to type of protocol that is used for this device. For example, CNec would
//be replaced with CApple if you were working with an apple device

int CX = 60;
int DX = 62;
int EX = 64;
int FX = 65;
int GX = 67;
int AX = 69;
int BX = 71;
//Each of these variables are accosciated with an individual note e.g AX is for note A. The number that follows each note variable is a MIDI note number. Note numbers range from 0 to 127 and fill the pitch parameter in the MIDI
//NoteOn and NoteOff functions. The note numbers I have attached to these varibles correspond to the note in the 4th octave which is widely considered to be the base octave

int i = 1;
//The variable i is a counting variable and therefore it is coding standard that this varibale starts at 1

void noteOn(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };

  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };

  MidiUSB.sendMIDI(noteOff);
}
// The two above functions initilaise the NoteOn and NoteOff commands so they can be used later in my program. The first lines set out the parameters needed for MIDI notes; channel, pitch and velocity. I have an in depth
// explanation of what each of these parameters do in my documentation

int button_pressed[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
//This is an array which tracks the state of all 7 of the note buttons in my design, hence one item in the array corresponds to one button. 0 represents a LOW button state and 1 represents a HIGH button state

int keyboard_mode = 0;
//Setting the default mode of the device to the IR remote mode

void setup() {
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(11, INPUT);
  pinMode(10, INPUT);
  pinMode(9, INPUT);
  pinMode(4, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  //Tells the arduino which ports components are plugged into and whether the component produces and output (LED/Speaker etc) or takes an input (Button/Potentiometer etc)
  Serial.begin(31250);
  //Sets the baud rate to 31,250 baud/second
  if (!IRLremote.begin(pinIR))
    Serial.println(F("You did not choose a valid pin."));
    //Checks that the pin you have defined as your IR reciver port is valid
}

void loop() {

  if (keyboard_mode == 0) {

    // Reads the new data sent to the reciever from the remote
    auto data = IRLremote.read();
    int command = data.command;
    // Transfers the button readings from the remote into a command number that is refered to in my code
    Serial.println();
    Serial.print(F("Address: 0x"));
    Serial.println(data.address, HEX);
    Serial.print(F("Command: "));
    Serial.println(command);
    //The Serial prints above are just presenting the readings obtained by the remote in an clear and easy to read format. In the serial monitor readings the Address (Slave address used for I2C communication) and the command 
    //number is displayed. When a number is not being pressed the same format is still printed but the command will be 0 and the address will be '0x0'

    if (command == 28) {
      tone(3, 261, 100);
      keyboard_mode = 1;
      //If the 'OK' button is pressed on the IR remote (corresponding command number is 28), the keyboard mode switches to the wired buttons. A note on the speaker is also played to give the user confirmation the mode has been 
      //changed
    }
//Each of the repeated if statements like below is linked to button on the IR remote by a command = X where X is the number associated with a button on
//the IR remote e.g command 25 is button 0 on the controller/remote. Each statement is checking to see if the button has been pressed by looking for the corresponding command signal. 
//After that another nested if statement checks to see if the button is currently being pressed or not, according to the array. This stops a note being repeated whilst holding it. 
//A small debugging line Serial.println(X) will be helpful to check if button presses are being register when an error occurs.
//When a button press is registered a note is sent in two parts where the first part is noteOn(X, Y, Z) essenially determining the note being sent. The next line and second part; 'MidiUSB.flush' is a library function
//That sends the MIDI note out to the connected device, so in my context, sends the note to the music software through the USB port. Finally, the button state as held by the array is changed to HIGH to reflect that the button
//is currently being pressed/held down
    if (command == 69) {
      if (button_pressed[0] == 0) {
        Serial.println("Pin 13!");
        noteOn(0, CX, 64);  //channel 0, pitch 60 (C4 Note), 64 (Standard Velocity)
        MidiUSB.flush();
        button_pressed[0] = 1;
      }
      //Each of the else if statements, such as the one seen below, serve the purpose of turning off the note (from the previous if statement) and re-setting the button state as per the array. The parameters in the else if loops check
      //the button has just been released hence the check for both a low state (no press) and the check that it has just been pressed which is derived from the array number 1 in the buttons index. The NoteOff(X,Y,Z) has the same
      //values as its partnered NoteOn command to signal that the note being played is the one being turned off. This signal is flushed to implement it and the button state in the array has been re-set to a LOW state using the number 0
    } else if (command == 25 && button_pressed[0] == 1) {
      noteOff(0, CX, 64);
      MidiUSB.flush();
      button_pressed[0] = 0;
    }
    if (command == 70) {
      if (button_pressed[1] == 0) {
        Serial.println("Pin 12!");
        noteOn(0, DX, 64);
        MidiUSB.flush();
        button_pressed[1] = 1;
      }
    } else if (command == 25 && button_pressed[1] == 1) {
      Serial.println("stopping!");
      noteOff(0, DX, 64);
      MidiUSB.flush();
      button_pressed[1] = 0;
    }
    if (command == 71) {
      if (button_pressed[2] == 0) {
        Serial.println("Pin 11!");
        noteOn(0, EX, 64);
        MidiUSB.flush();
        button_pressed[2] = 1;
      }
    } else if (command == 25 && button_pressed[2] == 1) {
      noteOff(0, EX, 64);
      MidiUSB.flush();
      button_pressed[2] = 0;
    }
    if (command == 68) {
      if (button_pressed[3] == 0) {
        Serial.println("Pin 10!");
        noteOn(0, FX, 64);
        MidiUSB.flush();
        button_pressed[3] = 1;
      }
    } else if (command == 25 && button_pressed[3] == 1) {
      noteOff(0, FX, 64);
      MidiUSB.flush();
      button_pressed[3] = 0;
    }
    if (command == 64) {
      if (button_pressed[4] == 0) {
        Serial.println("Pin 9!");
        noteOn(0, GX, 64);
        MidiUSB.flush();
        button_pressed[4] = 1;
      }
    } else if (command == 25 && button_pressed[4] == 1) {
      noteOff(0, GX, 64);
      MidiUSB.flush();
      button_pressed[4] = 0;
    }
    if (command == 67) {
      if (button_pressed[5] == 0) {
        Serial.println("Pin 8!");
        noteOn(0, AX, 64);
        MidiUSB.flush();
        button_pressed[5] = 1;
      }
    } else if (command == 25 && button_pressed[5] == 1) {
      noteOff(0, AX, 64);
      MidiUSB.flush();
      button_pressed[5] = 0;
    }
    if (command == 7) {
      if (button_pressed[6] == 0) {
        Serial.println("Pin 7!");
        noteOn(0, BX, 64);
        MidiUSB.flush();
        button_pressed[6] = 1;
      }
    } else if (command == 25 && button_pressed[6] == 1) {
      noteOff(0, BX, 64);
      MidiUSB.flush();
      button_pressed[6] = 0;
    }

    if (command == 21) {
      if (button_pressed[7] == 0) {
        Serial.println("Special Note!");
        noteOn(0, (CX + 12), 64);
        MidiUSB.flush();
        button_pressed[7] = 1;
      }
    } else if (command == 25 && button_pressed[7] == 1) {
      noteOff(0, (CX + 12), 64);
      MidiUSB.flush();
      button_pressed[7] = 0;
    }

    if (command == 24) {
      Serial.println("Pin 6!");
      noteOff(0, CX, 64);
      noteOff(0, DX, 64);
      noteOff(0, EX, 64);
      noteOff(0, FX, 64);
      noteOff(0, GX, 64);
      noteOff(0, AX, 64);
      noteOff(0, BX, 64);
      if (i == 1) {
        CX = CX + 12;
        DX = DX + 12;
        EX = EX + 12;
        FX = FX + 12;
        GX = GX + 12;
        AX = AX + 12;
        BX = BX + 12;
        //In MIDI notes every 12 note numbers the octave of a single note rises. E.g C4 is note 60 and C5 is note 72. Therefore adding 12 to all the notes is the same as rising the octave by one
        i = i + 1;
        tone(3, 261, 100);
        //The counting variable is the condition for this statement because it prevents 'button bouncing' which is when the button reads multiple highs and therefore activates the code multiple times.
        if (CX > 127) {
          Serial.println("x2");
          
          CX = CX - 12;
          DX = DX - 12;
          EX = EX - 12;
          FX = FX - 12;
          GX = GX - 12;
          AX = AX - 12;
          BX = BX - 12;
          Serial.println(CX);
        }
        //Checks that the note number has not exceeded the maximum value for any note which is 127. If this number has been exceeded, the octave increase is 'undone' by reversing the plus 12 procedure that was originally completed
        delay(1000);
        i = 1;
      }
    }
    if (command == 82) {
      Serial.println("Pin 5!");
      noteOff(0, CX, 64);
      noteOff(0, DX, 64);
      noteOff(0, EX, 64);
      noteOff(0, FX, 64);
      noteOff(0, GX, 64);
      noteOff(0, AX, 64);
      noteOff(0, BX, 64);
      if (i == 1) {
        CX = CX - 12;
        DX = DX - 12;
        EX = EX - 12;
        FX = FX - 12;
        GX = GX - 12;
        AX = AX - 12;
        BX = BX - 12;
        i = i + 1;
        tone(3, 261, 100);
        if (CX < 21) {
          CX = CX + 12;
          DX = DX + 12;
          EX = EX + 12;
          FX = FX + 12;
          GX = GX + 12;
          AX = AX + 12;
          BX = BX + 12;
        }
        delay(1000);
        i = 1;
      }
      // This second group of nested if statements is the reverse of the above block. This button lowers the octave but still checks that the octave never goes below octave 0 (Note 21)
    }
//Same purpose & function as the 'command = X' if loops above but instead these refer to digitalRead(X) which is reading the state of the physical button (as HIGH or LOW) connected into the breadboard
  } else if (keyboard_mode == 1) {
    if (digitalRead(13) == HIGH) {
      if (button_pressed[0] == 0) {
        Serial.println("Pin 13!");
        noteOn(0, CX, 64);  //channel 0, pitch 60 (C4 Note), 64 (Standard Velocity)
        MidiUSB.flush();
        button_pressed[0] = 1;
      }
      //Same purpose & function as the 'command = X' else if loops above but instead these refer to digitalRead(X) which is reading the state of the physical button (as HIGH or LOW) connected into the breadboard
    } else if (digitalRead(13) == LOW && button_pressed[0] == 1) {
      noteOff(0, CX, 64);
      MidiUSB.flush();
      button_pressed[0] = 0;
    }
    if (digitalRead(12) == HIGH) {
      if (button_pressed[1] == 0) {
        Serial.println("Pin 12!");
        noteOn(0, DX, 64);
        MidiUSB.flush();
        button_pressed[1] = 1;
      }
    } else if (digitalRead(12) == LOW && button_pressed[1] == 1) {
      noteOff(0, DX, 64);
      MidiUSB.flush();
      button_pressed[1] = 0;
    }
    if (digitalRead(11) == HIGH) {
      if (button_pressed[2] == 0) {
        Serial.println("Pin 11!");
        noteOn(0, EX, 64);
        MidiUSB.flush();
        button_pressed[2] = 1;
      }
    } else if (digitalRead(11) == LOW && button_pressed[2] == 1) {
      noteOff(0, EX, 64);
      MidiUSB.flush();
      button_pressed[2] = 0;
    }
    if (digitalRead(10) == HIGH) {
      if (button_pressed[3] == 0) {
        Serial.println("Pin 10!");
        noteOn(0, FX, 64);
        MidiUSB.flush();
        button_pressed[3] = 1;
      }
    } else if (digitalRead(10) == LOW && button_pressed[3] == 1) {
      noteOff(0, FX, 64);
      MidiUSB.flush();
      button_pressed[3] = 0;
    }
    if (digitalRead(9) == HIGH) {
      if (button_pressed[4] == 0) {
        Serial.println("Pin 9!");
        noteOn(0, GX, 64);
        MidiUSB.flush();
        button_pressed[4] = 1;
      }
    } else if (digitalRead(9) == LOW && button_pressed[4] == 1) {
      noteOff(0, GX, 64);
      MidiUSB.flush();
      button_pressed[4] = 0;
    }
    if (digitalRead(4) == HIGH) {
      if (button_pressed[5] == 0) {
        Serial.println("Pin 4!");
        noteOn(0, AX, 64);
        MidiUSB.flush();
        button_pressed[5] = 1;
      }
    } else if (digitalRead(4) == LOW && button_pressed[5] == 1) {
      noteOff(0, AX, 64);
      MidiUSB.flush();
      button_pressed[5] = 0;
    }
    if (digitalRead(7) == HIGH) {
      if (button_pressed[6] == 0) {
        Serial.println("Pin 7!");
        noteOn(0, BX, 64);
        MidiUSB.flush();
        button_pressed[6] = 1;
      }
    } else if (digitalRead(7) == LOW && button_pressed[6] == 1) {
      noteOff(0, BX, 64);
      MidiUSB.flush();
      button_pressed[6] = 0;
    }

    if (digitalRead(5) == HIGH) {
      Serial.println("Pin 6!");
      noteOff(0, CX, 64);
      noteOff(0, DX, 64);
      noteOff(0, EX, 64);
      noteOff(0, FX, 64);
      noteOff(0, GX, 64);
      noteOff(0, AX, 64);
      noteOff(0, BX, 64);
        //The noteOff's turn off any current playing notes so that when the octave changes no note is left turned on. This gets rid of the sticking problem that was found during summative testing
      if (i == 1) {
        CX = CX + 12;
        DX = DX + 12;
        EX = EX + 12;
        FX = FX + 12;
        GX = GX + 12;
        AX = AX + 12;
        BX = BX + 12;
        //In MIDI notes every 12 note numbers the octave of a single note rises. E.g C4 is note 60 and C5 is note 72. Therefore adding 12 to all the notes is the same as rising the octave by one
        i = i + 1;
        tone(3, 261, 100);
        //The counting variable is the condition for this statement because it prevents 'button bouncing' which is when the button reads multiple highs and therefore activates the code multiple times.
        if (CX > 108) {
          Serial.println("x2");
          CX = CX - 12;
          DX = DX - 12;
          EX = EX - 12;
          FX = FX - 12;
          GX = GX - 12;
          AX = AX - 12;
          BX = BX - 12;
          Serial.println(CX);
        }
        //Checks that the note number has not exceeded the maximum value for any note which is 127. If this number has been exceeded, the octave increase is 'undone' by reversing the plus 12 procedure that was originally completed
        delay(1000);
        i = 1;
      }
    }
    if (digitalRead(6) == HIGH) {
      Serial.println("Pin 5!");
      noteOff(0, CX, 64);
      noteOff(0, DX, 64);
      noteOff(0, EX, 64);
      noteOff(0, FX, 64);
      noteOff(0, GX, 64);
      noteOff(0, AX, 64);
      noteOff(0, BX, 64);
      //The noteOff's turn off any current playing notes so that when the octave changes no note is left turned on. This gets rid of the sticking problem that was found during summative testing
      if (i == 1) {
        CX = CX - 12;
        DX = DX - 12;
        EX = EX - 12;
        FX = FX - 12;
        GX = GX - 12;
        AX = AX - 12;
        BX = BX - 12;
        i = i + 1;
        tone(3, 261, 100);
        if (CX < 21) {
          CX = CX + 12;
          DX = DX + 12;
          EX = EX + 12;
          FX = FX + 12;
          GX = GX + 12;
          AX = AX + 12;
          BX = BX + 12;
        }
        delay(1000);
        i = 1;
      }
    }
    if (digitalRead(5) == HIGH && digitalRead(6) == HIGH) {
      tone(3, 261, 100);
      keyboard_mode = 0;
      //This if loop is looking for when both octave buttons (on pins 5 & 6) are pressed down at the same time. This command changes the keyboard mode back to wireless. When the mode is changed a note is played to tell the user
      //their attempt to change mode has been sucsessful and they are ready to go
    }
  }
}
