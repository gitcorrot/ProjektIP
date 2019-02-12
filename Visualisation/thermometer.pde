class thermometer {
  int x, y, current_temp, nominal_temp;

  public thermometer(int x, int y, int current_temp) {
    this.x = x;
    this.y = y;
    this.current_temp = current_temp;
  }

  void show() {
    textFont(createFont("Ubuntu", 14));
    image(img_thermometer, x-10, y, img_thermometer.width/3, img_thermometer.height/3);
    fill(255);
    textAlign(CENTER, CENTER);
    text("Current\ntemperature\n" +current_temp+" °C", x+img_thermometer.width-17, y+img_thermometer.height/6);
  }
}
