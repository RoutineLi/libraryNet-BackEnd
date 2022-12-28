#ifndef _REDIS_H_
#define _REDIS_H_

#include <vector>
#include <cstring>
#include <string>
#include <memory>
#include <cstdio>
#include <hiredis/hiredis.h>

#include "../../src/Log.h"
#include "../../src/Thread.h"
#include "../book.h"
static Routn::Logger::ptr logger = ROUTN_LOG_NAME("redis");

class Redis
{
public:
    using ptr = std::shared_ptr<Redis>; 
    using RWMutexType = Routn::RW_Mutex;
    Redis(){}
 //释放资源
    ~Redis()
    {
        if(this->_connect){
            delete this->_connect;
            this->_reply = NULL;
        }
        if(this->_reply){
            delete this->_reply;
            this->_reply = NULL;
        }
    }
    

 //创建连接
    bool connect(const char* host = "0.0.0.0", int port = 6379)
    {
        RWMutexType::WriteLock lock(_mutex);
        this->_connect = redisConnect(host, port);
        if(this->_connect != NULL && this->_connect->err)
        {
            ROUTN_LOG_ERROR(logger) << "connect to redis fail";
            return false;
        }
        ROUTN_LOG_INFO(logger) << "connect success!";
        return true;
    }


/// SADD SREM
    bool saddOP(std::string key, std::string& val){
        this->_reply = (redisReply*)redisCommand(this->_connect, "SADD %s %s", key.c_str(), val.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_INTEGER){
            return true;
        }
        return false;
    }

    bool sremOP(std::string key, std::string& val){
        this->_reply = (redisReply*)redisCommand(this->_connect, "SREM %s %s", key.c_str(), val.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_INTEGER){
            return true;
        }
        return false;
    }

/// ZADD ZREM
    bool zaddOP(std::string key, std::string score, std::string& val){
        this->_reply = (redisReply*)redisCommand(this->_connect, "ZADD %s %s %s", key.c_str(), score.c_str(), val.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_INTEGER){
            return true;
        }
        return false;
    }

    bool zremOP(std::string key, std::string member){
        this->_reply = (redisReply*)redisCommand(this->_connect, "ZREM %s %s", key.c_str(), member.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_INTEGER){
            return true;
        }
        return false;
    }

    bool isRangeMember(std::string key, std::string& cmp, int max = 1000000){
        this->_reply = (redisReply*)redisCommand(this->_connect, "ZRANGE %s 0 %d", key.c_str(), max);
        if(this->_reply && this->_reply->elements != 0){
            for(size_t i = 0; i < this->_reply->elements; ++i){
                std::string temp = this->_reply->element[i]->str;
                if(temp == cmp){
                    return true;
                }
            }
        }
        return false;
    }

///del key
    bool delBook(std::string& key){
        RWMutexType::WriteLock lock(_mutex);
        auto bookid = getField(key, "id");
        this->_reply = (redisReply*)redisCommand(this->_connect, "DEL %s", key.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_INTEGER){
            if(this->_reply->integer == 0){
                return false;
            }
            this->_reply = (redisReply*)redisCommand(this->_connect, "DEL %s", bookid.c_str());
            sremOP("books", key);
        }
        return true;
    }

///search key
    bool getKey(std::string& key){
        RWMutexType::WriteLock lock(_mutex);
        this->_reply = (redisReply*)redisCommand(this->_connect, "KEYS %s", key.c_str());
        if(this->_reply && this->_reply->type == REDIS_REPLY_ARRAY){
            for(size_t i = 0; i < this->_reply->elements; ++i){
                if(std::string((const char*)this->_reply->element[i]->str) == key){
                    ROUTN_LOG_INFO(logger) << "find key = " << key;
                    return true;
                }
            }
        }
        return false;
    }

///get请求
    void getField(std::string& key, std::string field, std::string& ret)
    {
        RWMutexType::WriteLock lock(_mutex);
        this->_reply = (redisReply*)redisCommand(this->_connect, "HGET %s %s", key.c_str(), field.c_str());
        if (this->_reply && this->_reply->type==REDIS_REPLY_STRING) {
            ret = (const char*)this->_reply->str;
        }
        ROUTN_LOG_INFO(logger) << "find key: [" << key << "] field: [" << field 
            << "] value: [" << ret << "] success!";
    }

    std::string getField(std::string& key, std::string field)
    {
        RWMutexType::WriteLock lock(_mutex);
        this->_reply = (redisReply*)redisCommand(this->_connect, "HGET %s %s", key.c_str(), field.c_str());
        if (this->_reply && this->_reply->type==REDIS_REPLY_STRING) {
            return {(const char*)this->_reply->str};
        }
        return "";
    }

//持久化操作
    void saveConf(std::string config = "save"){
        RWMutexType::WriteLock lock(_mutex);
        ROUTN_LOG_INFO(logger) << "server now start saving as [" << config << "] mode";
        if(config == "bgsave"){           
            redisCommand(this->_connect, "BGSAVE");
        }else{           
            redisCommand(this->_connect, "SAVE");
        }
    }

///HSET 创建User结构并存储
    void createUser(std::string& name, std::string& id, std::string& password, std::string& token){
        RWMutexType::WriteLock lock(_mutex);
        this->_reply = (redisReply*)redisCommand(this->_connect
            , "HSET %s id %s password %s token %s"
            , name.c_str()
            , id.c_str()
            , password.c_str()
            , token.c_str());
        saddOP("users", name);
    }

///HSET 创建Book结构并存储
    void createBook(std::string& title, std::string id, std::string& body
                    , std::string& category 
                    , std::string& location
                    , std::string count){
        RWMutexType::WriteLock lock(_mutex);
        this->_reply = (redisReply*)redisCommand(this->_connect
            , "HSET %s id %s body %s category %s location %s count %s"
            , title.c_str()
            , id.c_str()
            , body.c_str()
            , category.c_str()
            , location.c_str()
            , count.c_str());
        saddOP("books", title);         
    }

/// SET 修改
    bool setVal(std::string& title, std::string field, std::string val){
        this->_reply = (redisReply*)redisCommand(this->_connect, "HSET %s %s %s", title.c_str(), field.c_str(), val.c_str());
        freeReplyObject(this->_reply);
        return true;
    }

    bool setBookTitle(std::string& old_v, std::string& new_v){
        if(getKey(old_v)){
            this->_reply = (redisReply*)redisCommand(this->_connect, "RENAME %s %s", old_v.c_str(), new_v.c_str());
            //books set
            sremOP("books", old_v);
            saddOP("books", new_v);
            return true;
        }
        return false;
    }

/// GET book
    bool getBook(std::string& key, book::st_BookDefine& book){
        if(!getKey(key)){
            return false;
        }
        this->_reply = (redisReply*)redisCommand(this->_connect, "HGETALL %s", key.c_str());
        if(!this->_reply->elements){
            return false;
        }
        size_t cnt = this->_reply->elements;
        for(size_t i = 0; i < cnt; i += 2){
            std::string temp = this->_reply->element[i]->str;
            if(temp == "count"){
                book.count = atoi(this->_reply->element[i + 1]->str);
            }else if(temp == "location"){
                book.location = this->_reply->element[i + 1]->str;
            }else if(temp == "category"){
                book.category = this->_reply->element[i + 1]->str;
            }else if(temp == "id"){
                book.id = this->_reply->element[i + 1]->str;
            }else if(temp == "body"){
                book.body = this->_reply->element[i + 1]->str;
            }
        }
        book.title = key;
        this->_reply = (redisReply*)redisCommand(this->_connect, "ZCARD %s", book.id.c_str());
        size_t n = this->_reply->integer;
        this->_reply = (redisReply*)redisCommand(this->_connect, "ZRANGE %s 0 %d WITHSCORES", book.id.c_str(), n);
        for(size_t i = 0; i < n * 2; i += 2){
            std::string borrowid = this->_reply->element[i]->str;
            uint64_t returntime = atoi(this->_reply->element[i + 1]->str);
            book._borrows.emplace(borrowid, returntime);
        }
        return true;
    }

    std::vector<book::st_BookDefine> getBooks(){
        std::vector<book::st_BookDefine> ret;
        this->_reply = (redisReply*)redisCommand(this->_connect, "SMEMBERS books");
        if(this->_reply){
            size_t n = this->_reply->elements;
            std::vector<std::string> keys;
            for(size_t i = 0; i < n; ++i){
                keys.push_back(std::string(this->_reply->element[i]->str));
                ROUTN_LOG_INFO(logger) << "keys: " << this->_reply->element[i]->str << ", ";
            }
            for(auto i : keys){
                book::st_BookDefine temp;
                if(getBook(i, temp)){
                    ret.push_back(temp);
                }
            }
        }
        return ret;
    }
private:

    redisContext* _connect = NULL;
    redisReply* _reply = NULL;
    RWMutexType _mutex;

};

#endif  //_REDIS_H_
