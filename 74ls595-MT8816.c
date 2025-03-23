// --- MT8816 Control Pins ---
const int mt8816_chipSelectPin = 7;  // CS
const int mt8816_resetPin = 4;      // RST
const int mt8816_dataPin = 6;      // DI
const int mt8816_clockPin = 5;     // CLK

// --- 74HC595 Control Pins ---
const int shiftRegister_dataPin = 8;    // DS (Data)
const int shiftRegister_latchPin = 9;   // ST_CP (Latch)
const int shiftRegister_clockPin = 10;  // SH_CP (Clock)

// --- MT8816 Commands ---
const byte MT8816_RESET = 0x00;
const byte MT8816_SET = 0x01;
const byte MT8816_CLEAR = 0x00;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial
  }

  // --- MT8816 Pin Modes ---
  pinMode(mt8816_chipSelectPin, OUTPUT);
  pinMode(mt8816_resetPin, OUTPUT);
  pinMode(mt8816_dataPin, OUTPUT);
  pinMode(mt8816_clockPin, OUTPUT);

  // --- 74HC595 Pin Modes ---
  pinMode(shiftRegister_dataPin, OUTPUT);
  pinMode(shiftRegister_latchPin, OUTPUT);
  pinMode(shiftRegister_clockPin, OUTPUT);

  // --- Initial Pin States ---
  digitalWrite(mt8816_chipSelectPin, HIGH);
  digitalWrite(mt8816_clockPin, LOW);
  digitalWrite(mt8816_resetPin, HIGH);
  digitalWrite(shiftRegister_clockPin, LOW); // initialize clock pin to low

  // --- Reset MT8816 ---
  resetMT8816();
    Serial.println("MT8816 Switch Matrix Controller Ready");
    Serial.println("Commands:  'set X Y' to connect, 'clear X Y' to disconnect, 'reset' to reset all, 'q' to quit");
}



void resetMT8816() {
  digitalWrite(mt8816_resetPin, LOW);
  delayMicroseconds(1);
  digitalWrite(mt8816_resetPin, HIGH);
  delayMicroseconds(10);
}



void clearAllSwitches(){
 writeMT8816(MT8816_RESET, 0, 0);
}



void writeMT8816(byte command, byte ax, byte ay) {
  // 1. Send address to 74HC595
  byte addressByte = ax | (ay << 4) ;
  shiftOutAddress(addressByte);

  // 2. Send command bits to MT8816
  digitalWrite(mt8816_chipSelectPin, HIGH);
  sendBit(command); // C1
  digitalWrite(mt8816_chipSelectPin, LOW);
}




void shiftOutAddress(byte address) {
    digitalWrite(shiftRegister_latchPin, LOW);
    Serial.println("addressssss");
    Serial.println(address);
    for (int i = 7; i >= 0; i--) {
        byte byte2send = (address >> i) & 0x01;
        Serial.println(byte2send);
        digitalWrite(shiftRegister_dataPin, byte2send );
        digitalWrite(shiftRegister_clockPin, HIGH);
        digitalWrite(shiftRegister_clockPin, LOW);
    }
    digitalWrite(shiftRegister_latchPin, HIGH);
}


void sendBit(int bitValue) {
  digitalWrite(mt8816_dataPin, bitValue); 
  digitalWrite(mt8816_clockPin, HIGH);
  delayMicroseconds(1);  // Data setup/hold
  digitalWrite(mt8816_clockPin, LOW);
  delayMicroseconds(1);
   Serial.println("SEND BIT");
    Serial.println(bitValue);
}

void setSwitch(byte x, byte y) {
 if (x < 0 || x > 15 || y < 0 || y > 7) {
    Serial.println("Error: Invalid X or Y coordinate.");
    Serial.println(x);
    Serial.println(y);
    return;
  }
  writeMT8816(MT8816_SET, x, y);
}

void clearSwitch(byte x, byte y) {
  if (x < 0 || x > 15 || y < 0 || y > 7) {
    Serial.println("Error: Invalid X or Y coordinate.");
    return;
  }
  writeMT8816(MT8816_CLEAR, x, y);
}

void loop() {
 if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("reset")) {
          clearAllSwitches();
          Serial.println("MT8816 Reset");
        } else if (input.equalsIgnoreCase("q")){
           Serial.println("Exiting...");
           while(true){
               //Infinite loop for stop
           }
        }
          else if (input.equalsIgnoreCase("clearall")) {
          clearAllSwitches();
          Serial.println("All switches cleared");
         }
        else
        {
              // Parse the command.  Expect "set X Y" or "clear X Y"
              int firstSpace = input.indexOf(' ');
              int secondSpace = input.indexOf(' ', firstSpace + 1);

              if (firstSpace != -1 && secondSpace != -1) {
                String command = input.substring(0, firstSpace);
                String xStr = input.substring(firstSpace + 1, secondSpace);
                String yStr = input.substring(secondSpace + 1);

                if (isNumeric(xStr) && isNumeric(yStr)) {
                  byte x = xStr.toInt();
                  byte y = yStr.toInt();

                  if (command.equalsIgnoreCase("set")) {
                    digitalWrite(mt8816_resetPin, LOW);
                    setSwitch(x, y);
                    Serial.print("Set switch: X=");
                    Serial.print(x);
                    Serial.print(", Y=");
                    Serial.println(y);
                  } else if (command.equalsIgnoreCase("clear")) {
                    clearSwitch(x, y);
                    Serial.print("Cleared switch: X=");
                    Serial.print(x);
                    Serial.print(", Y=");
                    Serial.println(y);
                  } else {
                    Serial.println("Error: Invalid command. Use 'set' or 'clear'.");
                  }
                } else {
                  Serial.println("Error: Invalid X or Y values. Must be numeric.");

                }
              } else {
                Serial.println("Error: Invalid command format. Use 'set X Y' or 'clear X Y', 'reset', or 'q'.");
              }
        }
    }
}

bool isNumeric(const String& str) {
  for (int i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}
