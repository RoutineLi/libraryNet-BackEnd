/*************************************************************************
	> File Name: book_servlet.cpp
	> Author: 
	> Mail: 
	> Created Time: 2022年11月28日 星期一 21时58分05秒
 ************************************************************************/

#include "book_servlet.h"

#include "../src/http/HTTP.h"
#include "../src/Macro.h"
#include "../src/IoManager.h"
#include "../src/Config.h"

#include <iomanip>
#include <random>
#include <ctime>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/base.h>

using namespace configor;

/**
 * 用户注册
 * 通过注册获取Token
*/

static Routn::Logger::ptr g_book = ROUTN_LOG_ROOT();
static std::string GetUUID() {
    static std::random_device rd;
    static std::uniform_int_distribution<uint64_t> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);
    uint64_t ab = dist(rd);
    uint64_t cd = dist(rd);
    uint32_t a, b, c, d;
    std::stringstream ss;
    ab = ( ab & 0xFFFFFFFFFFFF0FFFULL ) | 0x0000000000004000ULL;
    cd = ( cd & 0x3FFFFFFFFFFFFFFFULL ) | 0x8000000000000000ULL;
    a  = ( ab >> 32U );
    b  = ( ab & 0xFFFFFFFFU);
    c  = ( cd >> 32U );
    d  = ( cd & 0xFFFFFFFFU);
    ss << std::hex << std::nouppercase << std::setfill('0');
    ss << std::setw(8) << (a) << '-';
    ss << std::setw(4) << (b >> 16U) << '-';
    ss << std::setw(4) << (b & 0xFFFFU) << '-';
    ss << std::setw(4) << (c >> 16U) << '-';
    ss << std::setw(4) << (c & 0xFFFFU);
    ss << std::setw(8) << d;
    return ss.str();
}



static json::value serialize(std::vector<book::st_BookDefine>& vec){
    json::value arr = json::array{};
    for(auto i : vec){
        json::value t = json::object{
            {"id", i.id},
            {"title", i.title},
            {"category", i.category},
            {"return_time", i.getReturnTime()},
            {"book_borrowers_id", i.getBorrows()},
            {"bookshelf_location", i.location},
            {"count", i.count} 
        };
        arr.push_back(t);
    }
    return arr;
}


static int checkValid(Routn::Http::HttpRequest::ptr request,
            Routn::Http::HttpResponse::ptr response, Routn::Http::HttpMethod method, Redis::ptr redis_cli){
    ROUTN_LOG_INFO(g_book) << "req: "<< request->toString();
    if(request->getMethod() != method){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;        
    }
    if(request->getHeader("content-type", "application/json").empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }  

    if(request->getHeader("Authorization").empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }

    if(!redis_cli->connect()){
        response->setStatus(Routn::Http::HttpStatus::BAD_GATEWAY);
        response->setHeader("content-type", "text/html");
        response->setHtmlBody("/home/vrvuser/Book-System/bin/html/502.html");
        return 0;
    }

    std::string token = request->getHeader("Authorization");

    auto decoded = jwt::decode(token);

    std::string name = decoded.get_payload_claim("name").as_string();

    if(token != redis_cli->getField(name, "token")){
        response->setStatus(Routn::Http::HttpStatus::FORBIDDEN);
        response->setHeader("content-type", "text/html");
        response->setHtmlBody("/home/vrvuser/Book-System/bin/html/index.html");
        return 0;
    }

    ROUTN_LOG_INFO(g_book) << "got Authorized token = " << token << ", name = " << name;    
    return 1;
}

/// @brief 用户模块 
RegServlet::RegServlet()
    : Routn::Http::Servlet("register/1.0"){

}

int32_t RegServlet::handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session)
{
    ROUTN_LOG_INFO(g_book) << "req: "<< request->toString();

    if(request->getMethod() != Routn::Http::HttpMethod::POST){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    if(request->getHeader("content-type", "application/json").empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    std::string body = request->getBody();
    if(body.empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    json::value json = json::parse(body);
    std::string name = json["name"];
    if(name.empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    std::string password = json["password"];
    if(password.empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }

    std::string id = GetUUID();

    ROUTN_LOG_INFO(g_book) << "got user data, name = " << name << ", password = " << password 
        << ", id = " << id;

    auto token = jwt::create()
        .set_algorithm("HS256")
        .set_type("JWS")
        .set_payload_claim("name", jwt::claim(std::string(name)))
        .set_payload_claim("password", jwt::claim(std::string(password)))
        .sign(jwt::algorithm::hs256{"secret"});    

    json.clear();
    
    json = json::object{
        {"token", token},
        {"meta", json::object{
            {"msg", "注册成功"},
            {"status", 200}
        }}
    };
    Redis::ptr redis_cli(new Redis);
    if(!redis_cli->connect("127.0.0.1", 6379)){
        response->setStatus(Routn::Http::HttpStatus::BAD_GATEWAY);
        json = json::object{
            {"meta", json::object{
                {"msg", "服务器开小差了，请重试"},
                {"status", 502}
            }}
        };  
        response->setBody(json::dump(json));
        return 0;
    }

    if(redis_cli->getField(name, "password") == password){
        json = json::object{
            {"token", ""},
            {"meta", json::object{
                {"msg", "用户已注册"},
                {"status", 200}
            }}
        };    
        response->setBody(json::dump(json));
        return 0;
    } 
    redis_cli->createUser(name, id, password, token);
    redis_cli->saveConf();

    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setHeader("content-type", "application/json");
    response->setBody(json::dump(json));

    ROUTN_LOG_INFO(g_book) << "rsp: " << response->toString();

    return 0;
}


/**
 * 用户登录
*/
UserLoginServlet::UserLoginServlet()
    : Routn::Http::Servlet("login/1.0")
    , _name("routn"){
    
}



int32_t UserLoginServlet::handle(Routn::Http::HttpRequest::ptr request,
               Routn::Http::HttpResponse::ptr response,
               Routn::Http::HttpSession::ptr session)
{
    ROUTN_LOG_INFO(g_book) << "req: "<< request->toString();
    if(request->getMethod() != Routn::Http::HttpMethod::GET){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;        
    }
    if(request->getHeader("content-type", "application/json").empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    if(request->getQuery().empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    std::string name, password;
    auto name_pos = request->getQuery().find_first_of('=');
    auto divide = request->getQuery().find_last_of('&');
    auto pass_pos = request->getQuery().find_last_of('=');

    name = request->getQuery().substr(name_pos + 1, divide - (name_pos + 1));
    password = request->getQuery().substr(pass_pos + 1, divide - pass_pos);
    
    //printf(L_GREEN"name = %s, password = %s\n" _NONE, name.c_str(), password.c_str());

    if(name.empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    if(password.empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    Redis::ptr redis_cli(new Redis);

    if(!redis_cli->connect()){
        return 0;
    }
    std::string id, cmp_password;
    json::value json;
    id = redis_cli->getField(name, "id");
    cmp_password = redis_cli->getField(name, "password");
    
    //std::cout << L_GREEN << cmp_password << _NONE << std::endl;
    
    if(!id.empty()){
        if(password == cmp_password){    
            json = json::object{
                {"user", json::object{{"id", id.c_str()}, {"name", name}}},
                {"meta", json::object{{"msg", "登录成功"}, {"status", 200}}}
            };
        }else{
            json = json::object{
                {"user", json::object{{"id", -1}, {"name", ""}}},
                {"meta", json::object{{"msg", "密码或用户名错误"}, {"status", 200}}}
            };    
        }
    }else{
        json = json::object{
            {"user", json::object{{"id", -1}, {"name", name}}},
            {"meta", json::object{{"msg", "用户未注册"}, {"status", 200}}}
        };       
    }

    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setHeader("content-type", "application/json");
    response->setBody(json::dump(json));

    ROUTN_LOG_INFO(g_book) << "rsp: " << response->toString();
    return 0;
}

/// @brief 获取图书
UserReadServlet::UserReadServlet()
    : Servlet("admin/1.0"){
}

int32_t UserReadServlet::handle(Routn::Http::HttpRequest::ptr request,
                Routn::Http::HttpResponse::ptr response,
                Routn::Http::HttpSession::ptr session){
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::GET, redis_cli) == 0){
        return 0;
    }    
    auto vec = redis_cli->getBooks();    
    json::value arr = json::array{};
    for(auto i : vec){
        json::value t = json::object{
            {"id", i.id},
            {"title", i.title},
            {"category", i.category},
            {"return_time", i.getReturnTime()},
            {"bookshelf_location", i.location},
        };
        arr.push_back(t);
    }
    json::value json = json::object{
        {"books", arr},
        {"meta", json::object{
            {"msg", "查询图书成功"},
            {"status", 200}                    
        }}
    };    
    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json)); 
    return 0;
}


UserBorrowServlet::UserBorrowServlet()
    : Servlet("borrow/1.0"){

}


int32_t UserBorrowServlet::handle(Routn::Http::HttpRequest::ptr request,
                Routn::Http::HttpResponse::ptr response,
                Routn::Http::HttpSession::ptr session){
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::GET, redis_cli) == 0){
        return 0;
    }    
    std::string query = request->getQuery();
    auto pos_1 = query.find_first_of('=');
    auto div_1 = query.find_first_of('&');
    auto pos_2 = query.find_first_of('=', div_1);
    auto div_2 = query.find_last_of('&');
    auto pos_3 = query.find_last_of('=');
    std::string title = query.substr(pos_1 + 1, div_1 - (pos_1 + 1));
    std::string user_id = query.substr(pos_2 + 1, div_2 - (pos_2 + 1));
    std::string return_time = query.substr(pos_3 + 1);

    ROUTN_LOG_INFO(g_book) << "got query: [ title = " << title << ", user_id = " << user_id << ", returnTime = " << return_time << " ]";
    bool ret = false;
    int count = 0;
    auto book_id = redis_cli->getField(title, "id");
    if(redis_cli->isRangeMember(book_id, user_id)){
        json::value json = json::object{
            {"meta", json::object{
                {"msg", "您已经借阅过该书"},
                {"status", 200}                    
            }}
        };    
        response->setHeader("content-type", "application/json");
        response->setStatus(Routn::Http::HttpStatus::OK);
        response->setBody(json::dump(json));    
        return 0;        
    }
    if((count = atoi(redis_cli->getField(title, "count").c_str())) != 0){
        ret = redis_cli->zaddOP(book_id, return_time, user_id);
        if(!ret){
            response->setStatus(Routn::Http::HttpStatus::BAD_GATEWAY);
            response->setHeader("content-type", "text/html");
            response->setHtmlBody("/home/vrvuser/Book-System/bin/html/502.html");
            return 0;
        }
        redis_cli->setVal(title, "count", std::to_string(--count));
        json::value json = json::object{
            {"meta", json::object{
                {"msg", "借阅图书成功"},
                {"status", 200}                    
            }}
        };
        //TODO添加定时器任务
        //auto this_iom = Routn::IOManager::GetThis();
        //uint64_t timeout = time(0) - atoi(return_time.c_str());
        //this_iom->addTimer(timeout, [=](){
        //    redis_cli->zremOP(book_id, user_id);
        //    ROUTN_LOG_INFO(g_book) << "A book [ " << title << " ] which borrowd is in timout-state";
        //}, false);
        response->setHeader("content-type", "application/json");
        response->setStatus(Routn::Http::HttpStatus::OK);
        response->setBody(json::dump(json)); 
        return 0;
    }
         
    json::value json = json::object{
        {"meta", json::object{
            {"msg", "该图书已被借走"},
            {"status", 200}                    
        }}
    };    
    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json));    
    return 0;
}


/**
 * @brief 管理员添加图书API
 * 
 */

AdminPubServlet::AdminPubServlet()
    : Routn::Http::Servlet("admin/1.0"){
}

int32_t AdminPubServlet::handle(Routn::Http::HttpRequest::ptr request,
                Routn::Http::HttpResponse::ptr response,
                Routn::Http::HttpSession::ptr session){
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::POST, redis_cli) == 0){
        return 0;
    }

    if(request->getBody().empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }

    json::value json = json::parse(request->getBody());
    std::string ret = "添加图书成功";
    addBook(json, ret, redis_cli);            
    json.clear();
    json = json::object{
        {"meta", json::object{
            {"msg", ret}, 
            {"status", 200}}
        }
    };
    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json));
    return 0;

}

bool AdminPubServlet::addBook(json::value& book, std::string& ret, Redis::ptr redis_cli){
    std::string title = book["title"];
    std::string body = book["body"];
    std::string category = book["category"];
    std::string bookshelf_location = book["bookshelf_location"];
    uint32_t count = book["count"];
    auto id = GetUUID();
    if(redis_cli->getField(title, "body") == body && redis_cli->getField(title, "category") == category){
        ret = "该书已经添加";
        return false;
    }
    redis_cli->createBook(title
        , id
        , body, category
        , bookshelf_location
        , std::to_string(count));
    redis_cli->saveConf();
    return true;
}



AdminDelServlet::AdminDelServlet()
    : Routn::Http::Servlet("admin/1.0"){
}

int32_t AdminDelServlet::handle(Routn::Http::HttpRequest::ptr request,
                   Routn::Http::HttpResponse::ptr response,
                   Routn::Http::HttpSession::ptr session){    
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::GET, redis_cli) == 0){
        return 0;
    }

    std::string title = request->getQuery();
    auto pos = title.find_first_of('=');
    title = title.substr(pos + 1);
    

    std::string reason;
    delBook(title, reason, redis_cli);
    json::value json = json::object{
        {"meta", json::object{
            {"msg", reason}, 
            {"status", 200}}
        }
    };

    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json));      

    return 0;
}

bool AdminDelServlet::delBook(std::string& title, std::string& ret, Redis::ptr redis_cli){
    if(!redis_cli->getKey(title)){
        ret = "没有找到该书相关信息";
        return false;
    }
    if(!redis_cli->delBook(title)){
        ret = "删除失败";
        return false;
    }
    ret = "删除图书成功";
    return true;
}


AdminUpdateServlet::AdminUpdateServlet()
    : Servlet("admin/1.0"){

}

int32_t AdminUpdateServlet::handle(Routn::Http::HttpRequest::ptr request,
                Routn::Http::HttpResponse::ptr response,
                Routn::Http::HttpSession::ptr session){
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::POST, redis_cli) == 0){
        return 0;
    }

    if(request->getBody().empty()){
        response->setStatus(Routn::Http::HttpStatus::BAD_REQUEST);
        return 0;
    }
    std::string update_val = request->getBody();
    json::value json = json::parse(update_val);
    std::string ret;
    if(updateBook(json, redis_cli)){
        json.clear();
        json = json::object{
            {"meta", json::object{
                    {"msg", "修改图书成功"},
                    {"status", 200}
                }
            }
        };
        response->setHeader("content-type", "application/json");
        response->setStatus(Routn::Http::HttpStatus::OK);
        response->setBody(json::dump(json)); 
        return 0;
    }

    json.clear();
    json = json::object{
        {"meta", json::object{
                {"msg", "修改图书失败"},
                {"status", 200}
            }
        }
    };
    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json)); 
    return 0;
}

bool AdminUpdateServlet::updateBook(configor::json::value& book, Redis::ptr redis_cli){
    std::string tkey = book["title"];
    size_t pos = 0;
    if((pos = tkey.find_last_of('&')) != tkey.npos){
        std::string oldv = tkey.substr(0, pos);
        std::string newv = tkey.substr(pos + 1);
        ROUTN_LOG_DEBUG(g_book) << "title = " << oldv << ", new title = " << newv;
        if(!redis_cli->setBookTitle(oldv, newv)){
            return false;
        }
        tkey = newv;
    }
    if(!redis_cli->getKey(tkey)){
        return false;
    }
    if(!book["body"].is_null()){
        std::string body = book["body"];
        if(!redis_cli->setVal(tkey, "body", body)){
            return false;
        }
    }
    if(!book["category"].is_null()){
        std::string category = book["category"];
        if(!redis_cli->setVal(tkey, "category", category)){
            return false;
        }            
    }
    if(!book["bookshelf _location"].is_null()){
        std::string location = book["bookshelf _location"];
        if(!redis_cli->setVal(tkey, "location", location)){
            return false;
        }            
    }
    if(!book["count"].is_null()){
        int count = book["count"];
        if(!redis_cli->setVal(tkey, "count", std::to_string(count))){
            return false;
        }            
    }
    return true;
}

AdminReadServlet::AdminReadServlet()
    : Servlet("admin/1.0"){
}

int32_t AdminReadServlet::handle(Routn::Http::HttpRequest::ptr request,
                Routn::Http::HttpResponse::ptr response,
                Routn::Http::HttpSession::ptr session){
    Redis::ptr redis_cli(new Redis);
    if(checkValid(request, response, Routn::Http::HttpMethod::GET, redis_cli) == 0){
        return 0;
    }

    std::string title = request->getQuery();
    auto pos = title.find_first_of('=');
    title = title.substr(pos + 1);
    
    std::cout << L_GREEN << title << _NONE <<std::endl;
    if(title == "all"){
        auto vec = redis_cli->getBooks();
        json::value json = json::object{
            {"books", serialize(vec)},
            {"meta", json::object{
                {"msg", "查询图书成功"},
                {"status", 200}                    
            }}
        };    
        response->setHeader("content-type", "application/json");
        response->setStatus(Routn::Http::HttpStatus::OK);
        response->setBody(json::dump(json)); 
        return 0;
    }
    book::st_BookDefine st_book;
    json::value json;
    if(!redis_cli->getBook(title, st_book)){
        json = json::object{
            {"meta", json::object{
                {"msg", "未找到相关图书"},
                {"status", 200}
            }}
        };
        response->setHeader("content-type", "application/json");
        response->setStatus(Routn::Http::HttpStatus::OK);
        response->setBody(json::dump(json)); 
        return 0;
    }
    json = json::object{
        {"books", json::object{
            {"id", st_book.id},
            {"title", st_book.title},
            {"body", st_book.body},
            {"category", st_book.category},
            {"return_time", st_book.getReturnTime()},
            {"book_borrowers_id", st_book.getBorrows()},
            {"bookshelf_location", st_book.location},
            {"count", st_book.count}
        }},
        {"meta", json::object{
                {"msg", "查询图书成功"},
                {"status", 200}
        }} 
    };
    response->setHeader("content-type", "application/json");
    response->setStatus(Routn::Http::HttpStatus::OK);
    response->setBody(json::dump(json)); 
    return 0;
}

