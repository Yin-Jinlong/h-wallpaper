import path from 'path'
import fs from 'fs'

import JSZip from 'jszip'
import pc from 'picocolors'

import {DOWNLOAD_DIR, oraP} from './vars'

/**
 * 下载zip
 * @param name 库名
 * @param lib 库信息
 *
 * @author YJL
 */
export async function download(name: string, lib: ZipLib) {
    oraP.start(`Downloading ${name}...`)
    let downloadFile = path.resolve(DOWNLOAD_DIR, lib['download-name'])
    if (fs.existsSync(downloadFile)) {
        oraP.info(`${name} already downloaded`)
        return
    }
    oraP.text = `fetching ${name}...`
    let res = await fetch(lib.url)
    if (res.status !== 200) {
        oraP.fail(`Failed to download ${name}, status: ${res.status}`)
        throw new Error(`Failed to download ${name}, status: ${res.status}`)
    }
    let buf = await res.arrayBuffer()
    fs.writeFileSync(downloadFile, Buffer.from(buf))
    oraP.succeed(`Downloaded ${name}`)
}

/**
 * 解压zip
 * @param name 库名
 * @param lib 库信息
 *
 * @author YJL
 */
export async function extract(name: string, lib: ZipLib) {
    return new Promise(async resolve => {

        console.log(`${pc.cyan('Extracting')} ${name}...`)
        let patternMap = new Map<RegExp | string, string>()
        for (let dst in lib.libs) {
            for (let pattern of lib.libs[dst]) {
                if (pattern.endsWith('/')) {
                    if (pattern == '/')
                        pattern = ''
                    patternMap.set(pattern, dst)
                } else
                    patternMap.set(new RegExp(pattern), dst)
            }
        }
        let zipInstance = new JSZip()
        let zip = await zipInstance.loadAsync(fs.readFileSync(path.resolve(DOWNLOAD_DIR, lib['download-name'])))

        for (const relativePath in zip.files) {
            let file = zip.files[relativePath]
            let dst = ''
            for (const pattern of patternMap.keys()) {
                if (typeof pattern === 'string') {
                    if (relativePath.startsWith(pattern)) {
                        dst = patternMap.get(pattern)! + file.name.substring(pattern.length)
                        dst = path.dirname(dst)
                        break
                    }
                } else {
                    if (pattern.test(relativePath)) {
                        dst = patternMap.get(pattern)!
                        break
                    }
                }
            }

            if (!dst)
                continue
            if (file.dir) {
                fs.mkdirSync(path.resolve(dst, file.name), {recursive: true})
                continue
            }

            if (!fs.existsSync(dst))
                fs.mkdirSync(dst, {recursive: true})

            let data = await file.async('uint8array')
            let name = path.basename(file.name)
            let dstFile = path.resolve(dst, name)
            if (fs.existsSync(dstFile)) {
                oraP.info(`${pc.yellow('skip')} ${name}`)
            } else {
                oraP.text = `Extracting ${file.name} to ${dst}`
                fs.writeFileSync(path.resolve(dst, name), data)
            }
        }
        oraP.succeed(`${pc.green('Extracted')} ${name}`)
        resolve(undefined)
    })
}
