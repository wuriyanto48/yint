let yintcc = require('./lib');

const main = () => {
    const res = yintcc.httpGet('tokopedia.com');
    console.log(res);
}

main();