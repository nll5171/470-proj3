/* First thing we do is grab a reference ot our socket so that we can use
    it from our client code. The io() function comes from the socket.io
    client side library, which is in a script tag in views/index.handlebars.
*/
const socket = io();

// Create canvas variables
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const { width, height } = canvas.getBoundingClientRect();

const tree = new Image();
const bg = new Image();

let decorator;
let ornaments = [];
let currentOrnament;

const Ornament = class {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        // Sets the image used for the ornament
        this.img = new Image();
        this.img.src = '/assets/img/ornament.png';
    }

    move(x, y) {
        this.x += x;
        this.y += y;
    }
};

const redrawCanvas = () => {
    ctx.clearRect(0, 0, 1240, 810);

    // Draw the background first
    ctx.drawImage(bg, 0, 0);

    // Draw the tree to the screen
    ctx.drawImage(tree, 
        (width / 2) - (tree.naturalWidth / 2),
        (height / 2) - (tree.naturalHeight / 2));

    // Draw all ornaments to the screen
    for (let i = 0; i < ornaments.length; i++) {
        ctx.drawImage(ornaments[i].img, ornaments[i].x, ornaments[i].y);
    }

    requestAnimationFrame(redrawCanvas);
};

const moveOrnament = (input) => {
    // There is no current ornament, so make one
    if (currentOrnament === -1) {
        currentOrnament = ornaments.length;
        const ornament = new Ornament(0, 0);
        ornaments.push(ornament);
    }

    ornaments[currentOrnament].move(input.x, input.y);
};

// Prevent moving the current ornament
const placeOrnament = (data) => {
    console.log(data);
    currentOrnament = -1;
}

const init = () => {
    currentOrnament = -1;
    tree.src = '/assets/img/tree.png';
    bg.src = '/assets/img/canvas-background.png';

    // Check if the server is sending input here
    socket.on('move ornament', moveOrnament);
    socket.on('place ornament', placeOrnament);
    requestAnimationFrame(redrawCanvas);
};

window.onload = init;