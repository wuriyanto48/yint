const yintcc = require('./build/Release/yintcc');

const main = () => {
    const res = yintcc.getIP('www.wuriyanto.com');
    console.log(res);
}

main();