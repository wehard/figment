var e = Figment.EntityCreate();
e.addComponent(1);
e.setPosition(-5.0 + e.getHandle() * 1.1, 0, 0);
e.setScale(1, 1, 1);
var s = 1.0;
var r = 1.0;
setInterval(() => {
    s += 0.01 * (1.0 - Math.random());
    r += 0.01 * (1.0 - Math.random());
    e.setPosition(Math.cos(s) * r, Math.sin(s) * r, 0);
}, 1);
