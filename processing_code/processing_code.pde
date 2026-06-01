/*
  Change of Heart — Processing (audio)
  Receives BPM from Arduino via Serial and plays the heartbeat sound.

  Instructions:
    1. Place "latido.wav" in the same folder as this file.
    2. Replace "COM3" with the port your Arduino is connected to
       (check in Arduino IDE → Tools → Port).
    3. Make sure the baud rate matches the Arduino (9600).

  Requires the Sound library for Processing:
    Sketch → Import Library → Sound
*/

import processing.serial.*;
import processing.sound.*;

Serial port;
SoundFile sound;
float receivedBPM = 0;

void setup() {
  size(400, 200);
  background(255);

  // Replace "COM3" with your port
  port = new Serial(this, "COM3", 9600);

  // latido.wav must be in the same folder as this .pde
  sound = new SoundFile(this, "latido.wav");
}

void draw() {
  if (port.available() > 0) {
    String data = port.readStringUntil('\n');
    if (data != null) {
      data = data.trim();

      // Ignore text messages (e.g. "PulseSensor ready")
      try {
        float bpm = Float.parseFloat(data);

        if (bpm >= 40 && bpm <= 200) {
          receivedBPM = bpm;

          // Display BPM on screen
          background(255);
          fill(200, 0, 0);
          textSize(20);
          text("♥  BPM: " + int(receivedBPM), 20, 60);

          // Play sound on each received heartbeat
          if (sound.isPlaying()) {
            sound.stop();
          }
          sound.play();
        }
      } catch (NumberFormatException e) {
        // Non-numeric line — ignore
      }
    }
  }
}
