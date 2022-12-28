/*************************************************************************
	> File Name: book_servlet.h
	> Author: 
	> Mail: 
	> Created Time: 2022年11月28日 星期一 19时22分21秒
 ************************************************************************/

#ifndef _BOOK_SERVLET_H
#define _BOOK_SERVLET_H

#include "../src/http/HttpServlet.h"
#include "configor/json.hpp"
#include "redis/redis.h"
#include "book.h"

using namespace book;

//注册(无权限servlet)
class RegServlet : public Routn::Http::Servlet {
public:
    using ptr = std::shared_ptr<RegServlet>;
    RegServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    std::string _name;     //用户名
    std::string _password; //密码
};

///登录
class UserLoginServlet : public Routn::Http::Servlet {
public:
    using ptr = std::shared_ptr<UserLoginServlet>;
    UserLoginServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;

private:
    std::string _name;  //用户名
    std::string _token; //用户token

};

///获取图书
class UserReadServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<UserReadServlet>;
    UserReadServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};

/// 借书
class UserBorrowServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<UserBorrowServlet>;
    UserBorrowServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};

///管理员API
class AdminPubServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<AdminPubServlet>;
    AdminPubServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    bool addBook(configor::json::value& book, std::string& ret, Redis::ptr redis_cli);
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};

class AdminDelServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<AdminDelServlet>;
    AdminDelServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    bool delBook(std::string& title, std::string& reason, Redis::ptr redis_cli);
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};

class AdminUpdateServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<AdminUpdateServlet>;
    AdminUpdateServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    bool updateBook(configor::json::value& book, Redis::ptr redis_cli);
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};

class AdminReadServlet : public Routn::Http::Servlet{
public:
    using ptr = std::shared_ptr<AdminReadServlet>;
    AdminReadServlet();

    int32_t handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session) override;
private:
    std::string _token;         //用户访问权限路径携带的token
    std::string _name;
};


#endif
