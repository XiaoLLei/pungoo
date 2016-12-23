#include "def.h"
#include "webhdfs.h"

#include <json.h>


#define MAX_URL_LEN 512

webhdfs_t* webhdfs_t_create(hdfs_address_t* addr, int num, const char* user)
{
    if (num > MAX_ADDRESS_NUM || num <= 0)
    {
        LOG("need 1 hdfs address at least, 2 at most. num: %d", num);
        return NULL;
    }

    if (!user || strlen(user) < 1 || strlen(user) >= MAX_USER_NAME_LEN)
    {
        LOG("hdfs' user is not right. user: %s", user);
        return NULL;
    }

    webhdfs_t* hdfs = (webhdfs_t*)malloc(sizeof(webhdfs_t));

    if (!hdfs)
    {
        LOG("malloc mem for webhdfs_t failure.");
        return NULL;
    }

    memset(hdfs, 0, sizeof(webhdfs_t));

    hdfs->handle_ = ghttp_request_new();
    if (!hdfs->handle_)
    {
        LOG("webhdfs handle init failure.");
        free(hdfs);
        hdfs = NULL;
        return NULL;
    }

    int i = 0;
    for (; i < num; ++i)
    {
        memcpy(hdfs->address_ + i, addr + i, sizeof(hdfs_address_t));
    }

    strncpy(hdfs->user_, user, MAX_USER_NAME_LEN - 1);

    hdfs->create_file = webhdfs_create_file;
    hdfs->delete_file = webhdfs_delete_file;
    hdfs->read = webhdfs_read;
    hdfs->write = webhdfs_write;
    hdfs->status = webhdfs_status;
    hdfs->create_dir = webhdfs_create_dir;
    hdfs->delete_dir = webhdfs_delete_dir;

    return hdfs;
}

int webhdfs_t_destroy(webhdfs_t* hdfs)
{
    if (hdfs)
    {
        ghttp_clean(hdfs->handle_);
        ghttp_request_destroy(hdfs->handle_);
        hdfs->handle_ = NULL;
        free(hdfs);
    }
    return 0;
}

int webhdfs_create_file(webhdfs_t* hdfs, char* filename)
{
    if (!hdfs)
    {
        LOG("empty webhdfs_t object.");
        return -1;
    }

    if (!filename || strlen(filename) < 1)
    {
        LOG("wrong filename. filename: %s", filename);
        return -1;
    }

    int ret = -1;

    int i = 0;
    for (; i < MAX_ADDRESS_NUM; ++i)
    {
        char url[MAX_URL_LEN] = {0};

        snprintf(
            url,
            MAX_URL_LEN,
            "http://%s:%d/webhdfs/v1%s?user.name=%s&op=CREATE",
            hdfs->address_[hdfs->index_].ip,
            hdfs->address_[hdfs->index_].port,
            filename,
            hdfs->user_
        );

        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_put);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_prepare(hdfs->handle_);

        ghttp_status status = ghttp_process(hdfs->handle_);

        if (ghttp_error == status)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("create hdfs file failed, url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        int code = ghttp_status_code(hdfs->handle_);
        if (307 != code)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("create hdfs file failed, url: %s, http_code: %d", url, code);
            ghttp_clean(hdfs->handle_);
            continue;
        }

        //获取重定向地址
        const char* redirect_url = ghttp_get_header(hdfs->handle_, "Location");
        if (!redirect_url)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            ghttp_clean(hdfs->handle_);
            LOG("create hdfs file failed, no location, url: %s", url);
            continue;
        }

        strncpy(url, redirect_url, MAX_URL_LEN);
        ghttp_clean(hdfs->handle_);

        //根据重定向的location，再次发送请求
        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_put);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_prepare(hdfs->handle_);

        status = ghttp_process(hdfs->handle_);
        if (ghttp_error == status)
        {
            ghttp_clean(hdfs->handle_);
            LOG("create hdfs file failed, redirect url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            break;
        }

        code = ghttp_status_code(hdfs->handle_);
        if (code != 201)
        {
            //过滤掉文件已存在的错误
            if (code == 403)
            {
                char *response = NULL;
                int response_len = 0;
                struct json_object *resp_obj = NULL;
                struct json_object *obj_field = NULL;
                struct json_object *sub_obj_field = NULL;

                response_len = ghttp_get_body_len(hdfs->handle_);
                response = (char*)malloc(response_len + 1);
                if (!response)
                {
                    ghttp_clean(hdfs->handle_);
                    LOG("create hdfs file failed, response malloc failed, redirect url: %s", url);
                    break;
                }

                memcpy(response, ghttp_get_body(hdfs->handle_), response_len);
                response[response_len] = '\0';
                //解析json格式响应
                resp_obj = json_tokener_parse(response);
                if (!resp_obj)
                {
                    free(response);
                    ghttp_clean(hdfs->handle_);
                    LOG("create hdfs file failed, json response parse failed, redirect url: %s", url);
                    break;
                }
                json_object_object_get_ex(resp_obj, "RemoteException", &obj_field);
                if (!obj_field)
                {
                    json_object_put(resp_obj);
                    free(response);
                    ghttp_clean(hdfs->handle_);
                    LOG("create hdfs file failed, json response parse item:RemoteException failed, redirect url: %s", url);
                    break;
                }
                json_object_object_get_ex(obj_field, "exception", &sub_obj_field);
                if (!sub_obj_field)
                {
                    json_object_put(resp_obj);
                    free(response);
                    ghttp_clean(hdfs->handle_);
                    LOG("create hdfs file failed, json response parse item:exception failed, redirect url: %s", url);
                    break;
                }
                if (strcmp("FileAlreadyExistsException", json_object_get_string(sub_obj_field)) == 0)
                {
                    // 文件存在，默认为创建成功
                }
                else
                {
                    LOG("create hdfs file failed, redirect url: %s, exception: %s", url, json_object_get_string(sub_obj_field));
                    json_object_put(resp_obj);
                    free(response);
                    ghttp_clean(hdfs->handle_);
                    ret = -1;
                    break;
                }
            }
            else
            {
                ghttp_clean(hdfs->handle_);
                LOG("create hdfs file failed, redirect url: %s, code: %d", url, code);
                break;
            }
        }

        ghttp_clean(hdfs->handle_);

        ret = 0;

        break;
    }

    return ret;
}

int webhdfs_delete_file(webhdfs_t* hdfs, char* filename)
{
    return webhdfs_delete_dir(hdfs, filename);
}

int webhdfs_read(webhdfs_t* hdfs, char* filename, char* buf, long long offset, int len)
{
    return 0;
}

int webhdfs_write(webhdfs_t* hdfs, char* filename, char* buf, int len)
{
    if (!hdfs)
    {
        LOG("empty webhdfs_t object.");
        return -1;
    }

    if (!buf || len < 1)
    {
        LOG("empty buffer. len: %d", len);
        return -1;
    }

    int ret = -1;

    int i = 0;
    for (; i < MAX_ADDRESS_NUM; ++i)
    {
        char url[MAX_URL_LEN] = {0};

        snprintf(
            url,
            MAX_URL_LEN,
            "http://%s:%d/webhdfs/v1%s?user.name=%s&op=APPEND",
            hdfs->address_[hdfs->index_].ip,
            hdfs->address_[hdfs->index_].port,
            filename,
            hdfs->user_
        );

        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_post);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_prepare(hdfs->handle_);

        ghttp_status status = ghttp_process(hdfs->handle_);

        if (ghttp_error == status)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("write hdfs failed. url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        int code = ghttp_status_code(hdfs->handle_);
        if (code != 307)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("write hdfs failed. url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        //获取重定向地址
        const char * redirect_url = ghttp_get_header(hdfs->handle_, "Location");
        if (!redirect_url)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("write hdfs failed, no redirect location. url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }
        strncpy(url, redirect_url, MAX_URL_LEN);
        ghttp_clean(hdfs->handle_);

        //根据重定向的location，再次发送请求
        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_post);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_set_body(hdfs->handle_, buf, len);

        ghttp_prepare(hdfs->handle_);
        status = ghttp_process(hdfs->handle_);
        if (ghttp_error == status)
        {
            //这种错误忽略掉
            if (errno == EAGAIN && strcmp(ghttp_get_error(hdfs->handle_), "Unknown Error.") == 0)
            {
                LOG("hdfs write file ignore error, redirect url: %s, errno: %d, error: %s, len: %d", url, errno, ghttp_get_error(hdfs->handle_), len);
                ghttp_clean(hdfs->handle_);
                ret = len;
                break;
            }
            else
            {
                LOG("write hdfs file failed, redirect url: %s, errno: %d, error: %s", url, errno, ghttp_get_error(hdfs->handle_));
                ghttp_clean(hdfs->handle_);
                break; // 这种失败不再继续循环
            }
        }
        code = ghttp_status_code(hdfs->handle_);
        if (code != 200)
        {
            ghttp_clean(hdfs->handle_);
            LOG("hdfs write file failed, redirect url: %s, code: %d", url, code);
            break; // 这种失败不再继续循环
        }

        ghttp_clean(hdfs->handle_);

        ret = len;
        break;
    }

    return ret;
}

int webhdfs_status(webhdfs_t* hdfs)
{
    return 0;
}

int webhdfs_create_dir(webhdfs_t* hdfs, char* dir)
{
    if (!hdfs)
    {
        LOG("empty webhdfs_t object.");
        return -1;
    }

    if (!dir || strlen(dir) < 1)
    {
        LOG("wrong dir path. dir: %s", dir);
        return -1;
    }

    int ret = -1;

    int i = 0;
    for (; i < MAX_ADDRESS_NUM; ++i)
    {
        char url[MAX_URL_LEN] = {0};

        snprintf(
            url,
            MAX_URL_LEN,
            "http://%s:%d/webhdfs/v1%s?user.name=%s&op=MKDIRS",
            hdfs->address_[hdfs->index_].ip,
            hdfs->address_[hdfs->index_].port,
            dir,
            hdfs->user_
        );

        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_put);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_prepare(hdfs->handle_);

        ghttp_status status = ghttp_process(hdfs->handle_);

        if (ghttp_error == status)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("create hdfs dir failed, url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        if (ghttp_status_code(hdfs->handle_) != 200)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("create hdfs dir failed, url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        ghttp_clean(hdfs->handle_);

        ret = 0;

        break;
    }

    return ret;
}

int webhdfs_delete_dir(webhdfs_t* hdfs, char* dir)
{
    if (!hdfs)
    {
        LOG("empty webhdfs_t object.");
        return -1;
    }

    if (!dir || strlen(dir) < 1)
    {
        LOG("wrong dir path. dir: %s", dir);
        return -1;
    }

    int ret = -1;

    int i = 0;
    for (; i < MAX_ADDRESS_NUM; ++i)
    {

        char url[MAX_URL_LEN] = {0};

        snprintf(
            url,
            MAX_URL_LEN,
            "http://%s:%d/webhdfs/v1%s?user.name=%s&op=DELETE&recursive=true",
            hdfs->address_[hdfs->index_].ip,
            hdfs->address_[hdfs->index_].port,
            dir,
            hdfs->user_
        );

        ghttp_set_uri(hdfs->handle_, url);
        ghttp_set_type(hdfs->handle_, ghttp_type_delete);
        ghttp_set_header(hdfs->handle_, http_hdr_Timeout, "10");
        ghttp_prepare(hdfs->handle_);

        ghttp_status status = ghttp_process(hdfs->handle_);

        if (ghttp_error == status)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("delete hdfs dir failed, url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        if (ghttp_status_code(hdfs->handle_) != 200)
        {
            hdfs->index_ = (hdfs->index_ + 1) % MAX_ADDRESS_NUM; //连接出错，就切换ip
            LOG("delete hdfs dir failed, url: %s, error: %s", url, ghttp_get_error(hdfs->handle_));
            ghttp_clean(hdfs->handle_);
            continue;
        }

        ghttp_clean(hdfs->handle_);

        ret = 0;

        break;
    }

    return ret;
}