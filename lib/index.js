let yint;
try{
    yint = require('../build/Release/yint.node');
} catch(e) {
    if (e.code == 'MODULE_NOT_FOUND') throw e;
    yint = require('../build/Debug/yint.node');
}

module.exports = yint;