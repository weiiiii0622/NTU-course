class Pig extends RedBird {
  constructor(x, y, r, img) {
    super(x, y, r, img);
    this.type = "pig";
    this.isDead = false;
  }

  show() {
    const pos = this.body.position;
    const angle = this.body.angle;
    push();
    translate(pos.x, pos.y);
    rotate(angle+0.2);
    imageMode(CENTER);
    image(this.img, 0, 0, this.r * 2.2, this.r * 2.2);
    pop();
  }
}
