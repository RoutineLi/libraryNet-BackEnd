# 图书管理系统 后端开发 基于routn1.04框架



## 数据库选型--redis
redis-cli --raw 
# 存储采取哈希结构存储
	id属性不重复可以作为key
```
ex:

$  hset 201907020518 "routn" password "vrv123456" _token = "jwt-example"
$  (integer)3
$  hset 213214 title "高等数学" body "考研必备科目" category "教材"
	ret-time "2023/11/28" BorrowId "201907020518" Location "图书馆"
	total num "15"
$  (integer)7

```
# HGET 查询
```
$  hget user1 name
$  "routn"
$  hget book1 title
$  "高等数学"
```
# HSET 修改
```
$  hset user1 name "lpj"
$  (integer)0
$  hget user1 name
$  "lpj"

使用github开源的hiredis库解析并连接数据库

```



# jwt-token解析采用 jwt-cpp

开源地址： [Thalhammer/jwt-cpp: A header only library for creating and validating json web tokens in c++ (github.com)](https://github.com/Thalhammer/jwt-cpp) 



# JSON解析采用configor-cpp

开源地址： [Nomango/configor: A light weight configuration library for C++ (github.com)](https://github.com/Nomango/configor) 



# BASE64编码以及SHA256算法采用OPENSSL库

