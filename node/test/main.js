const path = require('path');

const projectDir = path.resolve(`${__dirname}`, '../..');
// setenv
var env = process.env;
env.Path += ';' + path.resolve(env.QTDIR_64, 'bin')
        + ';' + path.resolve(projectDir, 'lib/3rdpart/moxa');

const filePath = path.resolve(projectDir, 'node/datastudio/lib/protocore/debug/protocore.node');
var proto = require(filePath);

//

var config = {
    sourceType: 'file',
    filePath: path.resolve(projectDir, 'config/protocol.json')
}
var parser = proto.Parser.create(JSON.stringify(config));
if (parser) {
    var object = parser.parse("1001/2001/ICDTable_0ceb8821fc5d4990bd35faa5a29f6f79", 3, 1);
    console.log(object.name);

}


