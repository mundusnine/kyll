let project = new Project('Kyll');

project.kore = false; 

project.setCmd();

project.addIncludeDir('md4c/src');
project.addFiles('md4c/src/md4c.h','md4c/src/md4c.c','md4c/src/md4c-html.h','md4c/src/md4c-html.c','md4c/src/entity.h','md4c/src/entity.c');
project.addExclude('md4c/src/CMakeLists.txt')
project.addFile('Sources/**');
project.setDebugDir('Deployment');
project.icon = './icon.png';

resolve(project);
