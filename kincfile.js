const fs = require('fs-extra');;
fs.copy('./webcontent', './Deployment/webcontent', err => {
    if (err) return console.error(err)
    console.log('Copied highlight.js to Deployment')
})

let project = new Project('Kyll'); 
project.kore = false; 

project.setCmd();

project.addIncludeDir('Libraries/md4c/src');
project.addIncludeDir('Libraries/tinydir');
project.addFiles(
    'Libraries/tinydir/tinydir.h',
    'Libraries/md4c/src/md4c.h',
    'Libraries/md4c/src/md4c.c',
    'Libraries/md4c/src/md4c-html.h',
    'Libraries/md4c/src/md4c-html.c',
    'Libraries/md4c/src/entity.h',
    'Libraries/md4c/src/entity.c');
project.addExclude('Libraries/md4c/src/CMakeLists.txt')
project.addFile('Sources/**');
project.setDebugDir('Deployment');
project.icon = './icon.png';

resolve(project);
