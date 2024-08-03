#pragma once
#include "lua/lua.hpp"
#include "sol/sol.hpp"
#include <iostream>
#include <tuple>

#define INFO std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << __FUNCDNAME__ << std::endl;
//ֱ��ִ��lua�ַ���
void test_string_exe()
{
	INFO;

	sol::state lua;
	// open some common libraries ���ر�Ҫ�Ŀ�
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('bark bark bark!')"); //ֱ��ִ���ַ����ű�

	std::cout << std::endl;
}



//ԭ��lua_State ���ݸ�sol
int use_sol2(lua_State* L)
{
	//��ԭ��lua����ͼ
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

//ִ��lua�ļ�
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



//��lua���ݲ���
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





//���ȡ
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


//��������
void test_create_table()
{
	INFO;
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["abc_sol2"] = lua.create_table_with(
		0, 24
	); //ֱ�Ӵ���һ����

	sol::table inner_table = lua.create_table_with("bark", 50,
		// can reference other existing stuff too
		"woof", lua["abc_sol2"]
	);
	lua.create_named_table("def_sol2",
		"ghi", inner_table
	);//���Ѿ����ڵı��в������
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

//������
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
	lua["m1"] = &some_class::member_function;		//lua��Ҫ������ʵ��
	lua.set_function("m2", &some_class::member_function, some_class{});	//���ݽ�ȥһ����ʵ��

	lua["v1"] = &some_class::variable;	//����Ա������Ϊ����

	some_class a;
	lua.set_function("v2", &some_class::variable, a);	//����Ա������ʵ��һ���

}

//����
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




