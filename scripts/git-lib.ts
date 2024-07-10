import fs from 'fs'
import path from 'path'
import {spawnSync} from 'child_process'

import pc from 'picocolors'

import {DOWNLOAD_DIR, oraP} from './vars'

/**
 * git仓库克隆
 * @param name 库名
 * @param lib 仓库信息
 *
 * @author YJL
 */
export async function gitClone(name: string, lib: GitLib) {
    // git仓库目录存存在，更新
    if (fs.existsSync(path.resolve(DOWNLOAD_DIR, name))) {
        console.log(`${pc.cyan('Updating')} ${name}...`)
        let p = spawnSync('git', ['checkout', '--force', lib.tag], {
            cwd: path.resolve(DOWNLOAD_DIR, name),
            stdio: 'inherit'
        })
        if (p.status !== 0) {
            oraP.fail(`${pc.red('Failed')} to update ${name}, status: ${p.status}`)
            throw new Error(`Failed to update ${name}, status: ${p.status}`)
        }
        oraP.succeed(`Updated ${name}`)
    } else {// 不存在，浅克隆
        console.log(`${pc.cyan('Cloning')} ${name}...`)
        let p = spawnSync('git', ['clone', '--depth=1', '-b', lib.tag, '--single-branch', lib.url, name], {
            cwd: DOWNLOAD_DIR,
            stdio: 'inherit'
        })
        if (p.status !== 0) {
            oraP.fail(`${pc.red('Failed')} to clone ${name}, status: ${p.status}`)
            throw new Error(`Failed to clone ${name}, status: ${p.status}`)
        }
        oraP.succeed(`${pc.green('Cloned')} ${name}`)
    }
}

/**
 * git仓库构建
 * @param name 库名
 * @param lib 仓库信息
 *
 * @author YJL
 */
export async function gitBuild(name: string, lib: GitLib) {
    // 不同构建类型
    for (let buildName in lib.builds) {
        let build = lib.builds[buildName]

        // 构建前命令
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
            oraP.succeed(`${pc.green('Built')} ${name}`)
        }

        // 构建命令
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
            oraP.succeed(`${pc.green('Built')} ${name}`)
        }

        // 安装命令
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
            oraP.succeed(`${pc.green('Installed')} ${name}`)
        }
    }

}