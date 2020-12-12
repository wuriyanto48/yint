let yintcc;
try{
    yintcc = require('../build/Release/yintcc.node');
} catch(e) {
    if (e.code == 'MODULE_NOT_FOUND') throw e;
    yintcc = require('../build/Debug/yintcc.node');
}

module.exports = yintcc;