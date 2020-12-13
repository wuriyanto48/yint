let yintcc;
try{
    yintcc = require('../build/Release/yint.node');
} catch(e) {
    if (e.code == 'MODULE_NOT_FOUND') throw e;
    yintcc = require('../build/Debug/yint.node');
}

module.exports = yintcc;