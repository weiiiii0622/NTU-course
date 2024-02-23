class Ground extends Box {
  constructor(x, y, w, h) {
    super(x, y, w, h);
    this.body.isStatic = true;
    this.type = "ground";
  }

  show() {
    const pos = this.body.position;
    const angle = this.body.angle;
    push();
    translate(pos.x, pos.y);
    rotate(angle);
    noStroke();
    fill(0);
    rectMode(CENTER);
    rect(0, 0, this.w, this.h);
    imageMode(CENTER);
    image(groundImg, 0, 0, this.w, this.h);
    pop();
  }
}
