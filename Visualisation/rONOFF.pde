class rONOFF {
  int x, y;
  int rwidth, rheight;
  String name; 
  Boolean state;

  rONOFF(int x, int y, int rwidth, int rheight, String name, Boolean state) {
    this.x = x;
    this.y = y;
    this.rwidth = rwidth;
    this.rheight = rheight;
    this.name = name;
    this.state = state;
  }

  void show() {
    textAlign(CENTER, CENTER);
    if (state) {
      fill(0, 255, 0);
      ellipse(x, y, rwidth, rheight);
     // fill(0);
     // text("ON", x+rwidth/2, y+rheight/2);
    } else {
      fill(255, 0, 0);
      ellipse(x, y, rwidth, rheight);
     // fill(0);
      //text("OFF", x+rwidth/2, y+rheight/2);
    }
    fill(255);
    textAlign(CENTER, CENTER);
    text(name, x, y+rheight);
  }
}
