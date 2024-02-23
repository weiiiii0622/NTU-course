class RedBird {
  constructor(x, y, r, img) {
    const options = {
      restitution: 0.5
    };
    this.body = Matter.Bodies.circle(x, y, r, options);
    Matter.Body.setMass(this.body, this.body.mass * 4);
    Matter.World.add(world, this.body);
    this.type = "bird";
    this.r = r;
    this.img = img;
  }

  show() {
    const pos = this.body.position;
    const angle = this.body.angle;
    push();
    translate(pos.x, pos.y);
    rotate(angle);
    imageMode(CENTER);
    image(RedBirdImg, 0, 0, this.r * 2, this.r * 2);
    pop();
  }
}
