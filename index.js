let yint = require('./lib');

const main = () => {
    const res = yint.httpGet('wuriyanto.com');
    console.log(res);
}

main();