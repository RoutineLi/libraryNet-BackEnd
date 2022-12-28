/*************************************************************************
	> File Name: book_module.cpp
	> Author: 
	> Mail: 
	> Created Time: 2022年11月28日 星期一 18时34分02秒
 ************************************************************************/

#include "book_module.h"
#include "book_servlet.h"

#include "../src/Log.h"
#include "../src/Application.h"
#include "../src/Config.h"
#include "../src/Env.h"


static Routn::Logger::ptr root = ROUTN_LOG_ROOT();

BookModule::BookModule()
	: Module("book-system", "1.0", ""){

}

bool BookModule::onLoad() {
	return true;
}

bool BookModule::onUnload() {
	return false;
}

bool BookModule::onServerUp() {
	return true;
}

bool BookModule::onServerReady() {
	std::vector<Routn::TcpServer::ptr> svrs;
	if(!Routn::Application::GetInstance()->getServer("http", svrs)){
		ROUTN_LOG_WARN(root) << "no http-server exist!";
		return false;
	}

	for(auto& i : svrs){
		Routn::Http::HttpServer::ptr sever = std::dynamic_pointer_cast<Routn::Http::HttpServer>(i);
		if(!i){
			continue;
		}

		auto slt = sever->getServletDispatch();
		
		//set 404 pages
		Routn::Http::FunctionServlet::ptr index = std::make_shared<Routn::Http::FunctionServlet>([](Routn::Http::HttpRequest::ptr request, 
					Routn::Http::HttpResponse::ptr response,
					Routn::Http::HttpSession::ptr session){
			std::ifstream fs;
			fs.open("/home/vrvuser/Book-System/bin/html/index2.html");
			std::string buff;
			if(fs.is_open()){
				std::string tmp;
				while(getline(fs, tmp)){
					buff += tmp;
				}
			}
			fs.close();
			response->setBody(buff);
			return 0;
		});

		slt->setDefault(index);

		RegServlet::ptr regslt(new RegServlet);
		UserLoginServlet::ptr loginslt(new UserLoginServlet);
		UserReadServlet::ptr readslt(new UserReadServlet);
		UserBorrowServlet::ptr borrowslt(new UserBorrowServlet);

		AdminPubServlet::ptr adminpubslt(new AdminPubServlet);
		AdminDelServlet::ptr admindelslt(new AdminDelServlet);
		AdminUpdateServlet::ptr adminupslt(new AdminUpdateServlet);
		AdminReadServlet::ptr adminreadslt(new AdminReadServlet);

		slt->addServlet("/api/public/v1/users/registered", regslt);
		slt->addServlet("/api/public/v1/users/login", loginslt);
		slt->addServlet("/api/public/v1/book/user", readslt);
		slt->addServlet("/api/public/v1/user/borrow", borrowslt);
		
		slt->addServlet("/api/public/v1/book/publish", adminpubslt);
		slt->addServlet("/api/public/v1/book/delete", admindelslt);
		slt->addServlet("/api/public/v1/book/update", adminupslt);
		slt->addServlet("/api/public/v1/book/read", adminreadslt);
	}
	return true;
}

extern "C"{
	Routn::Module* CreateModule(){
		Routn::Module* mod = new BookModule;
		ROUTN_LOG_INFO(root) << "Create BookModule Success!";
		return mod;
	}

	void DestroyModule(Routn::Module* module){
		ROUTN_LOG_INFO(root) << "Destroy BookModule";
		delete module;
	}
	
}

