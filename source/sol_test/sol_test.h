#pragma once
#include "lua/lua.hpp"
#include "sol/sol.hpp"
#include <iostream>
#include <tuple>

#define INFO std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << __FUNCDNAME__ << std::endl;
//直接执行lua字符串
void test_string_exe()
{
	INFO;

	sol::state lua;
	// open some common libraries 加载必要的库
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('bark bark bark!')"); //直接执行字符串脚本

	std::cout << std::endl;
}



//原生lua_State 传递给sol
int use_sol2(lua_State* L)
{
	//对原生lua的视图
	sol::state_view lua(L);
	lua.script("print('bark bark bark!')");
	return 0;
}

int test_luaState_to_Sol()
{
	INFO;

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushcclosure(L, &use_sol2, 0);
	lua_setglobal(L, "lua_sol2");

	if (luaL_dostring(L, "lua_sol2"))
	{
		lua_error(L);
		return -1;
	}
	std::cout << "\n";
	return 0;
}

//执行lua文件
void test_exe_luaFile()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.script_file("../script/test.lua");

	sol::load_result test_script = lua.load_file("../script/test.lua");
	sol::load_result test_string_script = lua.load("return 'hello world'");

	sol::protected_function_result result1 = test_string_script();
	if (result1.valid())
	{
		std::cout << result1.get<std::string>() << std::endl;
	}
}



//向lua传递参数
void test_send_args_to_lua()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	const auto& my_script = R"(
local a,b,c = ...
print(a+b+c)
)";

	sol::load_result result = lua.load(my_script);
	if (!result.valid())
	{
		sol::error err = result;
		std::cerr << "ERROR:" << err.what() << std::endl;
	}
	result(1, 2, 3);
}





//表读取
void read_table()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.script(R"(
		abc = { [0] = 24 }
		def = { 
			ghi = { 
				bark = 50, 
				woof = abc 
			} 
		}
	)");

	sol::table abc = lua["abc"];
	sol::table def = lua["def"];
	sol::table ghi = lua["def"]["ghi"];

	int bark1 = def["ghi"]["bark"];
	int bark2 = lua["def"]["ghi"]["bark"];
	assert(bark1 == 50);
	assert(bark2 == 50);

	int abcval1 = abc[0];
	int abcval2 = ghi["woof"][0];
	assert(abcval1 == 24);
	assert(abcval2 == 24);
}


//构构建表
void test_create_table()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["abc_sol2"] = lua.create_table_with(
		0, 24
	); //直接创建一个表

	sol::table inner_table = lua.create_table_with("bark", 50,
		// can reference other existing stuff too
		"woof", lua["abc_sol2"]
	);
	lua.create_named_table("def_sol2",
		"ghi", inner_table
	);//在已经存在的表中插入变量
}


void test_exe_lua_func()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	//lua.script("function f (a, b, c) return a, b, c end");
	lua.script_file("../script/test2.lua");
	std::tuple<int, int, int> tup = lua["f"](100, 200, 300);

	std::cout << std::get<1>(tup) << std::endl;
	std::cout << std::get<2>(tup) << std::endl;
	std::cout << std::get<0>(tup) << std::endl;

	int a, b; std::string c;
	sol::tie(a, b, c) = lua["f"](1, 2, "hello");
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	std::cout << c << std::endl;

}

//函数绑定
void some_function() {
	std::cout << "some function!" << std::endl;
}

void some_other_function() {
	std::cout << "some other function!" << std::endl;
}

struct some_class {
	int variable = 30;

	double member_function() {
		return 24.5;
	}
};

void func_bind()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["f1"] = some_function;
	lua.set_function("f2", &some_other_function);

	lua.set("sc", some_class());
	lua["m1"] = &some_class::member_function;		//lua需要传递类实例
	lua.set_function("m2", &some_class::member_function, some_class{});	//传递进去一个类实例

	lua["v1"] = &some_class::variable;	//将成员变量绑定为函数

	some_class a;
	lua.set_function("v2", &some_class::variable, a);	//将成员变量和实例一块绑定

}

//绑定类
int  bind_class()
{
	INFO;
	struct my_class {
		int b = 24;

		int f() const {
			return 24;
		}

		void g() {
			++b;
		}
	};

	sol::state lua;
	lua.open_libraries();

	// "bark" namespacing in Lua
	// namespacing is just putting things in a table
	// forces creation if it does not exist
	auto bark = lua["bark"].get_or_create<sol::table>();
	// equivalent-ish:
	//sol::table bark = lua["bark"].force(); // forces table creation
	// equivalent, and more flexible:
	//sol::table bark = lua["bark"].get_or_create<sol::table>(sol::new_table());
	// equivalent, but less efficient/ugly:
	//sol::table bark = lua["bark"] = lua.get_or("bark", lua.create_table());
	bark.new_usertype<my_class>("my_class",
		"f", &my_class::f,
		"g", &my_class::g); // the usual

	// can add functions, as well (just like the global table)
	bark.set_function("print_my_class", [](my_class& self) { std::cout << "my_class { b: " << self.b << " }" << std::endl; });
	
	// this works
	lua.script("obj = bark.my_class.new()");
	lua.script("obj:g()");

	// calling this function also works
	lua.script("bark.print_my_class(obj)");
	my_class& obj = lua["obj"];
	assert(obj.b == 25);

	std::cout << std::endl;

	return 0;
}




