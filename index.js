let yintcc = require('./lib');

const main = () => {
    const res = yintcc.httpGet('wuriyanto.com');
    console.log(res);
}

main();