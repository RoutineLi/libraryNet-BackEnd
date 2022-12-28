/*************************************************************************
	> File Name: book_module.h
	> Author: lpj
	> Mail: lipeijie@vrvmail.com.cn
	> Created Time: 2022年11月28日 星期一 13时27分11秒
 ************************************************************************/

#ifndef _BOOK_MODULE_H
#define _BOOK_MODULE_H

#include "../src/Module.h"

#include <memory>

using namespace Routn;

class BookModule : public Module{
public:
	using ptr = std::shared_ptr<BookModule>;
	BookModule();

	bool onLoad() override;
	bool onUnload() override;
	bool onServerReady() override;
	bool onServerUp() override;
};

#endif
