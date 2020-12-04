const yintcc = require('./build/Release/yintcc');

const main = () => {
    const res = yintcc.httpGet('www.wuriyanto.com');
    console.log(res);
}

main();