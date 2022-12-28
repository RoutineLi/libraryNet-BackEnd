/*************************************************************************
	> File Name: book.h
	> Author: 
	> Mail: 
	> Created Time: 2022年11月28日 星期一 18时34分37秒
 ************************************************************************/

#ifndef _BOOK_H
#define _BOOK_H

#include <memory>
#include <string>
#include <map>
#include <cstdint>
#include <sstream>
#include "configor/json.hpp"

using namespace configor;

namespace book{
/**
 * 图书结构体定义
 */

struct st_BookDefine{
    std::string id;                //图书编号
    std::string title;          //书名
    std::string body;           //内容
    std::string category;       //类别
    std::map<std::string, uint64_t> _borrows;   //returnTime&&borrowsId
    std::string location;               //书架位置
    uint32_t count;                     //数量

    std::string toString() {
        std::stringstream ss;
        ss << "title " << title
           << " body " << body
           << " category " << category
           << " location " << location
           << " count " << count;
        return ss.str();
    }

    json::value getReturnTime(){
        json::value ret = json::array{};
        for(auto &i : _borrows){
            ret.push_back(i.second);
        }
        return ret;
    }

    json::value getBorrows(){
        json::value ret = json::array{};
        for(auto &i : _borrows){
            ret.push_back(i.first);
        }
        return ret;
    }
};

struct st_UserInfo{
    std::string id;                //用户id
    std::string name;           //用户名
    std::string password;       //密码
    std::string token;          //用户jwt—token

    std::string toString(){
        std::stringstream ss;
        ss << "name " << name << "id " 
           << id << " password " 
           << password << " token " << token;
        return ss.str();
    }
};




}

#endif
