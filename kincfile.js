const fs = require('fs-extra');;
fs.copy('./webcontent', './Deployment/webcontent', err => {
    if (err) return console.error(err)
    console.log('Copied webcontent to Deployment');
})

const system = platform === Platform.Windows ? "win32" :
			   platform === Platform.Linux   ? "linux" :
			   platform === Platform.OSX     ? "macos" :
			   platform === Platform.HTML5   ? "html5" :
			   platform === Platform.Android ? "android" :
			   platform === Platform.iOS     ? "ios" :
                                                  "unknown";

let project = new Project('Kyll'); 

project.kore = false; 

project.setCmd();

if(system == "win32"){
    project.addDefine('WIN32');
}

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
