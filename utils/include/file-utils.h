#pragma once

#include <pre.h>
#include <fstream>

/**
 * @brief 判断文件是否存在
 * @param name 文件名
 * @return 是否存在
 *
 * @author YJL
 */
HWPP_API bool file_exists(const std::string &name);

/**
 * @brief 创建空文件
 * @param name 文件名
 * @return 是否创建成功
 *
 * @author YJL
 */
HWPP_API bool file_create_empty(const std::string &name);

/**
 * @brief 读取文件
 * @param name 文件名
 * @param[out] len 文件大小
 * @return 文件内容或空（文件不存在或出现读取错误）
 */
HWPP_API BYTE *file_read(const std::string &name, size_t &len);

/**
 * @brief 写文件
 * @param name 文件名
 * @param content 文本内容
 * @param size 内容大小
 * @return 是否写入成功
 *
 * @author YJL
 */
HWPP_API bool file_write(const std::string &name, void *content, size_t size);

/**
 * @brief 删除文件
 * @param name 文件名
 * @return 是否删除成功
 *
 * @author YJL
 */
HWPP_API bool file_delete(const std::string &name);
