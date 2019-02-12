String[] data;
ONOFF heaterONOFF;
ONOFF coolerONOFF;
ONOFF mixerONOFF;
rONOFF workONOFF;
thermometer t;

PImage img_thermometer;
PImage img_flowmeter; 
PImage img_measure;
boolean cooler;
boolean heater;
boolean mixer;
boolean work;
boolean release;
int current_temperature;
//int nominal_temperature;
int liquid_level;
float cup_liquid_level;
int current_flow;
//int nominal_flow;


// "liquid_level", "current_temperature", "nominal_temperature",
// "current_flow", "nominal_flow", "work", "next", "heater", 
// "cooler", "mixer", "release", "joghurt_valve", "measure_valve"


void setup() {
  frameRate(10);
  size(700, 600);

  img_flowmeter = loadImage("/home/corrot/Pulpit/IP/Projekt/flowmeter.png");
  img_thermometer = loadImage("/home/corrot/Pulpit/IP/Projekt/thermometer.png");
  img_measure = loadImage("/home/corrot/Pulpit/IP/Projekt/measure.png");
}

void draw() {
  background(55);
  drawEnvironment();
  getData();


  drawMeasure();
  drawCup();
  drawFlowmeter();
  drawRelease();
  drawPanel();
}

void getData() {
  fill(255);
  textFont(createFont("Arial", 14));
  if (loadStrings("/home/corrot/Pulpit/IP/Projekt/data.txt") != null) {
    data = loadStrings("/home/corrot/Pulpit/IP/Projekt/data.txt");
  }

 // int textalignment = 250;
  for (int i = 0; i < data.length; i++) {

    if (data[i].contains("liquid_level")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        liquid_level = Integer.parseInt(val);
      }
    }

    if (data[i].contains("current_temperature")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        current_temperature = Integer.parseInt(val);
      }
    }

    /*    if (data[i].contains("nominal_temperature")) {
     if (data[i].split(" ").length > 1) {
     String val = data[i].split(" ")[1];
     nominal_temperature = Integer.parseInt(val);
     }
     }
     */
    if (data[i].contains("current_flow")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        current_flow = Integer.parseInt(val);
      }
    }
    /*
    if (data[i].contains("nominal_flow")) {
     if (data[i].split(" ").length > 1) {
     String val = data[i].split(" ")[1];
     nominal_flow = Integer.parseInt(val);
     }
     }
     */
    if (data[i].contains("heater")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        if (val.equals("1")) 
          heater = true;
        else 
        heater = false;
      }
    }

    if (data[i].contains("mixer")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        if (val.equals("1")) 
          mixer = true;
        else 
        mixer = false;
      }
    }

    if (data[i].contains("release")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        if (val.equals("1")) 
          release = true;
        else 
        release = false;
      }
    }

    if (data[i].contains("cooler")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        if (val.equals("1") )
          cooler = true;
        else 
        cooler = false;
      }
    }

    if (data[i].contains("work")) {
      if (data[i].split(" ").length > 1) {
        String val = data[i].split(" ")[1];
        if (val.equals("1") )
          work = true;
        else 
        work = false;
      }
    }

    /*textAlign(LEFT, RIGHT);
     text(data[i], 10, textalignment);
     textalignment += 20;*/
  }
}

void drawPanel() {
  textFont(createFont("Ubuntu", 24));
  stroke(255);
  textAlign(CENTER, CENTER);
  text("Joghurt \nBottler", 600, 70);
  textFont(createFont("Ubuntu", 18));
  fill(22, 100, 255, 33);
  rect(525, 25, 150, 550); 
  heaterONOFF = new ONOFF(550, 300, 100, 50, "Heater", heater);
  coolerONOFF = new ONOFF(550, 400, 100, 50, "Cooler", cooler);
  mixerONOFF = new ONOFF(550, 500, 100, 50, "Mixer", mixer);
  workONOFF = new rONOFF(50, 30, 30, 30, "Work", work); 
  t = new thermometer(550, 150, current_temperature);

  heaterONOFF.show();
  coolerONOFF.show();
  mixerONOFF.show();
  workONOFF.show();
  t.show();
}

void drawEnvironment() {
  stroke(0);
  beginShape();
  fill(100);
  vertex(120, 0);
  vertex(120, 100);
  vertex(240, 250);
  vertex(240, 300);
  vertex(280, 300);
  vertex(280, 250);
  vertex(400, 100);
  vertex(400, 0);
  endShape();

  fill(4);
  rect(50, 400, 400, 100); 
  rect(75, 500, 25, 100);
  rect(400, 500, 25, 100);
}

void drawMeasure() {
  fill(255, 0, 0);
  noStroke();
  //rect(255, 250, 15,195);
  rect(255, 250-map(liquid_level, 0, 100, 0, 195), 15, map(liquid_level, 0, 100, 0, 195));

  image(img_measure, 225, 0, img_measure.width/2, img_measure.height*3/4);
}

void drawCup() {

  if (release) {
    cup_liquid_level = map(liquid_level, 0, 100, 80, 0);
    fill(255, 0, 0, 150);
    rect(255, 300, 10, 100);
  } else {
    cup_liquid_level = 0;
  }

  noStroke();
  fill(255, 0, 0);
  rect(235, 400-map(cup_liquid_level, 0, 100, 0, 80), 
       50, map(cup_liquid_level, 0, 100, 0, 80));
  stroke(0);

  beginShape();
  fill(255, 30);
  vertex(235, 400);
  vertex(235, 320);
  vertex(285, 320);
  vertex(285, 400);
  endShape();
}

void drawFlowmeter() {
  beginShape();
  fill(0);
  vertex(240, 270);  
  vertex(240, 260);
  vertex(300, 260);
  vertex(300, 270);
  endShape();

  fill(255);
  textAlign(LEFT, LEFT);
  text(current_flow+" [m/s]", 355, 270);
  image(img_flowmeter, 290, 235, img_flowmeter.width/20, img_flowmeter.height/20);
}

void drawRelease() {
}
