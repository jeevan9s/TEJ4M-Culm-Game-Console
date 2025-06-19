int blue_sw = 2;
int blue_led = 3;
int green_sw = 4;
int green_led = 5;
int white_sw = 6;
int white_led = 7;
int yellow_sw = 8;
int yellow_led = 9;
int red_sw = 10;
int red_led = 11;

int tog = 13;
int buzz = 12;

bool consoleOn = false;
int modeClicks = 0;
unsigned long lastClick = 0;
const unsigned long clickTimeout = 2000;

void setup() {
  pinMode(blue_sw, INPUT_PULLUP);
  pinMode(blue_led, OUTPUT);
  pinMode(green_sw, INPUT_PULLUP);
  pinMode(green_led, OUTPUT);
  pinMode(white_sw, INPUT_PULLUP);
  pinMode(white_led, OUTPUT);
  pinMode(yellow_sw, INPUT_PULLUP);
  pinMode(yellow_led, OUTPUT);
  pinMode(red_sw, INPUT_PULLUP);
  pinMode(red_led, OUTPUT);
  pinMode(tog, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);
}

void loop() {
  if (!consoleOn && digitalRead(tog) == LOW) {
    consoleOn = true;
    startupSequence();
  }

  if (consoleOn) {
    if (digitalRead(tog) == LOW && millis() - lastClick > 250) {
      modeClicks++;
      lastClick = millis();
    }

    if (modeClicks > 0 && millis() - lastClick > clickTimeout) {
      if (modeClicks == 1) {
        whackALight();
      } else if (modeClicks == 2) {
        simonSays();
      }
      modeClicks = 0;
    }
  }
}

void startupSequence() {
  int leds[] = {blue_led, green_led, white_led, yellow_led, red_led};
  int melody[] = {659, 784, 880, 987, 1175, 987, 880, 784, 659};
  int noteDurations[] = {140, 140, 140, 140, 280, 140, 140, 140, 280};
  int numNotes = sizeof(melody) / sizeof(melody[0]);

  for (int repeat = 0; repeat < 3; repeat++) {
    for (int i = 0; i < numNotes; i++) {
      tone(buzz, melody[i]);
      int ledIndex = i % 5;
      digitalWrite(leds[ledIndex], HIGH);
      int speedUp = map(i, 0, numNotes - 1, 30, 0);
      delay(noteDurations[i] - speedUp);
      digitalWrite(leds[ledIndex], LOW);
      noTone(buzz);
      delay(20);
    }
  }

  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], HIGH);
  }
  delay(180);
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], LOW);
  }
  delay(140);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      digitalWrite(leds[j], HIGH);
    }
    delay(90);
    for (int j = 0; j < 5; j++) {
      digitalWrite(leds[j], LOW);
    }
    delay(90);
  }
  noTone(buzz);
}

void whackALight() {
  int leds[] = {blue_led, green_led, white_led, yellow_led, red_led};
  int buttons[] = {blue_sw, green_sw, white_sw, yellow_sw, red_sw};
  int activeIndex;
  int waitTime = 2000;

  randomSeed(analogRead(0)); 
  bool playing = true;

  while (playing) {
    activeIndex = random(0, 5);
    digitalWrite(leds[activeIndex], HIGH);
    unsigned long startTime = millis();
    bool hit = false;

    while (millis() - startTime < waitTime) {
      if (digitalRead(buttons[activeIndex]) == LOW) {
        hit = true;
        break;
      }
      for (int i = 0; i < 5; i++) {
        if (i != activeIndex && digitalRead(buttons[i]) == LOW) {
          playing = false;
          break;
        }
      }
      if (!playing) break;
    }

    digitalWrite(leds[activeIndex], LOW);

    if (hit) {
      tone(buzz, 1000, 100);
      delay(300);
      waitTime *= 0.95;
    } else {
      playing = false;
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 5; j++) digitalWrite(leds[j], HIGH);
    tone(buzz, 200, 100);
    delay(150);
    for (int j = 0; j < 5; j++) digitalWrite(leds[j], LOW);
    delay(150);
  }

  delay(2000);
}

void simonSays() {
  int leds[] = {blue_led, green_led, white_led, yellow_led, red_led};
  int buttons[] = {blue_sw, green_sw, white_sw, yellow_sw, red_sw};
  int sequence[50];
  int waitTime = 3000;
  int seqLen = 0;
  bool playing = true;

  randomSeed(analogRead(0));
  delay(300);

  while (playing && seqLen < 50) {
    int next = random(0, 5);
    sequence[seqLen] = next;
    seqLen++;

    for (int i = 0; i < seqLen; i++) {
      digitalWrite(leds[sequence[i]], HIGH);
      tone(buzz, 300 + sequence[i] * 100, 200);
      delay(400);
      digitalWrite(leds[sequence[i]], LOW);
      delay(150);
    }

    for (int i = 0; i < seqLen; i++) {
      bool correct = false;
      unsigned long start = millis();
      while (millis() - start < waitTime) {
        for (int j = 0; j < 5; j++) {
          if (digitalRead(buttons[j]) == LOW) {
            digitalWrite(leds[j], HIGH);
            tone(buzz, 600 + j * 70, 200);
            delay(250);
            digitalWrite(leds[j], LOW);
            noTone(buzz);
            if (j == sequence[i]) {
              correct = true;
            }
            while (digitalRead(buttons[j]) == LOW);
            break;
          }
        }
        if (correct) break;
      }
      if (!correct) {
        playing = false;
        break;
      }
    }

    delay(400);
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 5; j++) digitalWrite(leds[j], HIGH);
    tone(buzz, 200, 150);
    delay(200);
    for (int j = 0; j < 5; j++) digitalWrite(leds[j], LOW);
    delay(200);
  }

  delay(1000);
  noTone(buzz);
}
