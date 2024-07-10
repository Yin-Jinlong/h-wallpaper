import fs from 'fs'

import yaml from 'yaml'

import {DOWNLOAD_DIR} from './vars'
import {download, extract} from './zip-lib'
import {gitBuild, gitClone} from './git-lib'


async function run() {
    const libFile = yaml.parseDocument(fs.readFileSync('libs.yaml', 'utf8')).toJSON() as LibFile
    if (!fs.existsSync(DOWNLOAD_DIR))
        fs.mkdirSync(DOWNLOAD_DIR)
    for (const libName in libFile.libs) {
        let lib = libFile.libs[libName]
        switch (lib.type) {
            case 'zip':
                await download(libName, lib as ZipLib)
                await extract(libName, lib as ZipLib)
                break
            case 'git':
                await gitClone(libName, lib as GitLib)
                await gitBuild(libName, lib as GitLib)
                break
        }
    }

}

run().then()
