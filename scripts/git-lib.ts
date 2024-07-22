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
        let p = spawnSync('git', ['clone', '--recursive', '--depth=1', '-b', lib.tag, '--single-branch', lib.url, name], {
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
    for (let buildType in lib.builds) {
        let build = lib.builds[buildType]

        function getCmd(cmdName: 'pre-build' | 'build' | 'install') {
            let cmd = build?.[`${cmdName}-cmd` as keyof Builds] as string | undefined
            let extras = build?.[`${cmdName}-cmake-extras` as keyof Builds] as string[] | undefined ?? []
            if (cmd)
                return cmd;
            let args = extras.join(' ')
            let tn = buildType.toLowerCase()
            switch (cmdName) {
                case "pre-build":
                    return `cmake -B build-${tn} -S . ${args} -DCMAKE_INSTALL_PREFIX=../../libs/${name}-${buildType}`
                case "build":
                    return `cd build-${tn} && cmake --build . --config ${buildType}`
                case "install":
                    return `cd build-${tn} && cmake --install . --config ${buildType}`
            }
        }

        // 构建前命令
        let preBuildCmd = getCmd('pre-build')
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
        let buildCmd = getCmd('build')
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
        let installCmd = getCmd('install')
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