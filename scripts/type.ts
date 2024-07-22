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
    /**
     * 只有带此前缀的目录会被创建，复制文件时会匹配到前缀。创建和复制时都会移除前缀进行
     */
    prefix: string
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
    'pre-build-cmd'?: string
    'pre-build-cmake-extras': string[]
    'build-cmd'?: string
    'build-cmake-extras'?: string[]
    'install-cmd'?: string
    'install-cmake-extras'?: string[]
}

/**
 * 库复制映射
 * @author YJL
 */
declare interface LibMap {
    [key: string]: string[]
}
