# H-Wallpaper - 动态壁纸

小巧，快速的壁纸引擎。

## 开发

需要：

- `cmake` `3.28`以上版本
- `pnpm`

### 开始

- 拉取构建外部依赖。`./setup`

### 构建

构建需要用`MSVC`

```shell
cmake --build build -j 8 --config Release
```

### 安装

`install`目标，生成在构建目录`h-wallpaper <version>`下

```shell
cmake --build build --target install -j 8
```

## 使用

- 拖动视频文件到程序，对于已经运行的实例，会自动切换并播放（单实例）。
- 命令行指定视频文件。
