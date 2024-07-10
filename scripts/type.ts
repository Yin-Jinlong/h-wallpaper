declare interface LibFile {
    version: number
    libs: Lib[]
}

declare interface Lib {
    type: 'zip' | 'git'
    url: string
}

declare interface ZipLib extends Lib {
    type: 'zip'
    'download-name': string
    libs: LibMap
}

declare interface GitLib extends Lib {
    type: 'git'
    tag: string
    builds: {
        [key: string]: Builds
    }
}

declare interface Builds {
    'pre-build-cmd': string
    'build-cmd': string
    'install-cmd': string
}

declare interface LibMap {
    [key: string]: string[]
}
