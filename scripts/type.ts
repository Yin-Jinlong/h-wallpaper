/**
 * @author YJL
 */
declare interface LibFile {
    version: number
    libs: Lib[]
}

/**
 * @author YJL
 */
declare interface Lib {
    type: 'zip' | 'git'
    /**
     * url路径
     */
    url: string
}

/**
 * @author YJL
 */
declare interface ZipLib extends Lib {
    type: 'zip'
    /**
     * 下载文件名
     */
    'download-name': string
    libs: LibMap
}

/**
 * @author YJL
 */
declare interface GitLib extends Lib {
    type: 'git'
    /**
     * git 标签
     */
    tag: string
    /**
     * 多类型构建
     */
    builds: {
        [key: string]: Builds
    }
}

/**
 * @author YJL
 */
declare interface Builds {
    'pre-build-cmd': string
    'build-cmd': string
    'install-cmd': string
}

/**
 * 库复制映射
 * @author YJL
 */
declare interface LibMap {
    [key: string]: string[]
}
