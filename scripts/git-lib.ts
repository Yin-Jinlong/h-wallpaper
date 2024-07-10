import fs from 'fs'
import path from 'path'
import {spawnSync} from 'child_process'

import {DOWNLOAD_DIR, oraP} from './vars'

export async function gitClone(name: string, lib: GitLib) {
    if (!fs.existsSync(path.resolve(DOWNLOAD_DIR, name))) {
        console.log(`Cloning ${name}...`)
        let p = spawnSync('git', ['clone', '--depth=1', '-b', lib.tag, '--single-branch', lib.url, name], {
            cwd: DOWNLOAD_DIR,
            stdio: 'inherit'
        })
        if (p.status !== 0) {
            oraP.fail(`Failed to clone ${name}, status: ${p.status}`)
            throw new Error(`Failed to clone ${name}, status: ${p.status}`)
        }
        oraP.succeed(`Cloned ${name}`)
    } else {
        console.log(`Updating ${name}...`)
        let p = spawnSync('git', ['checkout', '--force', lib.tag], {
            cwd: path.resolve(DOWNLOAD_DIR, name),
            stdio: 'inherit'
        })
        if (p.status !== 0) {
            oraP.fail(`Failed to update ${name}, status: ${p.status}`)
            throw new Error(`Failed to update ${name}, status: ${p.status}`)
        }
        oraP.succeed(`Updated ${name}`)
    }
}

export async function gitBuild(name: string, lib: GitLib) {
    for (let buildName in lib.builds) {
        let build = lib.builds[buildName]
        let preBuildCmd = build['pre-build-cmd']
        if (preBuildCmd) {
            oraP.start(`Pre-build ${name}...`)
            let p = spawnSync('cmd', ['/C', preBuildCmd], {
                cwd: path.resolve(DOWNLOAD_DIR, name),
                stdio: 'inherit'
            })
            if (p.status !== 0) {
                oraP.fail(`Failed to build ${name}, status: ${p.status}`)
                throw new Error(`Failed to build ${name}, status: ${p.status}`)
            }
            oraP.succeed(`Built ${name}`)
        }
        let buildCmd = build['build-cmd']
        if (buildCmd) {
            oraP.start(`Build ${name}...`)
            let p = spawnSync('cmd', ['/C', buildCmd], {
                cwd: path.resolve(DOWNLOAD_DIR, name),
                stdio: 'inherit'
            })
            if (p.status !== 0) {
                oraP.fail(`Failed to build ${name}, status: ${p.status}`)
                throw new Error(`Failed to build ${name}, status: ${p.status}`)
            }
            oraP.succeed(`Built ${name}`)
        }
        let installCmd = build['install-cmd']
        if (installCmd) {
            oraP.start(`Install ${name}...`)
            let p = spawnSync('cmd', ['/C', installCmd], {
                cwd: path.resolve(DOWNLOAD_DIR, name),
                stdio: 'inherit'
            })
            if (p.status !== 0) {
                oraP.fail(`Failed to install ${name}, status: ${p.status}`)
                throw new Error(`Failed to install ${name}, status: ${p.status}`)
            }
            oraP.succeed(`Installed ${name}`)
        }
    }

}